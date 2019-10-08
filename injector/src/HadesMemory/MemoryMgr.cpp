// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/Region.hpp>
#include <HadesMemory/Module.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Config.hpp>
#include <HadesMemory/Detail/ArgQuote.hpp>
#include <HadesMemory/Detail/EnsureCleanup.hpp>

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
#ifdef HADES_MSVC
#pragma warning(pop)
#endif
#ifdef HADES_GCC
#pragma GCC diagnostic pop
#endif

#include <Windows.h>
#include <TlHelp32.h>
#include <Shellapi.h>

namespace HadesMem
{
  // Constructor
  MemoryMgr::RemoteFunctionRet::RemoteFunctionRet(DWORD_PTR ReturnValue, 
    DWORD64 ReturnValue64, DWORD LastError) 
    : m_ReturnValue(ReturnValue), 
    m_ReturnValue64(ReturnValue64), 
    m_LastError(LastError)
  { }
  
  // Get return value
  DWORD_PTR MemoryMgr::RemoteFunctionRet::GetReturnValue() const
  {
    return m_ReturnValue;
  }
  
  // Get 64-bit return value
  DWORD64 MemoryMgr::RemoteFunctionRet::GetReturnValue64() const
  {
    return m_ReturnValue64;
  }
  
  // Get thread last error
  DWORD MemoryMgr::RemoteFunctionRet::GetLastError() const
  {
    return m_LastError;
  }
  
  // Open process from process ID
  MemoryMgr::MemoryMgr(DWORD ProcID) 
    : m_Process(ProcID) 
  { }
      
  // Copy constructor
  MemoryMgr::MemoryMgr(MemoryMgr const& Other)
    : m_Process(Other.m_Process)
  { }
  
  // Copy assignment operator
  MemoryMgr& MemoryMgr::operator=(MemoryMgr const& Other)
  {
    this->m_Process = Other.m_Process;
    
    return *this;
  }
  
  // Move constructor
  MemoryMgr::MemoryMgr(MemoryMgr&& Other)
    : m_Process(std::move(Other.m_Process))
  { }
  
  // Move assignment operator
  MemoryMgr& MemoryMgr::operator=(MemoryMgr&& Other)
  {
    this->m_Process = std::move(Other.m_Process);
    
    return *this;
  }
  
  // Destructor
  MemoryMgr::~MemoryMgr()
  { }

  // Call remote function
  MemoryMgr::RemoteFunctionRet MemoryMgr::Call(LPCVOID Address, 
    CallConv MyCallConv, std::vector<PVOID> const& Args) const
  {
    std::vector<LPCVOID> Addresses;
    Addresses.push_back(Address);
    std::vector<CallConv> MyCallConvs;
    MyCallConvs.push_back(MyCallConv);
    std::vector<std::vector<PVOID>> ArgsFull;
    ArgsFull.push_back(Args);
    return Call(Addresses, MyCallConvs, ArgsFull)[0];
  }

  // Call remote function
  std::vector<MemoryMgr::RemoteFunctionRet> MemoryMgr::Call(
    std::vector<LPCVOID> Addresses, 
    std::vector<CallConv> MyCallConvs, 
    std::vector<std::vector<PVOID>> const& ArgsFull) const 
  {
    if (Addresses.size() != MyCallConvs.size() || 
      Addresses.size() != ArgsFull.size())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Call") << 
        ErrorString("Size mismatch in parameters."));
    }
    
    AsmJit::Assembler MyJitFunc;

    AllocAndFree const ReturnValueRemote(*this, sizeof(DWORD_PTR) * Addresses.size());
    AllocAndFree const ReturnValue64Remote(*this, sizeof(DWORD64) * Addresses.size());
    AllocAndFree const LastErrorRemote(*this, sizeof(DWORD) * Addresses.size());

    Module Kernel32Mod(*this, L"kernel32.dll");
    DWORD_PTR const pGetLastError = reinterpret_cast<DWORD_PTR>(
      Kernel32Mod.FindProcedure("GetLastError"));
    DWORD_PTR const pSetLastError = reinterpret_cast<DWORD_PTR>(
      Kernel32Mod.FindProcedure("SetLastError"));

#if defined(_M_AMD64) 
    for (std::size_t i = 0; i < Addresses.size(); ++i)
    {
      LPCVOID Address = Addresses[i];
      CallConv MyCallConv = MyCallConvs[i];
      std::vector<PVOID> const& Args = ArgsFull[i];
      std::size_t const NumArgs = Args.size();
      
      // Check calling convention
      if (MyCallConv != CallConv_X64 && MyCallConv != CallConv_Default)
      {
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("MemoryMgr::Call") << 
          ErrorString("Invalid calling convention."));
      }
      
      // Prologue
      MyJitFunc.push(AsmJit::rbp);
      MyJitFunc.mov(AsmJit::rbp, AsmJit::rsp);
  
      // Allocate ghost space
      MyJitFunc.sub(AsmJit::rsp, AsmJit::Imm(0x20));
  
      // Call kernel32.dll!SetLastError
      MyJitFunc.mov(AsmJit::rcx, 0);
      MyJitFunc.mov(AsmJit::rax, pSetLastError);
      MyJitFunc.call(AsmJit::rax);
  
      // Cleanup ghost space
      MyJitFunc.add(AsmJit::rsp, AsmJit::Imm(0x20));
  
      // Set up first 4 parameters
      MyJitFunc.mov(AsmJit::rcx, NumArgs > 0 ? reinterpret_cast<DWORD_PTR>(
        Args[0]) : 0);
      MyJitFunc.mov(AsmJit::rdx, NumArgs > 1 ? reinterpret_cast<DWORD_PTR>(
        Args[1]) : 0);
      MyJitFunc.mov(AsmJit::r8, NumArgs > 2 ? reinterpret_cast<DWORD_PTR>(
        Args[2]) : 0);
      MyJitFunc.mov(AsmJit::r9, NumArgs > 3 ? reinterpret_cast<DWORD_PTR>(
        Args[3]) : 0);
  
      // Handle remaining parameters (if any)
      if (NumArgs > 4)
      {
        std::for_each(Args.crbegin(), Args.crend() - 4, 
          [&MyJitFunc] (PVOID Arg)
        {
          MyJitFunc.mov(AsmJit::rax, reinterpret_cast<DWORD_PTR>(Arg));
          MyJitFunc.push(AsmJit::rax);
        });
      }
  
      // Allocate ghost space
      MyJitFunc.sub(AsmJit::rsp, AsmJit::Imm(0x20));
  
      // Call target
      MyJitFunc.mov(AsmJit::rax, reinterpret_cast<DWORD_PTR>(Address));
      MyJitFunc.call(AsmJit::rax);
      
      // Cleanup ghost space
      MyJitFunc.add(AsmJit::rsp, AsmJit::Imm(0x20));
  
      // Clean up remaining stack space
      MyJitFunc.add(AsmJit::rsp, 0x8 * (NumArgs - 4));
  
      // Write return value to memory
      MyJitFunc.mov(AsmJit::rcx, reinterpret_cast<DWORD_PTR>(
        ReturnValueRemote.GetBase()) + i * sizeof(DWORD_PTR));
      MyJitFunc.mov(AsmJit::qword_ptr(AsmJit::rcx), AsmJit::rax);
  
      // Write 64-bit return value to memory
      MyJitFunc.mov(AsmJit::rcx, reinterpret_cast<DWORD_PTR>(
        ReturnValue64Remote.GetBase()) + i * sizeof(DWORD64));
      MyJitFunc.mov(AsmJit::qword_ptr(AsmJit::rcx), AsmJit::rax);
  
      // Call kernel32.dll!GetLastError
      MyJitFunc.mov(AsmJit::rax, pGetLastError);
      MyJitFunc.call(AsmJit::rax);
      
      // Write error code to memory
      MyJitFunc.mov(AsmJit::rcx, reinterpret_cast<DWORD_PTR>(
        LastErrorRemote.GetBase()) + i * sizeof(DWORD));
      MyJitFunc.mov(AsmJit::dword_ptr(AsmJit::rcx), AsmJit::rax);
  
      // Epilogue
      MyJitFunc.mov(AsmJit::rsp, AsmJit::rbp);
      MyJitFunc.pop(AsmJit::rbp);
    }

    // Return
    MyJitFunc.ret();
#elif defined(_M_IX86) 
    for (std::size_t i = 0; i < Addresses.size(); ++i)
    {
      LPCVOID Address = Addresses[i];
      CallConv MyCallConv = MyCallConvs[i];
      std::vector<PVOID> const& Args = ArgsFull[i];
      std::size_t const NumArgs = Args.size();
      
      // Prologue
      MyJitFunc.push(AsmJit::ebp);
      MyJitFunc.mov(AsmJit::ebp, AsmJit::esp);
  
      // Call kernel32.dll!SetLastError
      MyJitFunc.push(AsmJit::Imm(0x0));
      MyJitFunc.mov(AsmJit::eax, pSetLastError);
      MyJitFunc.call(AsmJit::eax);
  
      // Get stack arguments offset
      std::size_t StackArgOffs = 0;
      switch (MyCallConv)
      {
      case CallConv_THISCALL:
        StackArgOffs = 1;
        break;
  
      case CallConv_FASTCALL:
        StackArgOffs = 2;
        break;
  
      case CallConv_CDECL:
      case CallConv_STDCALL:
      case CallConv_Default:
        StackArgOffs = 0;
        break;
  
      default:
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("MemoryMgr::Call") << 
          ErrorString("Invalid calling convention."));
      }
  
      // Pass first arg in through ECX if 'thiscall' is specified
      if (MyCallConv == CallConv_THISCALL)
      {
        MyJitFunc.mov(AsmJit::ecx, NumArgs ? reinterpret_cast<DWORD_PTR>(
          Args[0]) : 0);
      }
  
      // Pass first two args in through ECX and EDX if 'fastcall' is specified
      if (MyCallConv == CallConv_FASTCALL)
      {
        MyJitFunc.mov(AsmJit::ecx, NumArgs ? reinterpret_cast<DWORD_PTR>(
          Args[0]) : 0);
        MyJitFunc.mov(AsmJit::edx, NumArgs > 1 ? reinterpret_cast<DWORD_PTR>(
          Args[1]) : 0);
      }
  
      // Pass all remaining args on stack if there are any left to process.
      if (NumArgs > StackArgOffs)
      {
        std::for_each(Args.crbegin(), Args.crend() - StackArgOffs, 
          [&] (PVOID Arg)
        {
          MyJitFunc.mov(AsmJit::eax, reinterpret_cast<DWORD_PTR>(Arg));
          MyJitFunc.push(AsmJit::eax);
        });
      }
      
      // Call target
      MyJitFunc.mov(AsmJit::eax, reinterpret_cast<DWORD_PTR>(Address));
      MyJitFunc.call(AsmJit::eax);
      
      // Write return value to memory
      MyJitFunc.mov(AsmJit::ecx, reinterpret_cast<DWORD_PTR>(
        ReturnValueRemote.GetBase()) + i * sizeof(DWORD_PTR));
      MyJitFunc.mov(AsmJit::dword_ptr(AsmJit::ecx), AsmJit::eax);
      
      // Write 64-bit return value to memory
      MyJitFunc.mov(AsmJit::ecx, reinterpret_cast<DWORD_PTR>(
        ReturnValue64Remote.GetBase()) + i * sizeof(DWORD64));
      MyJitFunc.mov(AsmJit::dword_ptr(AsmJit::ecx), AsmJit::eax);
      MyJitFunc.mov(AsmJit::dword_ptr(AsmJit::ecx, 4), AsmJit::edx);
      
      // Call kernel32.dll!GetLastError
      MyJitFunc.mov(AsmJit::eax, pGetLastError);
      MyJitFunc.call(AsmJit::eax);
      
      // Write error code to memory
      MyJitFunc.mov(AsmJit::ecx, reinterpret_cast<DWORD_PTR>(
        LastErrorRemote.GetBase()) + i * sizeof(DWORD));
      MyJitFunc.mov(AsmJit::dword_ptr(AsmJit::ecx), AsmJit::eax);
      
      // Clean up stack if necessary
      if (MyCallConv == CallConv_CDECL)
      {
        MyJitFunc.add(AsmJit::esp, AsmJit::Imm(NumArgs * sizeof(PVOID)));
      }
  
      // Epilogue
      MyJitFunc.mov(AsmJit::esp, AsmJit::ebp);
      MyJitFunc.pop(AsmJit::ebp);
    }

    // Return
    MyJitFunc.ret(AsmJit::Imm(0x4));
#else 
#error "[HadesMem] Unsupported architecture."
#endif
    
    DWORD_PTR const StubSize = MyJitFunc.getCodeSize();
    
    AllocAndFree const StubMemRemote(*this, StubSize);
    PBYTE const pRemoteStub = static_cast<PBYTE>(StubMemRemote.GetBase());
    DWORD_PTR const pRemoteStubTemp = reinterpret_cast<DWORD_PTR>(
      pRemoteStub);
    
    std::vector<BYTE> CodeReal(StubSize);
    MyJitFunc.relocCode(CodeReal.data(), reinterpret_cast<DWORD_PTR>(
      pRemoteStub));
    
    WriteList(pRemoteStub, CodeReal);
    
    Detail::EnsureCloseHandle const MyThread(CreateRemoteThread(m_Process.
      GetHandle(), nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(
      pRemoteStubTemp), nullptr, 0, nullptr));
    if (!MyThread)
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Call") << 
        ErrorString("Could not create remote thread.") << 
        ErrorCodeWinLast(LastError));
    }
    
    if (WaitForSingleObject(MyThread, INFINITE) != WAIT_OBJECT_0)
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Call") << 
        ErrorString("Could not wait for remote thread.") << 
        ErrorCodeWinLast(LastError));
    }
    
    std::vector<RemoteFunctionRet> ReturnVals;
    for (std::size_t i = 0; i < Addresses.size(); ++i)
    {
      DWORD_PTR const RetVal = Read<DWORD_PTR>(static_cast<DWORD_PTR*>(
        ReturnValueRemote.GetBase()) + i);
      DWORD64 const RetVal64 = Read<DWORD64>(static_cast<DWORD64*>(
        ReturnValue64Remote.GetBase()) + i);
      DWORD const ErrorCode = Read<DWORD>(static_cast<DWORD*>(
        LastErrorRemote.GetBase()) + i);
      ReturnVals.push_back(RemoteFunctionRet(RetVal, RetVal64, ErrorCode));
    }
    
    return ReturnVals;
  }

  // Whether an address is currently readable
  bool MemoryMgr::CanRead(LPCVOID Address) const
  {
    MEMORY_BASIC_INFORMATION MyMbi;
    ZeroMemory(&MyMbi, sizeof(MyMbi));
    if (VirtualQueryEx(m_Process.GetHandle(), Address, &MyMbi, 
      sizeof(MyMbi)) != sizeof(MyMbi))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::CanRead") << 
        ErrorString("Could not read process memory protection.") << 
        ErrorCodeWinLast(LastError));
    }
    
    return (MyMbi.State != MEM_RESERVE) && 
      ((MyMbi.Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ || 
      (MyMbi.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE || 
      (MyMbi.Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY || 
      (MyMbi.Protect & PAGE_READONLY) == PAGE_READONLY || 
      (MyMbi.Protect & PAGE_READWRITE) == PAGE_READWRITE || 
      (MyMbi.Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY);
  }

  // Whether an address is currently writable
  bool MemoryMgr::CanWrite(LPCVOID Address) const
  {
    MEMORY_BASIC_INFORMATION MyMbi;
    ZeroMemory(&MyMbi, sizeof(MyMbi));
    if (VirtualQueryEx(m_Process.GetHandle(), Address, &MyMbi, 
      sizeof(MyMbi)) != sizeof(MyMbi))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Write") << 
        ErrorString("Could not read process memory protection.") << 
        ErrorCodeWinLast(LastError));
    }
    
    return (MyMbi.State != MEM_RESERVE) && 
      ((MyMbi.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE || 
      (MyMbi.Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY || 
      (MyMbi.Protect & PAGE_READWRITE) == PAGE_READWRITE || 
      (MyMbi.Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY);
  }

  // Whether an address is currently executable
  bool MemoryMgr::CanExecute(LPCVOID Address) const
  {
    MEMORY_BASIC_INFORMATION MyMbi;
    ZeroMemory(&MyMbi, sizeof(MyMbi));
    if (VirtualQueryEx(m_Process.GetHandle(), Address, &MyMbi, 
      sizeof(MyMbi)) != sizeof(MyMbi))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Write") << 
        ErrorString("Could not read process memory protection.") << 
        ErrorCodeWinLast(LastError));
    }
    
    return (MyMbi.State != MEM_RESERVE) && 
      ((MyMbi.Protect & PAGE_EXECUTE) == PAGE_EXECUTE || 
      (MyMbi.Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ || 
      (MyMbi.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE || 
      (MyMbi.Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY);
  }

  // Whether an address is contained within a guard page
  bool MemoryMgr::IsGuard(LPCVOID Address) const
  {
    MEMORY_BASIC_INFORMATION MyMbi;
    ZeroMemory(&MyMbi, sizeof(MyMbi));
    if (VirtualQueryEx(m_Process.GetHandle(), Address, &MyMbi, 
      sizeof(MyMbi)) != sizeof(MyMbi))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::IsGuard") << 
        ErrorString("Could not read process memory protection.") << 
        ErrorCodeWinLast(LastError));
    }
    
    return (MyMbi.Protect & PAGE_GUARD) == PAGE_GUARD;
  }
  
  // Protect a memory region
  DWORD MemoryMgr::ProtectRegion(LPVOID Address, DWORD Protect) const
  {
    Region Target(*this, Address);
    return Target.SetProtect(Protect);
  }

  // Allocate memory
  PVOID MemoryMgr::Alloc(SIZE_T Size) const
  {
    PVOID const Address = VirtualAllocEx(m_Process.GetHandle(), nullptr, 
      Size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!Address)
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Alloc") << 
        ErrorString("Could not allocate memory.") << 
        ErrorCodeWinLast(LastError));
    }

    return Address;
  }

  // Free memory
  void MemoryMgr::Free(PVOID Address) const
  {
    if (!VirtualFreeEx(m_Process.GetHandle(), Address, 0, MEM_RELEASE))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Free") << 
        ErrorString("Could not free memory.") << 
        ErrorCodeWinLast(LastError));
    }
  }

  // Flush instruction cache
  void MemoryMgr::FlushCache(LPCVOID Address, SIZE_T Size) const
  {
    if (!FlushInstructionCache(m_Process.GetHandle(), Address, Size))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::FlushInstructionCache") << 
        ErrorString("Could not flush instruction cache.") << 
        ErrorCodeWinLast(LastError));
    }
  }

  // Get process handle of target
  HANDLE MemoryMgr::GetProcessHandle() const
  {
    return m_Process.GetHandle();
  }

  // Get process ID of target
  DWORD MemoryMgr::GetProcessId() const
  {
    return m_Process.GetID();
  }
    
  // Get process path
  std::wstring MemoryMgr::GetProcessPath() const
  {
    return m_Process.GetPath();
  }

  // Is WoW64 process
  bool MemoryMgr::IsWoW64Process() const 
  {
    return m_Process.IsWoW64();
  }
  
  // Equality operator
  bool MemoryMgr::operator==(MemoryMgr const& Rhs) const
  {
    return m_Process == Rhs.m_Process;
  }
  
  // Inequality operator
  bool MemoryMgr::operator!=(MemoryMgr const& Rhs) const
  {
    return !(*this == Rhs);
  }

  // Read memory
  void MemoryMgr::ReadImpl(PVOID Address, PVOID Out, std::size_t OutSize) const 
  {
    if (IsGuard(Address))
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Read") << 
        ErrorString("Attempt to read from guard page."));
    }
    
    bool const CanReadMem = CanRead(Address);

    DWORD OldProtect = 0;
    if (!CanReadMem)
    {
      OldProtect = ProtectRegion(Address, PAGE_EXECUTE_READWRITE);
    }
    
    SIZE_T BytesRead = 0;
    if (!ReadProcessMemory(m_Process.GetHandle(), Address, Out, 
      OutSize, &BytesRead) || BytesRead != OutSize)
    {
      if (!CanReadMem)
      {
        try
        {
          ProtectRegion(Address, OldProtect);
        }
        catch (std::exception const& /*e*/)
        { }
      }

      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Read") << 
        ErrorString("Could not read process memory.") << 
        ErrorCodeWinLast(LastError));
    }
    
    if (!CanReadMem)
    {
      ProtectRegion(Address, OldProtect);
    }
  }

  // Write memory
  void MemoryMgr::WriteImpl(PVOID Address, LPCVOID In, std::size_t InSize) const
  {
    if (IsGuard(Address))
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Write") << 
        ErrorString("Attempt to write to guard page."));
    }
    
    bool const CanWriteMem = CanWrite(Address);
    
    DWORD OldProtect = 0;
    if (!CanWriteMem)
    {
      OldProtect = ProtectRegion(Address, PAGE_EXECUTE_READWRITE);
    }
    
    SIZE_T BytesWritten = 0;
    if (!WriteProcessMemory(m_Process.GetHandle(), Address, In, 
      InSize, &BytesWritten) || BytesWritten != InSize)
    {
      if (!CanWriteMem)
      {
        try
        {
          ProtectRegion(Address, OldProtect);
        }
        catch (std::exception const& /*e*/)
        { }
      }

      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("MemoryMgr::Write") << 
        ErrorString("Could not write process memory.") << 
        ErrorCodeWinLast(LastError));
    }
    
    if (!CanWriteMem)
    {
      ProtectRegion(Address, OldProtect);
    }
  }
    
  // Create process
  MemoryMgr CreateProcess(std::wstring const& Path, 
    std::wstring const& CommandLine, 
    std::wstring const& WorkingDir)
  {
    if (!CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | 
      COINIT_DISABLE_OLE1DDE))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("CreateProcess") << 
        ErrorString("Could not initialize COM.") << 
        ErrorCodeWinLast(LastError));
    }
    
    Detail::EnsureCoUninitialize MyComCleanup;
    
    SHELLEXECUTEINFO ExecInfo;
    ZeroMemory(&ExecInfo, sizeof(ExecInfo));
    ExecInfo.cbSize = sizeof(ExecInfo);
#ifndef SEE_MASK_NOASYNC 
#define SEE_MASK_NOASYNC 0x00000100
#endif
    ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    ExecInfo.lpFile = Path.empty() ? nullptr : Path.c_str();
    ExecInfo.lpParameters = CommandLine.empty() ? nullptr : 
      CommandLine.c_str();
    ExecInfo.lpDirectory = WorkingDir.empty() ? nullptr : WorkingDir.c_str();
    ExecInfo.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteEx(&ExecInfo))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("CreateProcess") << 
        ErrorString("Could not create process.") << 
        ErrorCodeWinLast(LastError));
    }
    
    Detail::EnsureCloseHandle const MyProc(ExecInfo.hProcess);
    
    return MemoryMgr(GetProcessId(MyProc));
  }
    
  // Create process
  MemoryMgr CreateProcess(std::wstring const& Path, 
    std::vector<std::wstring> const& Args, 
    std::wstring const& WorkingDir)
  {
    std::wstring CommandLine;
    Detail::ArgvQuote(Path, CommandLine, false);
    std::for_each(std::begin(Args), std::end(Args), 
      [&] (std::wstring const& Arg) 
      {
        CommandLine += L' ';
        Detail::ArgvQuote(Arg, CommandLine, false);
      });
    
    return CreateProcess(Path, CommandLine, WorkingDir);
  }

  // Gets the SeDebugPrivilege
  void GetSeDebugPrivilege()
  {
    HANDLE TempToken = 0;
    BOOL const RetVal = OpenProcessToken(GetCurrentProcess(), 
      TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TempToken);
    if (!RetVal) 
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("GetSeDebugPrivilege") << 
        ErrorString("Could not open process token.") << 
        ErrorCodeWinLast(LastError));
    }
    Detail::EnsureCloseHandle const Token(TempToken);

    LUID Luid = { 0, 0 };
    if (!LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &Luid)) 
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("GetSeDebugPrivilege") << 
        ErrorString("Could not look up privilege value for "
        "SeDebugName.") << 
        ErrorCodeWinLast(LastError));
    }
    if (Luid.LowPart == 0 && Luid.HighPart == 0) 
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("GetSeDebugPrivilege") << 
        ErrorString("Could not get LUID for SeDebugName.") << 
        ErrorCodeWinLast(LastError));
    }

    TOKEN_PRIVILEGES Privileges;
    ZeroMemory(&Privileges, sizeof(Privileges));
    Privileges.PrivilegeCount = 1;
    Privileges.Privileges[0].Luid = Luid;
    Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(Token, FALSE, &Privileges, 
      sizeof(Privileges), nullptr, nullptr)) 
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("GetSeDebugPrivilege") << 
        ErrorString("Could not adjust token privileges.") << 
        ErrorCodeWinLast(LastError));
    }
    
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(MemoryMgr::Error() << 
        ErrorFunction("GetSeDebugPrivilege") << 
        ErrorString("Could not assign all privileges.") << 
        ErrorCodeWinLast(LastError));
    }
  }
  
  // Constructor
  AllocAndFree::AllocAndFree(MemoryMgr const& MyMemoryMgr, SIZE_T Size)
    : m_Memory(MyMemoryMgr), 
    m_Size(Size), 
    m_Address(MyMemoryMgr.Alloc(Size)) 
  { }
  
  // Move constructor
  AllocAndFree::AllocAndFree(AllocAndFree&& Other)
    : m_Memory(std::move(Other.m_Memory)), 
    m_Size(Other.m_Size), 
    m_Address(Other.m_Address) 
  {
    Other.m_Size = 0;
    Other.m_Address = nullptr;
  }
  
  // Move assignment operator
  AllocAndFree& AllocAndFree::operator=(AllocAndFree&& Other)
  {
    Free();
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_Size = Other.m_Size;
    Other.m_Size = 0;
    
    this->m_Address = Other.m_Address;
    Other.m_Address = nullptr;
    
    return *this;
  }
  
  // Destructor
  AllocAndFree::~AllocAndFree()
  {
    try
    {
      Free();
    }
    catch (std::exception const& e)
    {
      OutputDebugStringA(boost::diagnostic_information(e).c_str());
    }
    catch (...)
    {
      OutputDebugString(L"AllocAndFree::~AllocAndFree: Unknown error.");
    }
  }
  
  // Free memory
  void AllocAndFree::Free() const
  {
    if (m_Address)
    {
      m_Memory.Free(m_Address);
      m_Address = nullptr;
    }
  }
  
  // Get base address of memory region
  PVOID AllocAndFree::GetBase() const
  {
    return m_Address;
  }
  
  // Get size of memory region
  SIZE_T AllocAndFree::GetSize() const
  {
    return m_Size;
  }
}
