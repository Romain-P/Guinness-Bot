// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/Injector.hpp>
#include <HadesMemory/Module.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Config.hpp>
#include <HadesMemory/Detail/WinAux.hpp>
#include <HadesMemory/Detail/ArgQuote.hpp>
#include <HadesMemory/Detail/StringBuffer.hpp>
#include <HadesMemory/Detail/EnsureCleanup.hpp>

#include <Windows.h>

#include <iterator>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#ifdef HADES_MSVC
#pragma warning(push, 1)
#endif
#ifdef HADES_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-pedantic"
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <AsmJit/AsmJit.h>
#include <iostream>

#ifdef HADES_MSVC
#pragma warning(pop)
#endif
#ifdef HADES_GCC
#pragma GCC diagnostic pop
#endif

namespace HadesMem
{
  // Constructor
  Injector::Injector(MemoryMgr const& MyMemory) 
    : m_Memory(MyMemory)
  { }
      
  // Copy constructor
  Injector::Injector(Injector const& Other)
    : m_Memory(Other.m_Memory)
  { }
  
  // Copy assignment operator
  Injector& Injector::operator=(Injector const& Other)
  {
    this->m_Memory = Other.m_Memory;
    
    return *this;
  }
  
  // Move constructor
  Injector::Injector(Injector&& Other)
    : m_Memory(std::move(Other.m_Memory))
  { }
  
  // Move assignment operator
  Injector& Injector::operator=(Injector&& Other)
  {
    this->m_Memory = std::move(Other.m_Memory);
    
    return *this;
  }
  
  // Destructor
  Injector::~Injector()
  { }

  // Inject DLL
  HMODULE Injector::InjectDll(std::wstring const& Path, 
    InjectFlags Flags) const
  {
    // Do not continue if Shim Engine is enabled for local process, 
    // otherwise it could interfere with the address resolution.
    HMODULE const ShimEngMod = GetModuleHandle(L"ShimEng.dll");
    if (ShimEngMod)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Injector::InjectDll") << 
        ErrorString("Shims enabled for local process."));
    }
    
    boost::filesystem::path PathReal(Path);
      
    bool PathResolution = ((Flags & InjectFlag_PathResolution) == 
      InjectFlag_PathResolution);
    
    if (PathResolution && PathReal.is_relative())
    {
      PathReal = boost::filesystem::absolute(PathReal, 
        Detail::GetSelfDirPath());
    }
    
    PathReal.make_preferred();

    // Ensure target file exists
    // Note: Only performing this check when path resolution is enabled, 
    // because otherwise we would need to perform the check in the context 
    // of the remote process, which is not possible to do without 
    // introducing race conditions and other potential problems. So we just 
    // let LoadLibraryW do the check for us.
    if (PathResolution && !boost::filesystem::exists(PathReal))
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Injector::InjectDll") << 
        ErrorString("Could not find module file."));
    }
    
    std::wstring const PathString(PathReal.native());
    std::size_t const PathBufSize = (PathString.size() + 1) * 
      sizeof(wchar_t);
    
    AllocAndFree const LibFileRemote(m_Memory, PathBufSize);
    m_Memory.WriteString(LibFileRemote.GetBase(), PathString);
    
    Module Kernel32Mod(m_Memory, L"kernel32.dll");
    FARPROC const pLoadLibraryW = Kernel32Mod.FindProcedure("LoadLibraryW");
    DWORD_PTR pLoadLibraryWTemp = reinterpret_cast<DWORD_PTR>(pLoadLibraryW);
    
    std::vector<PVOID> Args;
    Args.push_back(LibFileRemote.GetBase());
    MemoryMgr::RemoteFunctionRet RemoteRet = m_Memory.Call(
      reinterpret_cast<PVOID>(pLoadLibraryWTemp), 
      MemoryMgr::CallConv_Default, Args);
    if (!RemoteRet.GetReturnValue())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Injector::InjectDll") << 
        ErrorString("Call to LoadLibraryW in remote process failed.") << 
        ErrorCodeWinLast(RemoteRet.GetLastError()));
    }
    
    return reinterpret_cast<HMODULE>(RemoteRet.GetReturnValue());
  }

  // Free DLL
  void Injector::FreeDll(HMODULE ModuleRemote) const
  {
    Module Kernel32Mod(m_Memory, L"kernel32.dll");
    FARPROC const pFreeLibrary = Kernel32Mod.FindProcedure("FreeLibrary");
    DWORD_PTR pFreeLibraryTemp = reinterpret_cast<DWORD_PTR>(pFreeLibrary);
    
    std::vector<PVOID> Args;
    Args.push_back(reinterpret_cast<PVOID>(ModuleRemote));
    MemoryMgr::RemoteFunctionRet RemoteRet = m_Memory.Call(
      reinterpret_cast<PVOID>(pFreeLibraryTemp), 
      MemoryMgr::CallConv_Default, Args);
    if (!RemoteRet.GetReturnValue())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Injector::FreeDll") << 
        ErrorString("Call to FreeLibrary in remote process failed.") << 
        ErrorCodeWinLast(RemoteRet.GetLastError()));
    }
  }

  // Call export
  MemoryMgr::RemoteFunctionRet Injector::CallExport(
    HMODULE RemoteModule, std::string const& Export) const
  {
    Module TargetMod(m_Memory, RemoteModule);
    FARPROC const pExportAddr = TargetMod.FindProcedure(Export);
    DWORD_PTR const pExportAddrTemp = reinterpret_cast<DWORD_PTR>(
      pExportAddr);
    
    std::vector<PVOID> ExportArgs;
    ExportArgs.push_back(RemoteModule);
    return m_Memory.Call(reinterpret_cast<PVOID>(pExportAddrTemp), 
      MemoryMgr::CallConv_Default, ExportArgs);
  }
  
  // Equality operator
  bool Injector::operator==(Injector const& Rhs) const
  {
    return m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool Injector::operator!=(Injector const& Rhs) const
  {
    return !(*this == Rhs);
  }

  // Constructor
  CreateAndInjectData::CreateAndInjectData(MemoryMgr const& MyMemory, 
    HMODULE Module, DWORD_PTR ExportRet, DWORD ExportLastError) 
    : m_Memory(MyMemory), 
    m_Module(Module), 
    m_ExportRet(ExportRet), 
    m_ExportLastError(ExportLastError)
  { }
  
  // Copy constructor
  CreateAndInjectData::CreateAndInjectData(CreateAndInjectData const& Other)
    : m_Memory(Other.m_Memory), 
    m_Module(Other.m_Module), 
    m_ExportRet(Other.m_ExportRet), 
    m_ExportLastError(Other.m_ExportLastError)
  { }
  
  // Copy assignment operator
  CreateAndInjectData& CreateAndInjectData::operator=(
    CreateAndInjectData const& Other)
  {
    this->m_Memory = Other.m_Memory;
    this->m_Module = Other.m_Module;
    this->m_ExportRet = Other.m_ExportRet;
    this->m_ExportLastError = Other.m_ExportLastError;
    
    return *this;
  }
  
  // Move constructor
  CreateAndInjectData::CreateAndInjectData(CreateAndInjectData&& Other)
    : m_Memory(std::move(Other.m_Memory)), 
    m_Module(Other.m_Module), 
    m_ExportRet(Other.m_ExportRet), 
    m_ExportLastError(Other.m_ExportLastError)
  {
    Other.m_Module = nullptr;
    Other.m_ExportRet = 0;
    Other.m_ExportLastError = 0;
  }
  
  // Move assignment operator
  CreateAndInjectData& CreateAndInjectData::operator=(
    CreateAndInjectData&& Other)
  {
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_Module = Other.m_Module;
    Other.m_Module = nullptr;
    
    this->m_ExportRet = Other.m_ExportRet;
    Other.m_ExportRet = 0;
    
    this->m_ExportLastError = Other.m_ExportLastError;
    Other.m_ExportLastError = 0;
    
    return *this;
  }
  
  // Destructor
  CreateAndInjectData::~CreateAndInjectData()
  { }
  
  // Get memory manager
  MemoryMgr CreateAndInjectData::GetMemoryMgr() const
  {
    return m_Memory;
  }
  
  // Get module
  HMODULE CreateAndInjectData::GetModule() const
  {
    return m_Module;
  }
  
  // Get export return value
  DWORD_PTR CreateAndInjectData::GetExportRet() const
  {
    return m_ExportRet;
  }
  
  // Get export last error code
  DWORD CreateAndInjectData::GetExportLastError() const
  {
    return m_ExportLastError;
  }
  
  // Create process (as suspended) and inject DLL
  CreateAndInjectData CreateAndInject(
    std::wstring const& Path, 
    std::wstring const& WorkDir, 
    std::vector<std::wstring> const& Args, 
    std::wstring const& Module, 
    std::string const& Export, 
    Injector::InjectFlags Flags)
  {
    boost::filesystem::path const PathReal(Path);
    
    std::wstring CommandLine;
    Detail::ArgvQuote(PathReal.native(), CommandLine, false);
    std::for_each(std::begin(Args), std::end(Args), 
      [&] (std::wstring const& Arg) 
      {
        CommandLine += L' ';
        Detail::ArgvQuote(Arg, CommandLine, false);
      });
    std::vector<wchar_t> ProcArgs(std::begin(CommandLine), 
      std::end(CommandLine));
    ProcArgs.push_back(L'\0');
    
    boost::filesystem::path WorkDirReal;
    if (!WorkDir.empty())
    {
      WorkDirReal = WorkDir;
    }
    else if (PathReal.has_parent_path())
    {
      WorkDirReal = PathReal.parent_path();
    }
    else
    {
      WorkDirReal = L"./";
    }

    STARTUPINFO StartInfo;
    ZeroMemory(&StartInfo, sizeof(StartInfo));
    StartInfo.cb = sizeof(StartInfo);
    PROCESS_INFORMATION ProcInfo;
    ZeroMemory(&ProcInfo, sizeof(ProcInfo));
    if (!CreateProcess(PathReal.c_str(), ProcArgs.data(), nullptr, nullptr, FALSE, 
      CREATE_SUSPENDED, nullptr, WorkDirReal.c_str(), &StartInfo, &ProcInfo))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Injector::Error() << 
        ErrorFunction("CreateAndInject") << 
        ErrorString("Could not create process.") << 
        ErrorCodeWinLast(LastError));
    }
    
    Detail::EnsureCloseHandle const ProcHandle(ProcInfo.hProcess);
    Detail::EnsureCloseHandle const ThreadHandle(ProcInfo.hThread);

    try
    {
      MemoryMgr const MyMemory(ProcInfo.dwProcessId);
      
      // Create Assembler.
      // This is used to generate a 'nullsub' function, which is called in 
      // the context of the remote process in order to 'force' a call to 
      // ntdll.dll!LdrInitializeThunk. This is necessary because module 
      // enumeration will fail if LdrInitializeThunk has not been called, 
      // and Injector::InjectDll (and the APIs it uses) depend on the 
      // module enumeration APIs.
      AsmJit::Assembler MyJitFunc;

#if defined(_M_AMD64) 
      // Return
      MyJitFunc.ret();
#elif defined(_M_IX86) 
      // Return
      MyJitFunc.ret(AsmJit::Imm(0x4));
#else 
#error "[HadesMem] Unsupported architecture."
#endif
      
      DWORD_PTR const StubSize = MyJitFunc.getCodeSize();
      
      AllocAndFree const StubMemRemote(MyMemory, StubSize);
      PBYTE pRemoteStub = static_cast<PBYTE>(StubMemRemote.GetBase());
      DWORD_PTR pRemoteStubTemp = reinterpret_cast<DWORD_PTR>(pRemoteStub);
      
      std::vector<BYTE> CodeReal(StubSize);
      MyJitFunc.relocCode(CodeReal.data(), reinterpret_cast<DWORD_PTR>(
        pRemoteStub));
      
      MyMemory.WriteList(pRemoteStub, CodeReal);
      
      Detail::EnsureCloseHandle const MyThread(CreateRemoteThread(
        MyMemory.GetProcessHandle(), nullptr, 0, 
        reinterpret_cast<LPTHREAD_START_ROUTINE>(pRemoteStubTemp), nullptr, 
        0, nullptr));
      if (!MyThread)
      {
        DWORD const LastError = GetLastError();
        BOOST_THROW_EXCEPTION(Injector::Error() << 
          ErrorFunction("CreateAndInject") << 
          ErrorString("Could not create remote thread.") << 
          ErrorCodeWinLast(LastError));
      }
      
      if (WaitForSingleObject(MyThread, INFINITE) != WAIT_OBJECT_0)
      {
        DWORD const LastError = GetLastError();
        BOOST_THROW_EXCEPTION(Injector::Error() << 
          ErrorFunction("CreateAndInject") << 
          ErrorString("Could not wait for remote thread.") << 
          ErrorCodeWinLast(LastError));
      }
      
      Injector const MyInjector(MyMemory);
      HMODULE const ModBase = MyInjector.InjectDll(Module, Flags);
      
      MemoryMgr::RemoteFunctionRet ExpRetData(0, 0, 0);
      if (!Export.empty())
      {
        ExpRetData = MyInjector.CallExport(ModBase, Export);
      }
      
      if (ResumeThread(ProcInfo.hThread) == static_cast<DWORD>(-1))
      {
        DWORD const LastError = GetLastError();
        BOOST_THROW_EXCEPTION(Injector::Error() << 
          ErrorFunction("CreateAndInject") << 
          ErrorString("Could not resume process.") << 
          ErrorCodeWinLast(LastError) << 
          ErrorCodeWinRet(ExpRetData.GetReturnValue()) << 
          ErrorCodeWinOther(ExpRetData.GetLastError()));
      }
      
      return CreateAndInjectData(MyMemory, ModBase, 
        ExpRetData.GetReturnValue(), ExpRetData.GetLastError());
    }
    catch (std::exception const& /*e*/)
    {
      // Terminate process if injection failed, otherwise the 'zombie' process 
      // would be leaked.
      TerminateProcess(ProcInfo.hProcess, 0);
      
      throw;
    }
  }
}
