// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/Detail/Process.hpp>
#include <HadesMemory/Detail/StringBuffer.hpp>
#include <HadesMemory/Detail/EnsureCleanup.hpp>

#include <Windows.h>
#include <psapi.h>

namespace HadesMem
{
  namespace Detail
  {
    // Process implementation
    class Process::Impl
    {
    public:
      friend class Process;
      
      // Constructor
      explicit Impl(DWORD ProcID) 
        : m_Handle(), 
        m_ID(ProcID), 
        m_IsWoW64(false)
      {
        if (GetCurrentProcessId() == m_ID)
        {
          m_Handle.reset(new EnsureCloseHandle(GetCurrentProcess()));
          m_ID = GetCurrentProcessId();
        }
        else
        {
          Open(m_ID);
        }
        
        SetWoW64();
      }
      
      // Swap
      void swap(Impl& Rhs)
      {
        m_Handle.swap(Rhs.m_Handle);
        std::swap(m_ID, Rhs.m_ID);
        std::swap(m_IsWoW64, Rhs.m_IsWoW64);
      }
  
      // Get process handle
      HANDLE GetHandle() const
      {
        return *m_Handle;
      }
  
      // Get process ID
      DWORD GetID() const
      {
        return m_ID;
      }
        
      // Get process path
      std::wstring GetPath() const
      {
        // The QueryFullProcessImageName API is more efficient and 
        // reliable but is only available on Vista+.
        DWORD const PathSize = 32767;
        std::wstring Path;
        if (!GetModuleFileNameEx(*m_Handle, nullptr, MakeStringBuffer(Path,
          PathSize), PathSize))
        {
          DWORD const LastError = GetLastError();
          BOOST_THROW_EXCEPTION(Error() << 
            ErrorFunction("Process::GetPath") << 
            ErrorString("Could not get path.") << 
            ErrorCodeWinLast(LastError));
        }
        
        return Path;
      }
      
      // Is WoW64 process
      bool IsWoW64() const
      {
        return m_IsWoW64;
      }
      
    private:
      // Get WoW64 status of process and set member var
      void SetWoW64()
      {
        typedef BOOL (WINAPI* tIsWow64Process)(HANDLE hProcess, 
          PBOOL Wow64Process);
        auto pIsWow64Process = reinterpret_cast<tIsWow64Process>(
          GetProcAddress(GetModuleHandle(L"kernel32.dll"), "IsWow64Process"));
        
        // If IsWow64Process API doesn't exist, assume that the OS doesn't 
        // support WoW64 and must be x86 only.
        if (!pIsWow64Process)
        {
          m_IsWoW64 = false;
          return;
        }

        BOOL IsWoW64Me = FALSE;
        if (!pIsWow64Process(GetCurrentProcess(), &IsWoW64Me))
        {
          DWORD const LastError = GetLastError();
          BOOST_THROW_EXCEPTION(Error() << 
            ErrorFunction("Process::SetWoW64") << 
            ErrorString("Could not detect WoW64 status of current process.") << 
            ErrorCodeWinLast(LastError));
        }
        
        BOOL IsWoW64 = FALSE;
        if (!pIsWow64Process(*m_Handle, &IsWoW64))
        {
          DWORD const LastError = GetLastError();
          BOOST_THROW_EXCEPTION(Error() << 
            ErrorFunction("Process::SetWoW64") << 
            ErrorString("Could not detect WoW64 status of target process.") << 
            ErrorCodeWinLast(LastError));
        }
        
        m_IsWoW64 = (IsWoW64 != FALSE);
        
        // Disable cross-architecture process manipulation
        if (IsWoW64Me != IsWoW64)
        {
          BOOST_THROW_EXCEPTION(Error() << 
            ErrorFunction("Process::SetWoW64") << 
            ErrorString("x86 -> x64 process manipulation is currently "
            "unsupported."));
        }
      }
  
      // Open process given process id
      void Open(DWORD ProcID)
      {
        m_Handle.reset(new EnsureCloseHandle(
          OpenProcess(PROCESS_CREATE_THREAD | 
          PROCESS_QUERY_INFORMATION | 
          PROCESS_VM_OPERATION | 
          PROCESS_VM_READ | 
          PROCESS_VM_WRITE, 
          FALSE, 
          ProcID)));
        if (!m_Handle)
        {
          DWORD const LastError = GetLastError();
          BOOST_THROW_EXCEPTION(Error() << 
            ErrorFunction("Process::Open") << 
            ErrorString("Could not open process.") << 
            ErrorCodeWinLast(LastError));
        }
      }
      
      // Process handle
      // Using shared pointer because handle does not need to be unique, 
      // and copying it may throw, so sharing it makes exception safe code 
      // far easier to write.
      std::shared_ptr<EnsureCloseHandle> m_Handle;
    
      // Process ID
      DWORD m_ID;
      
      // Is WoW64 process
      bool m_IsWoW64;
    };
    
    // Open process from process id
    Process::Process(DWORD ProcID)
      : m_pImpl(new Impl(ProcID))
    { }
      
    // Copy constructor
    Process::Process(Process const& Other)
      : m_pImpl(Other.m_pImpl)
    { }
    
    // Copy assignment operator
    Process& Process::operator=(Process const& Other)
    {
      this->m_pImpl = Other.m_pImpl;
      
      return *this;
    }
    
    // Move constructor
    Process::Process(Process&& Other)
      : m_pImpl()
    {
      *this = std::move(Other);
    }
    
    // Move assignment operator
    Process& Process::operator=(Process&& Other)
    {
      this->m_pImpl = std::move(Other.m_pImpl);
      
      return *this;
    }
    
    // Destructor
    // An empty destructor is required so the compiler can see Impl's 
    // destructor.
    Process::~Process()
    { }
  
    // Get process handle
    HANDLE Process::GetHandle() const
    {
      return m_pImpl->GetHandle();
    }
  
    // Get process ID
    DWORD Process::GetID() const
    {
      return m_pImpl->GetID();
    }
      
    // Get process path
    std::wstring Process::GetPath() const
    {
      return m_pImpl->GetPath();
    }
    
    // Is WoW64 process
    bool Process::IsWoW64() const
    {
      return m_pImpl->IsWoW64();
    }
    
    // Equality operator
    bool Process::operator==(Process const& Rhs) const
    {
      return m_pImpl->m_ID == Rhs.m_pImpl->m_ID;
    }
    
    // Inequality operator
    bool Process::operator!=(Process const& Rhs) const
    {
      return !(*this == Rhs);
    }
  }
}
