// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/DosHeader.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>

namespace HadesMem
{
  // Constructor
  DosHeader::DosHeader(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_pBase(static_cast<PBYTE>(m_PeFile.GetBase()))
  {
    EnsureValid();
  }
      
  // Copy constructor
  DosHeader::DosHeader(DosHeader const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  DosHeader& DosHeader::operator=(DosHeader const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  DosHeader::DosHeader(DosHeader&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  DosHeader& DosHeader::operator=(DosHeader&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  DosHeader::~DosHeader()
  { }
  
  // Get base
  PVOID DosHeader::GetBase() const
  {
    return m_pBase;
  }

  // Whether magic is valid
  bool DosHeader::IsValid() const
  {
    return IMAGE_DOS_SIGNATURE == GetMagic();
  }

  // Ensure magic is valid
  void DosHeader::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("DosHeader::EnsureValid") << 
        ErrorString("DOS header magic invalid."));
    }
  }

  // Get magic
  WORD DosHeader::GetMagic() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_magic));
  }

  // Get bytes on last page
  WORD DosHeader::GetBytesOnLastPage() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_cblp));
  }

  // Get pages in file
  WORD DosHeader::GetPagesInFile() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_cp));

  }

  // Get relocations
  WORD DosHeader::GetRelocations() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_crlc));
  }

  // Get size of header in paragraphs
  WORD DosHeader::GetSizeOfHeaderInParagraphs() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_cparhdr));
  }

  // Get minimum extra paragraphs needed
  WORD DosHeader::GetMinExtraParagraphs() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_minalloc));
  }

  // Get maximum extra paragraphs needed
  WORD DosHeader::GetMaxExtraParagraphs() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_maxalloc));
  }

  // Get initial SS value
  WORD DosHeader::GetInitialSS() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_ss));
  }

  // Get initial SP value
  WORD DosHeader::GetInitialSP() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_sp));
  }

  // Get checksum
  WORD DosHeader::GetChecksum() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_csum));
  }

  // Get initial IP value
  WORD DosHeader::GetInitialIP() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_ip));
  }

  // Get initial CS value
  WORD DosHeader::GetInitialCS() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_cs));
  }

  // Get file address of reloc table
  WORD DosHeader::GetRelocTableFileAddr() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_lfarlc));
  }

  // Get overlay number
  WORD DosHeader::GetOverlayNum() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_ovno));
  }

  // Get first set of reserved words
  std::array<WORD, 4> DosHeader::GetReservedWords1() const
  {
    return m_Memory.Read<std::array<WORD, 4>>(m_pBase + FIELD_OFFSET(
      IMAGE_DOS_HEADER, e_res));
  }

  // Get OEM ID
  WORD DosHeader::GetOEMID() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_oemid));
  }

  // Get OEM info
  WORD DosHeader::GetOEMInfo() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_oeminfo));
  }

  // Get second set of reserved words
  std::array<WORD, 10> DosHeader::GetReservedWords2() const
  {
    return m_Memory.Read<std::array<WORD, 10>>(m_pBase + FIELD_OFFSET(
      IMAGE_DOS_HEADER, e_res2));
  }

  // Get new header offset
  LONG DosHeader::GetNewHeaderOffset() const
  {
    return m_Memory.Read<LONG>(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, 
      e_lfanew));
  }

  // Set magic
  void DosHeader::SetMagic(WORD Magic) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic), 
      Magic);
  }

  // Set bytes on last page
  void DosHeader::SetBytesOnLastPage(WORD BytesOnLastPage) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_cblp), 
      BytesOnLastPage);
  }

  // Set pages in file
  void DosHeader::SetPagesInFile(WORD PagesInFile) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_cp), 
      PagesInFile);
  }

  // Set relocations
  void DosHeader::SetRelocations(WORD Relocations) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_crlc), 
      Relocations);
  }

  // Set size of header in paragraphs
  void DosHeader::SetSizeOfHeaderInParagraphs(WORD SizeOfHeaderInParagraphs) 
    const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_cparhdr), 
      SizeOfHeaderInParagraphs);
  }

  // Set min extra paragraphs
  void DosHeader::SetMinExtraParagraphs(WORD MinExtraParagraphs) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_minalloc), 
      MinExtraParagraphs);
  }

  // Set max extra paragraphs
  void DosHeader::SetMaxExtraParagraphs(WORD MaxExtraParagraphs) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_maxalloc), 
      MaxExtraParagraphs);
  }

  // Set initial SS
  void DosHeader::SetInitialSS(WORD InitialSS) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_ss), 
      InitialSS);
  }

  // Set initial SP
  void DosHeader::SetInitialSP(WORD InitialSP) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_sp), 
      InitialSP);
  }

  // Set checksum
  void DosHeader::SetChecksum(WORD Checksum) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_csum), 
      Checksum);
  }

  // Set initial IP
  void DosHeader::SetInitialIP(WORD InitialIP) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_ip), 
      InitialIP);
  }

  // Set initial CS
  void DosHeader::SetInitialCS(WORD InitialCS) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_cs), 
      InitialCS);
  }

  // Set reloc table file address
  void DosHeader::SetRelocTableFileAddr(WORD RelocTableFileAddr) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfarlc), 
      RelocTableFileAddr);
  }

  // Set overlay number
  void DosHeader::SetOverlayNum(WORD OverlayNum) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_ovno), 
      OverlayNum);
  }

  // Set first set of reserved words
  void DosHeader::SetReservedWords1(std::array<WORD, 4> const& 
    ReservedWords1) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_res), 
      ReservedWords1);
  }

  // Set OEM ID
  void DosHeader::SetOEMID(WORD OEMID) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_oemid), 
      OEMID);
  }

  // Set OEM info
  void DosHeader::SetOEMInfo(WORD OEMInfo) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_oeminfo), 
      OEMInfo);
  }

  // Set second set of reserved words
  void DosHeader::SetReservedWords2(std::array<WORD, 10> const& 
    ReservedWords2) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_res2), 
      ReservedWords2);
  }

  // Set new header offset
  void DosHeader::SetNewHeaderOffset(LONG Offset) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew), 
      Offset);
  }
  
  // Equality operator
  bool DosHeader::operator==(DosHeader const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool DosHeader::operator!=(DosHeader const& Rhs) const
  {
    return !(*this == Rhs);
  }
}
