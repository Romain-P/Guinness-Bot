// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Fwd.hpp>
#include <HadesMemory/Detail/Error.hpp>

#include <Windows.h>

namespace HadesMem
{
  // PE file format wrapper
  class PeFile
  {
  public:
    // PeFile exception type
    class Error : public virtual HadesMemError 
    { };

    // PE file types
    enum FileType
    {
      FileType_Image, 
      FileType_Data
    };

    // Constructor
    PeFile(MemoryMgr const& MyMemory, PVOID Address, 
      FileType Type = FileType_Image);
      
    // Copy constructor
    PeFile(PeFile const& Other);
    
    // Copy assignment operator
    PeFile& operator=(PeFile const& Other);
    
    // Move constructor
    PeFile(PeFile&& Other);
    
    // Move assignment operator
    PeFile& operator=(PeFile&& Other);
    
    // Destructor
    ~PeFile();
    
    // Equality operator
    bool operator==(PeFile const& Rhs) const;
    
    // Inequality operator
    bool operator!=(PeFile const& Rhs) const;

    // Get memory manager
    MemoryMgr GetMemoryMgr() const;

    // Get base address
    PVOID GetBase() const;

    // Convert RVA to VA
    PVOID RvaToVa(DWORD Rva) const;

    // Get file type
    FileType GetType() const;

  protected:
    // Memory instance
    MemoryMgr m_Memory;

    // Base address
    PBYTE m_pBase;

    // File type
    FileType m_Type;
  };
}
