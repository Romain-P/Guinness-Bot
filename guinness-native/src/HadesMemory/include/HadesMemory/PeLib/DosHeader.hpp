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

#include <array>

#include <Windows.h>

namespace HadesMem
{
  // PE file DOS header
  class DosHeader
  {
  public:
    // DOS header error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    explicit DosHeader(PeFile const& MyPeFile);
      
    // Copy constructor
    DosHeader(DosHeader const& Other);
    
    // Copy assignment operator
    DosHeader& operator=(DosHeader const& Other);
    
    // Move constructor
    DosHeader(DosHeader&& Other);
    
    // Move assignment operator
    DosHeader& operator=(DosHeader&& Other);
    
    // Destructor
    ~DosHeader();
    
    // Equality operator
    bool operator==(DosHeader const& Rhs) const;
    
    // Inequality operator
    bool operator!=(DosHeader const& Rhs) const;
    
    // Get base
    PVOID GetBase() const;

    // Whether magic is valid
    bool IsValid() const;

    // Ensure magic is valid
    void EnsureValid() const;

    // Get magic
    WORD GetMagic() const;

    // Get bytes on last page
    WORD GetBytesOnLastPage() const;

    // Get pages in file
    WORD GetPagesInFile() const;

    // Get relocations
    WORD GetRelocations() const;

    // Get size of header in paragraphs
    WORD GetSizeOfHeaderInParagraphs() const;

    // Get minimum extra paragraphs needed
    WORD GetMinExtraParagraphs() const;

    // Get maximum extra paragraphs needed
    WORD GetMaxExtraParagraphs() const;

    // Get initial SS value
    WORD GetInitialSS() const;

    // Get initial SP value
    WORD GetInitialSP() const;

    // Get checksum
    WORD GetChecksum() const;

    // Get initial IP value
    WORD GetInitialIP() const;

    // Get initial CS value
    WORD GetInitialCS() const;

    // Get file address of reloc table
    WORD GetRelocTableFileAddr() const;

    // Get overlay number
    WORD GetOverlayNum() const;

    // Get first set of reserved words
    std::array<WORD, 4> GetReservedWords1() const;

    // Get OEM ID
    WORD GetOEMID() const;

    // Get OEM info
    WORD GetOEMInfo() const;

    // Get second set of reserved words
    std::array<WORD, 10> GetReservedWords2() const;

    // Get new header offset
    LONG GetNewHeaderOffset() const;

    // Set magic
    void SetMagic(WORD Magic) const;

    // Set bytes on last page
    void SetBytesOnLastPage(WORD BytesOnLastPage) const;

    // Set pages in file
    void SetPagesInFile(WORD PagesInFile) const;

    // Set relocations
    void SetRelocations(WORD Relocations) const;

    // Set size of header in paragraphs
    void SetSizeOfHeaderInParagraphs(WORD SizeOfHeaderInParagraphs) const;

    // Set minimum extra paragraphs needed
    void SetMinExtraParagraphs(WORD MinExtraParagraphs) const;

    // Set maximum extra paragraphs needed
    void SetMaxExtraParagraphs(WORD MaxExtraParagraphs) const;

    // Set initial SS value
    void SetInitialSS(WORD InitialSS) const;

    // Set initial SP value
    void SetInitialSP(WORD InitialSP) const;

    // Set checksum
    void SetChecksum(WORD Checksum) const;

    // Set initial IP value
    void SetInitialIP(WORD InitialIP) const;

    // Set initial CS value
    void SetInitialCS(WORD InitialCS) const;

    // Set file address of reloc table
    void SetRelocTableFileAddr(WORD RelocTableFileAddr) const;

    // Set overlay number
    void SetOverlayNum(WORD OverlayNum) const;

    // Set first set of reserved words
    void SetReservedWords1(std::array<WORD, 4> const& ReservedWords1) const;

    // Set OEM ID
    void SetOEMID(WORD OEMID) const;

    // Set OEM info
    void SetOEMInfo(WORD OEMInfo) const;

    // Set second set of reserved words
    void SetReservedWords2(std::array<WORD, 10> const& ReservedWords2) const;

    // Set new header offset
    void SetNewHeaderOffset(LONG Offset) const;

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;

    // DOS header base
    PBYTE m_pBase;
  };
}
