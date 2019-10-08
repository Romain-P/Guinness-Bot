// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/Detail/Error.hpp>

#include <string>
#include <memory>

#include <Windows.h>

namespace HadesMem
{
  namespace Detail
  {
    // Process managing class
    class Process
    {
    public:
      // Process exception type
      class Error : public virtual HadesMemError 
      { };
    
      // Open process from process ID
      explicit Process(DWORD ProcID);
      
      // Copy constructor
      Process(Process const& Other);
      
      // Copy assignment operator
      Process& operator=(Process const& Other);
      
      // Move constructor
      Process(Process&& Other);
      
      // Move assignment operator
      Process& operator=(Process&& Other);
      
      // Destructor
      ~Process();
      
      // Equality operator
      bool operator==(Process const& Rhs) const;
      
      // Inequality operator
      bool operator!=(Process const& Rhs) const;
    
      // Get process handle
      HANDLE GetHandle() const;
      
      // Get process ID
      DWORD GetID() const;
      
      // Get process path
      std::wstring GetPath() const;
      
      // Is WoW64 process
      bool IsWoW64() const;
    
    private:
      // Implementation
      class Impl;
      std::shared_ptr<Impl> m_pImpl;
    };
  }
}
