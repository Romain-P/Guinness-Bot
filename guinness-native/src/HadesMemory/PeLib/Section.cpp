// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/Section.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/PeLib/NtHeaders.hpp>

#include <array>

namespace HadesMem
{
  // Constructor
  Section::Section(PeFile const& MyPeFile, WORD Number)
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_SectionNum(Number), 
    m_pBase(nullptr)
  { }
      
  // Copy constructor
  Section::Section(Section const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_SectionNum(Other.m_SectionNum), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  Section& Section::operator=(Section const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_SectionNum = Other.m_SectionNum;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  Section::Section(Section&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_SectionNum(Other.m_SectionNum), 
    m_pBase(Other.m_pBase)
  {
    Other.m_SectionNum = 0;
    
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  Section& Section::operator=(Section&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_SectionNum = Other.m_SectionNum;
    Other.m_SectionNum = 0;
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  Section::~Section()
  { }

  // Get section header base
  PVOID Section::GetBase() const
  {
    if (!m_pBase)
    {
      NtHeaders const MyNtHeaders(m_PeFile);
      if (m_SectionNum >= MyNtHeaders.GetNumberOfSections())
      {
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("Section::GetBase") << 
          ErrorString("Invalid section number."));
      }
      
      auto const NtHeadersRaw = m_Memory.Read<IMAGE_NT_HEADERS>(
        MyNtHeaders.GetBase());
      
      PIMAGE_SECTION_HEADER pSectionHeader = 
        reinterpret_cast<PIMAGE_SECTION_HEADER>(static_cast<PBYTE>(
        MyNtHeaders.GetBase()) + FIELD_OFFSET(IMAGE_NT_HEADERS, 
        OptionalHeader) + NtHeadersRaw.FileHeader.SizeOfOptionalHeader);
      
      pSectionHeader += m_SectionNum;
      
      m_pBase = reinterpret_cast<PBYTE>(pSectionHeader);
    }
    
    return m_pBase;
  }
  
  // Get section number
  WORD Section::GetNumber() const
  {
    return m_SectionNum;
  }

  // Get name
  std::string Section::GetName() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    std::array<char, 8> const NameData(m_Memory.Read<std::array<char, 8>>(
      pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, Name)));
    
    std::string Name;
    for (std::size_t i = 0; i < 8 && NameData[i]; ++i)
    {
      Name += NameData[i];
    }
    
    return Name;
  }

  // Get virtual address
  DWORD Section::GetVirtualAddress() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      VirtualAddress));
  }

  // Get virtual size
  DWORD Section::GetVirtualSize() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, Misc.VirtualSize));
  }

  // Get size of raw data
  DWORD Section::GetSizeOfRawData() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, SizeOfRawData));
  }

  // Get pointer to raw data
  DWORD Section::GetPointerToRawData() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, PointerToRawData));
  }

  // Get pointer to relocations
  DWORD Section::GetPointerToRelocations() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, PointerToRelocations));
  }

  // Get pointer to line numbers
  DWORD Section::GetPointerToLinenumbers() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, PointerToLinenumbers));
  }

  // Get number of relocations
  WORD Section::GetNumberOfRelocations() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<WORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, NumberOfRelocations));
  }

  // Get number of line numbers
  WORD Section::GetNumberOfLinenumbers() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<WORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, NumberOfLinenumbers));
  }

  // Get characteristics
  DWORD Section::GetCharacteristics() const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pBase + FIELD_OFFSET(
      IMAGE_SECTION_HEADER, Characteristics));
  }

  // Set name
  void Section::SetName(std::string const& Name) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.WriteString(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, Name), 
      Name);
  }

  // Set virtual address
  void Section::SetVirtualAddress(DWORD VirtualAddress) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      VirtualAddress), VirtualAddress);
  }

  // Set virtual size
  void Section::SetVirtualSize(DWORD VirtualSize) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      Misc.VirtualSize), VirtualSize);
  }

  // Set size of raw data
  void Section::SetSizeOfRawData(DWORD SizeOfRawData) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      SizeOfRawData), SizeOfRawData);
  }

  // Set pointer to raw data
  void Section::SetPointerToRawData(DWORD PointerToRawData) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      PointerToRawData), PointerToRawData);
  }

  // Set pointer to relocations
  void Section::SetPointerToRelocations(DWORD PointerToRelocations) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      PointerToRelocations), PointerToRelocations);
  }

  // Set pointer to line numbers
  void Section::SetPointerToLinenumbers(DWORD PointerToLinenumbers) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      PointerToLinenumbers), PointerToLinenumbers);
  }

  // Set number of relocations
  void Section::SetNumberOfRelocations(WORD NumberOfRelocations) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      NumberOfRelocations), NumberOfRelocations);
  }

  // Set number of line numbers
  void Section::SetNumberOfLinenumbers(WORD NumberOfLinenumbers) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      NumberOfLinenumbers), NumberOfLinenumbers);
  }

  // Set characteristics
  void Section::SetCharacteristics(DWORD Characteristics) const
  {
    PBYTE const pBase = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pBase + FIELD_OFFSET(IMAGE_SECTION_HEADER, 
      Characteristics), Characteristics);
  }
  
  // Constructor
  SectionList::SectionList(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Cache()
  { }
    
  // Move constructor
  SectionList::SectionList(SectionList&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Cache(std::move(Other.m_Cache))
  { }
  
  // Move assignment operator
  SectionList& SectionList::operator=(SectionList&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    this->m_Cache = std::move(Other.m_Cache);
    
    return *this;
  }
  
  // Get start of section list
  SectionList::iterator SectionList::begin()
  {
    return iterator(*this);
  }
  
  // Get end of section list
  SectionList::iterator SectionList::end()
  {
    return iterator();
  }
  
  // Get start of section list
  SectionList::const_iterator SectionList::begin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of section list
  SectionList::const_iterator SectionList::end() const
  {
    return const_iterator();
  }
  
  // Get start of section list
  SectionList::const_iterator SectionList::cbegin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of section list
  SectionList::const_iterator SectionList::cend() const
  {
    return const_iterator();
  }
  
  // Get section from cache by number
  boost::optional<Section&> SectionList::GetByNum(DWORD Num) const
  {
    while (Num >= m_Cache.size())
    {
      if (m_Cache.size() >= NtHeaders(m_PeFile).GetNumberOfSections())
      {
        return boost::optional<Section&>();
      }
      
      m_Cache.push_back(Section(m_PeFile, static_cast<WORD>(m_Cache.size())));
    }
    
    return m_Cache[Num];
  }
  
  // Equality operator
  bool Section::operator==(Section const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool Section::operator!=(Section const& Rhs) const
  {
    return !(*this == Rhs);
  }
}
