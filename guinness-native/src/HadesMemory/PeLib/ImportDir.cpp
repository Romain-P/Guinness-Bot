// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/ImportDir.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/PeLib/NtHeaders.hpp>
#include <HadesMemory/PeLib/DosHeader.hpp>

#include <vector>

namespace HadesMem
{
  // Constructor
  ImportDir::ImportDir(PeFile const& MyPeFile, 
    PIMAGE_IMPORT_DESCRIPTOR pImpDesc) 
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_pBase(pImpDesc)
  { }
      
  // Copy constructor
  ImportDir::ImportDir(ImportDir const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  ImportDir& ImportDir::operator=(ImportDir const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  ImportDir::ImportDir(ImportDir&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  ImportDir& ImportDir::operator=(ImportDir&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  ImportDir::~ImportDir()
  { }

  // Whether import directory is valid
  bool ImportDir::IsValid() const
  {
    NtHeaders const MyNtHeaders(m_PeFile);
    
    DWORD const DataDirSize(MyNtHeaders.GetDataDirectorySize(NtHeaders::
      DataDir_Import));
    DWORD const DataDirVa(MyNtHeaders.GetDataDirectoryVirtualAddress(
      NtHeaders::DataDir_Import));
    
    return DataDirSize && DataDirVa;
  }

  // Ensure import directory is valid
  void ImportDir::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ImportDir::EnsureValid") << 
        ErrorString("Import directory is invalid."));
    }
  }

  // Get import directory base
  PVOID ImportDir::GetBase() const
  {
    if (!m_pBase)
    {
      NtHeaders const MyNtHeaders(m_PeFile);
      
      DWORD const DataDirSize = MyNtHeaders.GetDataDirectorySize(NtHeaders::
        DataDir_Import);
      DWORD const DataDirVa = MyNtHeaders.GetDataDirectoryVirtualAddress(
        NtHeaders::DataDir_Import);
      if (!DataDirSize || !DataDirVa)
      {
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("ImportDir::GetBase") << 
          ErrorString("PE file has no import directory."));
      }
      
      m_pBase = static_cast<PIMAGE_IMPORT_DESCRIPTOR>(m_PeFile.RvaToVa(
        DataDirVa));
    }
    
    return m_pBase;
  }

  // Get characteristics
  DWORD ImportDir::GetCharacteristics() const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, Characteristics));
  }

  // Get time and date stamp
  DWORD ImportDir::GetTimeDateStamp() const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, TimeDateStamp));
  }

  // Get forwarder chain
  DWORD ImportDir::GetForwarderChain() const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, ForwarderChain));
  }

  // Get name (raw)
  DWORD ImportDir::GetNameRaw() const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, Name));
  }

  // Get name
  std::string ImportDir::GetName() const
  {
    return m_Memory.ReadString<std::string>(m_PeFile.RvaToVa(GetNameRaw()));
  }

  // Get first thunk
  DWORD ImportDir::GetFirstThunk() const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, FirstThunk));
  }

  // Set characteristics
  void ImportDir::SetCharacteristics(DWORD Characteristics) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, 
      Characteristics), Characteristics);
  }

  // Set time and date stamp
  void ImportDir::SetTimeDateStamp(DWORD TimeDateStamp) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, 
      TimeDateStamp), TimeDateStamp);
  }

  // Set forwarder chain
  void ImportDir::SetForwarderChain(DWORD ForwarderChain) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, 
      ForwarderChain), ForwarderChain);
  }

  // Set name
  void ImportDir::SetNameRaw(DWORD Name) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, 
      Name), Name);
  }

  // Set name (raw)
  void ImportDir::SetName(std::string const& Name) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    DWORD NameRva = m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_IMPORT_DESCRIPTOR, Name));
    if (!NameRva)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ImportDir::SetName") << 
        ErrorString("Name RVA is null."));
    }
    
    PVOID pName = m_PeFile.RvaToVa(NameRva);
    if (!pName)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ImportDir::SetName") << 
        ErrorString("Name VA is null."));
    }
      
    std::string const CurrentName = m_Memory.ReadString<std::string>(pName);
    if (Name.size() > CurrentName.size())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ImportDir::SetName") << 
        ErrorString("New name longer than existing name."));
    }
      
    return m_Memory.WriteString(pName, Name);
  }

  // Set first thunk
  void ImportDir::SetFirstThunk(DWORD FirstThunk) const
  {
    PBYTE pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_IMPORT_DESCRIPTOR, 
      FirstThunk), FirstThunk);
  }
  
  // Equality operator
  bool ImportDir::operator==(ImportDir const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool ImportDir::operator!=(ImportDir const& Rhs) const
  {
    return !(*this == Rhs);
  }
  
  // Constructor
  ImportDirList::ImportDirList(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Cache()
  { }
    
  // Move constructor
  ImportDirList::ImportDirList(ImportDirList&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Cache(std::move(Other.m_Cache))
  { }
  
  // Move assignment operator
  ImportDirList& ImportDirList::operator=(ImportDirList&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    this->m_Cache = std::move(Other.m_Cache);
    
    return *this;
  }
  
  // Get start of import dir list
  ImportDirList::iterator ImportDirList::begin()
  {
    return iterator(*this);
  }
  
  // Get end of import dir list
  ImportDirList::iterator ImportDirList::end()
  {
    return iterator();
  }
  
  // Get start of import dir list
  ImportDirList::const_iterator ImportDirList::begin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of import dir list
  ImportDirList::const_iterator ImportDirList::end() const
  {
    return const_iterator();
  }
  
  // Get start of import dir list
  ImportDirList::const_iterator ImportDirList::cbegin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of import dir list
  ImportDirList::const_iterator ImportDirList::cend() const
  {
    return const_iterator();
  }
  
  // Get import dir from cache by number
  boost::optional<ImportDir&> ImportDirList::GetByNum(DWORD Num) const
  {
    while (Num >= m_Cache.size())
    {
      ImportDir const TempImportDir(m_PeFile);
      if (!TempImportDir.IsValid() || !TempImportDir.GetCharacteristics())
      {
        return boost::optional<ImportDir&>();
      }
      else
      {
        auto pImpDesc = static_cast<PIMAGE_IMPORT_DESCRIPTOR>(
          TempImportDir.GetBase());
        ImportDir const MyImportDir(m_PeFile, pImpDesc + m_Cache.size());
        if (!MyImportDir.IsValid() || !MyImportDir.GetCharacteristics())
        {
          return boost::optional<ImportDir&>();
        }
        
        m_Cache.push_back(MyImportDir);
      }
    }
    
    return m_Cache[Num];
  }

  // Constructor
  ImportThunk::ImportThunk(PeFile const& MyPeFile, PVOID pThunk) 
    : m_PeFile(MyPeFile), 
    m_Memory(MyPeFile.GetMemoryMgr()), 
    m_pBase(reinterpret_cast<PBYTE>(pThunk))
  { }
      
  // Copy constructor
  ImportThunk::ImportThunk(ImportThunk const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  ImportThunk& ImportThunk::operator=(ImportThunk const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  ImportThunk::ImportThunk(ImportThunk&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  ImportThunk& ImportThunk::operator=(ImportThunk&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  ImportThunk::~ImportThunk()
  { }
  
  // Whether thunk is valid
  bool ImportThunk::IsValid() const
  {
    return GetAddressOfData() != 0;
  }

  // Ensure thunk is valid
  void ImportThunk::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ImportThunk::EnsureValid") << 
        ErrorString("Import thunk is invalid."));
    }
  }

  // Get address of data
  DWORD_PTR ImportThunk::GetAddressOfData() const
  {
    return m_Memory.Read<DWORD_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_THUNK_DATA, u1.AddressOfData));
  }

  // Set address of data
  void ImportThunk::SetAddressOfData(DWORD_PTR AddressOfData) const
  {
    return m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_THUNK_DATA, 
      u1.AddressOfData), AddressOfData);
  }

  // Get ordinal (raw)
  DWORD_PTR ImportThunk::GetOrdinalRaw() const
  {
    return m_Memory.Read<DWORD_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_THUNK_DATA, u1.Ordinal));
  }

  // Set ordinal (raw)
  void ImportThunk::SetOrdinalRaw(DWORD_PTR OrdinalRaw) const
  {
    return m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_THUNK_DATA, 
      u1.Ordinal), OrdinalRaw);
  }

  // Whether import is by ordinal
  bool ImportThunk::ByOrdinal() const
  {
    return IMAGE_SNAP_BY_ORDINAL(GetOrdinalRaw());
  }

  // Get ordinal
  WORD ImportThunk::GetOrdinal() const
  {
    return IMAGE_ORDINAL(GetOrdinalRaw());
  }

  // Get function
  DWORD_PTR ImportThunk::GetFunction() const
  {
    return m_Memory.Read<DWORD_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_THUNK_DATA, u1.Function));
  }

  // Set function
  void ImportThunk::SetFunction(DWORD_PTR Function) const
  {
    return m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_THUNK_DATA, 
      u1.Function), Function);
  }

  // Get hint
  WORD ImportThunk::GetHint() const
  {
    PBYTE const pNameImport = static_cast<PBYTE>(m_PeFile.RvaToVa(
      static_cast<DWORD>(GetAddressOfData())));
    return m_Memory.Read<WORD>(pNameImport + FIELD_OFFSET(
      IMAGE_IMPORT_BY_NAME, Hint));
  }

  // Set hint
  void ImportThunk::SetHint(WORD Hint) const
  {
    PBYTE const pNameImport = static_cast<PBYTE>(m_PeFile.RvaToVa(
      static_cast<DWORD>(GetAddressOfData())));
    return m_Memory.Write(pNameImport + FIELD_OFFSET(IMAGE_IMPORT_BY_NAME, 
      Hint), Hint);
  }

  // Get name
  std::string ImportThunk::GetName() const
  {
    PBYTE const pNameImport = static_cast<PBYTE>(m_PeFile.RvaToVa(
      static_cast<DWORD>(GetAddressOfData())));
    return m_Memory.ReadString<std::string>(pNameImport + FIELD_OFFSET(
      IMAGE_IMPORT_BY_NAME, Name));
  }
  
  // Get base
  PVOID ImportThunk::GetBase() const
  {
    return m_pBase;
  }
  
  // Equality operator
  bool ImportThunk::operator==(ImportThunk const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool ImportThunk::operator!=(ImportThunk const& Rhs) const
  {
    return !(*this == Rhs);
  }
  
  // Constructor
  ImportThunkList::ImportThunkList(PeFile const& MyPeFile, DWORD FirstThunk)
    : m_PeFile(MyPeFile), 
    m_FirstThunk(FirstThunk), 
    m_Cache()
  { }
    
  // Move constructor
  ImportThunkList::ImportThunkList(ImportThunkList&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_FirstThunk(Other.m_FirstThunk), 
    m_Cache(std::move(Other.m_Cache))
  {
    Other.m_FirstThunk = 0;
  }
  
  // Move assignment operator
  ImportThunkList& ImportThunkList::operator=(ImportThunkList&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
      
    this->m_FirstThunk = Other.m_FirstThunk;
    Other.m_FirstThunk = 0;
    
    this->m_Cache = std::move(Other.m_Cache);
    
    return *this;
  }
  
  // Get start of import thunk list
  ImportThunkList::iterator ImportThunkList::begin()
  {
    return iterator(*this);
  }
  
  // Get end of import thunk list
  ImportThunkList::iterator ImportThunkList::end()
  {
    return iterator();
  }
  
  // Get start of import thunk list
  ImportThunkList::const_iterator ImportThunkList::begin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of import thunk list
  ImportThunkList::const_iterator ImportThunkList::end() const
  {
    return const_iterator();
  }
  
  // Get start of import thunk list
  ImportThunkList::const_iterator ImportThunkList::cbegin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of import thunk list
  ImportThunkList::const_iterator ImportThunkList::cend() const
  {
    return const_iterator();
  }
  
  // Get import thunk from cache by number
  boost::optional<ImportThunk&> ImportThunkList::GetByNum(DWORD Num) const
  {
    while (Num >= m_Cache.size())
    {
      auto pThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(m_PeFile.RvaToVa(
        m_FirstThunk));
      ImportThunk const MyImportThunk(m_PeFile, pThunk + m_Cache.size());
      if (!MyImportThunk.IsValid())
      {
        return boost::optional<ImportThunk&>();
      }
      else
      {
        m_Cache.push_back(MyImportThunk);
      }
    }
    
    return m_Cache[Num];
  }
}
