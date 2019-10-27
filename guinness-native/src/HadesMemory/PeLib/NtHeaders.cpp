// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/NtHeaders.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/PeLib/DosHeader.hpp>

namespace HadesMem
{
  // Constructor
  NtHeaders::NtHeaders(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_pBase(nullptr)
  {
    DosHeader MyDosHeader(m_PeFile);
    m_pBase = static_cast<PBYTE>(m_PeFile.GetBase()) + 
      MyDosHeader.GetNewHeaderOffset();
    
    EnsureValid();
  }
      
  // Copy constructor
  NtHeaders::NtHeaders(NtHeaders const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  NtHeaders& NtHeaders::operator=(NtHeaders const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  NtHeaders::NtHeaders(NtHeaders&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  NtHeaders& NtHeaders::operator=(NtHeaders&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  NtHeaders::~NtHeaders()
  { }

  // Get base of NT headers
  PVOID NtHeaders::GetBase() const
  {
    return m_pBase;
  }

  // Whether signature is valid
  bool NtHeaders::IsValid() const
  {
    return IMAGE_NT_SIGNATURE == GetSignature();
  }

  // Ensure signature is valid
  void NtHeaders::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("NtHeaders::EnsureValid") << 
        ErrorString("NT headers signature invalid."));
    }
  }

  // Get signature
  DWORD NtHeaders::GetSignature() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      Signature));
  }

  // Get machine
  WORD NtHeaders::GetMachine() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.Machine));
  }

  // Get number of sections
  WORD NtHeaders::GetNumberOfSections() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.NumberOfSections));
  }

  // Get time date stamp
  DWORD NtHeaders::GetTimeDateStamp() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.TimeDateStamp));
  }

  // Get pointer to symbol table
  DWORD NtHeaders::GetPointerToSymbolTable() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.PointerToSymbolTable));
  }

  // Get number of symbols
  DWORD NtHeaders::GetNumberOfSymbols() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.NumberOfSymbols));
  }

  // Get size of optional header
  WORD NtHeaders::GetSizeOfOptionalHeader() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.SizeOfOptionalHeader));
  }

  // Get characteristics
  WORD NtHeaders::GetCharacteristics() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      FileHeader.Characteristics));
  }

  // Get magic
  WORD NtHeaders::GetMagic() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Magic));
  }

  // Get major linker version
  BYTE NtHeaders::GetMajorLinkerVersion() const
  {
    return m_Memory.Read<BYTE>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorLinkerVersion));
  }

  // Get minor linker version
  BYTE NtHeaders::GetMinorLinkerVersion() const
  {
    return m_Memory.Read<BYTE>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorLinkerVersion));
  }

  // Get size of code
  DWORD NtHeaders::GetSizeOfCode() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfCode));
  }

  // Get size of initialized data
  DWORD NtHeaders::GetSizeOfInitializedData() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfInitializedData));
  }

  // Get size of uninitialized data
  DWORD NtHeaders::GetSizeOfUninitializedData() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfUninitializedData));
  }

  // Get address of entry point
  DWORD NtHeaders::GetAddressOfEntryPoint() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.AddressOfEntryPoint));
  }

  // Get base of code
  DWORD NtHeaders::GetBaseOfCode() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.BaseOfCode));
  }

#if defined(_M_IX86) 
  // Get base of data
  DWORD NtHeaders::GetBaseOfData() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.BaseOfData));
  }
#endif

  // Get image base
  ULONG_PTR NtHeaders::GetImageBase() const
  {
    return m_Memory.Read<ULONG_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_NT_HEADERS, OptionalHeader.ImageBase));
  }

  // Get section alignment
  DWORD NtHeaders::GetSectionAlignment() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SectionAlignment));
  }

  // Get file alignment
  DWORD NtHeaders::GetFileAlignment() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.FileAlignment));
  }

  // Get major operating system version
  WORD NtHeaders::GetMajorOperatingSystemVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorOperatingSystemVersion));
  }

  // Get minor operating system version
  WORD NtHeaders::GetMinorOperatingSystemVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorOperatingSystemVersion));
  }

  // Get major image version
  WORD NtHeaders::GetMajorImageVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorImageVersion));
  }

  // Get minor image version
  WORD NtHeaders::GetMinorImageVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorImageVersion));
  }

  // Get major subsystem version
  WORD NtHeaders::GetMajorSubsystemVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorSubsystemVersion));
  }

  // Get minor subsystem version
  WORD NtHeaders::GetMinorSubsystemVersion() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorSubsystemVersion));
  }

  // Get Win32 version value
  DWORD NtHeaders::GetWin32VersionValue() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Win32VersionValue));
  }

  // Get size of image
  DWORD NtHeaders::GetSizeOfImage() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfImage));
  }

  // Get size of headers
  DWORD NtHeaders::GetSizeOfHeaders() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfHeaders));
  }

  // Get checksum
  DWORD NtHeaders::GetCheckSum() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.CheckSum));
  }

  // Get subsystem
  WORD NtHeaders::GetSubsystem() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Subsystem));
  }

  // Get DLL characteristics
  WORD NtHeaders::GetDllCharacteristics() const
  {
    return m_Memory.Read<WORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DllCharacteristics));
  }

  // Get size of stack reserve
  ULONG_PTR NtHeaders::GetSizeOfStackReserve() const
  {
    return m_Memory.Read<ULONG_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_NT_HEADERS, OptionalHeader.SizeOfStackReserve));
  }

  // Get size of stack commit
  ULONG_PTR NtHeaders::GetSizeOfStackCommit() const
  {
    return m_Memory.Read<ULONG_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_NT_HEADERS, OptionalHeader.SizeOfStackCommit));
  }

  // Get size of heap reserve
  ULONG_PTR NtHeaders::GetSizeOfHeapReserve() const
  {
    return m_Memory.Read<ULONG_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_NT_HEADERS, OptionalHeader.SizeOfHeapReserve));
  }

  // Get size of heap commit
  ULONG_PTR NtHeaders::GetSizeOfHeapCommit() const
  {
    return m_Memory.Read<ULONG_PTR>(m_pBase + FIELD_OFFSET(
      IMAGE_NT_HEADERS, OptionalHeader.SizeOfHeapCommit));
  }

  // Get loader flags
  DWORD NtHeaders::GetLoaderFlags() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.LoaderFlags));
  }

  // Get number of RVA and sizes
  DWORD NtHeaders::GetNumberOfRvaAndSizes() const
  {
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.NumberOfRvaAndSizes));
  }

  // Get data directory virtual address
  DWORD NtHeaders::GetDataDirectoryVirtualAddress(DataDir MyDataDir) const
  {
    if (static_cast<DWORD>(MyDataDir) >= GetNumberOfRvaAndSizes())
    {
      return 0;
    }
    
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DataDirectory[0]) + MyDataDir * sizeof(
      IMAGE_DATA_DIRECTORY) + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, 
      VirtualAddress));
  }

  // Get data directory size
  DWORD NtHeaders::GetDataDirectorySize(DataDir MyDataDir) const
  {
    if (static_cast<DWORD>(MyDataDir) >= GetNumberOfRvaAndSizes())
    {
      return 0;
    }
    
    return m_Memory.Read<DWORD>(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DataDirectory[0]) + MyDataDir * sizeof(
      IMAGE_DATA_DIRECTORY) + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, 
      Size));
  }

  // Set signature
  void NtHeaders::SetSignature(DWORD Signature) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, Signature), 
      Signature);
  }

  // Set machine
  void NtHeaders::SetMachine(WORD Machine) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      Machine), Machine);
  }

  // Set number of sections
  void NtHeaders::SetNumberOfSections(WORD NumberOfSections) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      NumberOfSections), NumberOfSections);
  }

  // Set time date stamp
  void NtHeaders::SetTimeDateStamp(DWORD TimeDateStamp) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      TimeDateStamp), TimeDateStamp);
  }

  // Set pointer to symbol table
  void NtHeaders::SetPointerToSymbolTable(DWORD PointerToSymbolTable) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      PointerToSymbolTable), PointerToSymbolTable);
  }

  // Set number of symbols
  void NtHeaders::SetNumberOfSymbols(DWORD NumberOfSymbols) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      NumberOfSymbols), NumberOfSymbols);
  }

  // Set size of optional header
  void NtHeaders::SetSizeOfOptionalHeader(WORD SizeOfOptionalHeader) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      SizeOfOptionalHeader), SizeOfOptionalHeader);

  }

  // Set characteristics
  void NtHeaders::SetCharacteristics(WORD Characteristics) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, FileHeader.
      Characteristics), Characteristics);
  }

  // Set magic
  void NtHeaders::SetMagic(WORD Magic) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Magic), Magic);
  }

  // Set major linker version
  void NtHeaders::SetMajorLinkerVersion(BYTE MajorLinkerVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorLinkerVersion), MajorLinkerVersion);
  }

  // Set minor linker version
  void NtHeaders::SetMinorLinkerVersion(BYTE MinorLinkerVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorLinkerVersion), MinorLinkerVersion);
  }

  // Set size of code
  void NtHeaders::SetSizeOfCode(DWORD SizeOfCode) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfCode), SizeOfCode);
  }

  // Set size of initialized data
  void NtHeaders::SetSizeOfInitializedData(DWORD SizeOfInitializedData) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfInitializedData), SizeOfInitializedData);
  }

  // Set size of uninitialized data
  void NtHeaders::SetSizeOfUninitializedData(DWORD SizeOfUninitializedData) 
    const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfUninitializedData), SizeOfUninitializedData);
  }

  // Set address of entry point
  void NtHeaders::SetAddressOfEntryPoint(DWORD AddressOfEntryPoint) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.AddressOfEntryPoint), AddressOfEntryPoint);
  }

  // Set base of code
  void NtHeaders::SetBaseOfCode(DWORD BaseOfCode) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.BaseOfCode), BaseOfCode);
  }

#if defined(_M_IX86) 
  // Set base of data
  void NtHeaders::SetBaseOfData(DWORD BaseOfData) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.BaseOfData), BaseOfData);
  }
#endif

  // Set image base
  void NtHeaders::SetImageBase(ULONG_PTR ImageBase) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.ImageBase), ImageBase);
  }

  // Set section alignment
  void NtHeaders::SetSectionAlignment(DWORD SectionAlignment) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SectionAlignment), SectionAlignment);
  }

  // Set file alignment
  void NtHeaders::SetFileAlignment(DWORD FileAlignment) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.FileAlignment), FileAlignment);
  }

  // Set major operating system version
  void NtHeaders::SetMajorOperatingSystemVersion(
    WORD MajorOperatingSystemVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorOperatingSystemVersion), 
      MajorOperatingSystemVersion);
  }

  // Set minor operating system version
  void NtHeaders::SetMinorOperatingSystemVersion(
    WORD MinorOperatingSystemVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorOperatingSystemVersion), 
      MinorOperatingSystemVersion);
  }

  // Set major image version
  void NtHeaders::SetMajorImageVersion(WORD MajorImageVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorImageVersion), MajorImageVersion);
  }

  // Set minor image version
  void NtHeaders::SetMinorImageVersion(WORD MinorImageVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorImageVersion), MinorImageVersion);
  }

  // Set major subsystem version
  void NtHeaders::SetMajorSubsystemVersion(WORD MajorSubsystemVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MajorSubsystemVersion), MajorSubsystemVersion);
  }

  // Set minor subsystem version
  void NtHeaders::SetMinorSubsystemVersion(WORD MinorSubsystemVersion) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.MinorSubsystemVersion), MinorSubsystemVersion);
  }

  // Set Win32 version value
  void NtHeaders::SetWin32VersionValue(DWORD Win32VersionValue) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Win32VersionValue), Win32VersionValue);
  }

  // Set size of image
  void NtHeaders::SetSizeOfImage(DWORD SizeOfImage) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfImage), SizeOfImage);
  }

  // Set size of headers
  void NtHeaders::SetSizeOfHeaders(DWORD SizeOfHeaders) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfHeaders), SizeOfHeaders);
  }

  // Set checksum
  void NtHeaders::SetCheckSum(DWORD CheckSum) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.CheckSum), CheckSum);
  }

  // Set subsystem
  void NtHeaders::SetSubsystem(WORD Subsystem) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.Subsystem), Subsystem);
  }

  // Set DLL characteristics
  void NtHeaders::SetDllCharacteristics(WORD DllCharacteristics) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DllCharacteristics), DllCharacteristics);
  }

  // Set size of stack reserve
  void NtHeaders::SetSizeOfStackReserve(ULONG_PTR SizeOfStackReserve) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfStackReserve), SizeOfStackReserve);
  }

  // Set size of stack commit
  void NtHeaders::SetSizeOfStackCommit(ULONG_PTR SizeOfStackCommit) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfStackCommit), SizeOfStackCommit);
  }

  // Set size of heap reserve
  void NtHeaders::SetSizeOfHeapReserve(ULONG_PTR SizeOfHeapReserve) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfHeapReserve), SizeOfHeapReserve);
  }

  // Set size of heap commit
  void NtHeaders::SetSizeOfHeapCommit(ULONG_PTR SizeOfHeapCommit) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.SizeOfHeapCommit), SizeOfHeapCommit);
  }

  // Set loader flags
  void NtHeaders::SetLoaderFlags(DWORD LoaderFlags) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.LoaderFlags), LoaderFlags);
  }

  // Set number of RVA and sizes
  void NtHeaders::SetNumberOfRvaAndSizes(DWORD NumberOfRvaAndSizes) const
  {
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.NumberOfRvaAndSizes), NumberOfRvaAndSizes);
  }

  // Set data directory virtual address
  void NtHeaders::SetDataDirectoryVirtualAddress(DataDir MyDataDir, 
    DWORD DataDirectoryVirtualAddress) const
  {
    if (static_cast<DWORD>(MyDataDir) >= GetNumberOfRvaAndSizes())
    {
      return;
    }
    
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DataDirectory[0]) + MyDataDir * sizeof(
      IMAGE_DATA_DIRECTORY) + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, 
      VirtualAddress), DataDirectoryVirtualAddress);
  }

  // Set data directory size
  void NtHeaders::SetDataDirectorySize(DataDir MyDataDir, 
    DWORD DataDirectorySize) const
  {
    if (static_cast<DWORD>(MyDataDir) >= GetNumberOfRvaAndSizes())
    {
      return;
    }
    
    m_Memory.Write(m_pBase + FIELD_OFFSET(IMAGE_NT_HEADERS, 
      OptionalHeader.DataDirectory[0]) + MyDataDir * sizeof(
      IMAGE_DATA_DIRECTORY) + FIELD_OFFSET(IMAGE_DATA_DIRECTORY, Size), 
      DataDirectorySize);
  }
  
  // Equality operator
  bool NtHeaders::operator==(NtHeaders const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool NtHeaders::operator!=(NtHeaders const& Rhs) const
  {
    return !(*this == Rhs);
  }
}
