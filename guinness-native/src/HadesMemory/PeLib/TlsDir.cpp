// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/TlsDir.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/PeLib/NtHeaders.hpp>

namespace HadesMem
{
  // Constructor
  TlsDir::TlsDir(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_pBase(nullptr)
  { }
      
  // Copy constructor
  TlsDir::TlsDir(TlsDir const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  TlsDir& TlsDir::operator=(TlsDir const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  TlsDir::TlsDir(TlsDir&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  TlsDir& TlsDir::operator=(TlsDir&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  TlsDir::~TlsDir()
  { }

  // Whether TLS directory is valid
  bool TlsDir::IsValid() const
  {
    NtHeaders const MyNtHeaders(m_PeFile);
    
    DWORD const DataDirSize(MyNtHeaders.GetDataDirectorySize(NtHeaders::
      DataDir_TLS));
    DWORD const DataDirVa(MyNtHeaders.GetDataDirectoryVirtualAddress(
      NtHeaders::DataDir_TLS));
    
    return DataDirSize && DataDirVa;
  }

  // Ensure export directory is valid
  void TlsDir::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("TlsDir::EnsureValid") << 
        ErrorString("TLS directory is invalid."));
    }
  }

  // Get base of export dir
  PVOID TlsDir::GetBase() const
  {
    if (!m_pBase)
    {
      NtHeaders const MyNtHeaders(m_PeFile);
      
      DWORD const DataDirSize = MyNtHeaders.GetDataDirectorySize(NtHeaders::
        DataDir_TLS);
      DWORD const DataDirVa = MyNtHeaders.GetDataDirectoryVirtualAddress(
        NtHeaders::DataDir_TLS);
      if (!DataDirSize || !DataDirVa)
      {
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("TlsDir::GetBase") << 
          ErrorString("PE file has no TLS directory."));
      }
      
      m_pBase = static_cast<PBYTE>(m_PeFile.RvaToVa(DataDirVa));
    }
    
    return m_pBase;
  }

  // Get start address of raw data
  DWORD_PTR TlsDir::GetStartAddressOfRawData() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD_PTR>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, StartAddressOfRawData));
  }

  // Get end address of raw data
  DWORD_PTR TlsDir::GetEndAddressOfRawData() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD_PTR>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, EndAddressOfRawData));
  }

  // Get address of index
  DWORD_PTR TlsDir::GetAddressOfIndex() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD_PTR>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, AddressOfIndex));
  }

  // Get address of callbacks
  DWORD_PTR TlsDir::GetAddressOfCallBacks() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD_PTR>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, AddressOfCallBacks));
  }

  // Get size of zero fill
  DWORD TlsDir::GetSizeOfZeroFill() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, SizeOfZeroFill));
  }

  // Get characteristics
  DWORD TlsDir::GetCharacteristics() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_TLS_DIRECTORY, Characteristics));
  }

  // Get list of TLS callbacks
  std::vector<PIMAGE_TLS_CALLBACK> TlsDir::GetCallbacks() const
  {
    std::vector<PIMAGE_TLS_CALLBACK> Callbacks;
    
    NtHeaders MyNtHeaders(m_PeFile);
    
    PIMAGE_TLS_CALLBACK* pCallbacks = reinterpret_cast<PIMAGE_TLS_CALLBACK*>(
      m_PeFile.RvaToVa(static_cast<DWORD>(GetAddressOfCallBacks() - 
      MyNtHeaders.GetImageBase())));
    
    for (PIMAGE_TLS_CALLBACK pCallback = m_Memory.Read<PIMAGE_TLS_CALLBACK>(
      pCallbacks); pCallback; pCallback = m_Memory.Read<PIMAGE_TLS_CALLBACK>(
      ++pCallbacks))
    {
      auto pCallbackRealTemp = reinterpret_cast<DWORD_PTR>(pCallback) - 
        MyNtHeaders.GetImageBase();
      auto pCallbackReal = reinterpret_cast<PIMAGE_TLS_CALLBACK>(
        pCallbackRealTemp);
      Callbacks.push_back(pCallbackReal);
    }
    
    return Callbacks;
  }

  // Set start address of raw data
  void TlsDir::SetStartAddressOfRawData(DWORD_PTR StartAddressOfRawData) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      StartAddressOfRawData), StartAddressOfRawData);
  }

  // Set end address of raw data
  void TlsDir::SetEndAddressOfRawData(DWORD_PTR EndAddressOfRawData) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      EndAddressOfRawData), EndAddressOfRawData);
  }

  // Set address of index
  void TlsDir::SetAddressOfIndex(DWORD_PTR AddressOfIndex) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      AddressOfIndex), AddressOfIndex);
  }

  // Set address of callbacks
  void TlsDir::SetAddressOfCallBacks(DWORD_PTR AddressOfCallBacks) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      AddressOfCallBacks), AddressOfCallBacks);
  }

  // Set size of zero fill
  void TlsDir::SetSizeOfZeroFill(DWORD SizeOfZeroFill) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      SizeOfZeroFill), SizeOfZeroFill);
  }

  // Set characteristics
  void TlsDir::SetCharacteristics(DWORD Characteristics) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_TLS_DIRECTORY, 
      Characteristics), Characteristics);

    if (GetCharacteristics() != Characteristics)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("TlsDir::SetCharacteristics") << 
        ErrorString("Could not set data. Verification mismatch."));
    }
  }
  
  // Equality operator
  bool TlsDir::operator==(TlsDir const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool TlsDir::operator!=(TlsDir const& Rhs) const
  {
    return !(*this == Rhs);
  }
}
