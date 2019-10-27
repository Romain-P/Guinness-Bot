// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Error.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>

#include <vector>

#include <Windows.h>

namespace HadesMem
{
  // PE file TLS directory
  class TlsDir
  {
  public:
    // TlsDir error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    explicit TlsDir(PeFile const& MyPeFile);
      
    // Copy constructor
    TlsDir(TlsDir const& Other);
    
    // Copy assignment operator
    TlsDir& operator=(TlsDir const& Other);
    
    // Move constructor
    TlsDir(TlsDir&& Other);
    
    // Move assignment operator
    TlsDir& operator=(TlsDir&& Other);
    
    // Destructor
    ~TlsDir();
    
    // Equality operator
    bool operator==(TlsDir const& Rhs) const;
    
    // Inequality operator
    bool operator!=(TlsDir const& Rhs) const;

    // Get base of TLS dir
    PVOID GetBase() const;

    // Whether TLS directory is valid
    bool IsValid() const;

    // Ensure TLS directory is valid
    void EnsureValid() const;

    // Get start address of raw data
    DWORD_PTR GetStartAddressOfRawData() const;

    // Get end address of raw data
    DWORD_PTR GetEndAddressOfRawData() const;

    // Get address of index
    DWORD_PTR GetAddressOfIndex() const;

    // Get address of callbacks
    DWORD_PTR GetAddressOfCallBacks() const;

    // Get list of TLS callbacks
    std::vector<PIMAGE_TLS_CALLBACK> GetCallbacks() const;

    // Get size of zero fill
    DWORD GetSizeOfZeroFill() const;

    // Get characteristics
    DWORD GetCharacteristics() const;

    // Set start address of raw data
    void SetStartAddressOfRawData(DWORD_PTR StartAddressOfRawData) const;

    // Set end address of raw data
    void SetEndAddressOfRawData(DWORD_PTR EndAddressOfRawData) const;

    // Set address of index
    void SetAddressOfIndex(DWORD_PTR AddressOfIndex) const;

    // Set address of callbacks
    void SetAddressOfCallBacks(DWORD_PTR AddressOfCallbacks) const;
      
    // TODO: SetCallbacks function

    // Set size of zero fill
    void SetSizeOfZeroFill(DWORD SizeOfZeroFill) const;

    // Set characteristics
    void SetCharacteristics(DWORD Characteristics) const;

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;
    
    // Tls dir base
    mutable PBYTE m_pBase;
  };
}
