// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/Detail/Fwd.hpp>
#include <HadesMemory/Detail/Error.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>

#include <Windows.h>

namespace HadesMem
{
  // PE file NT headers
  class NtHeaders
  {
  public:
    // NT headers error class
    class Error : public virtual HadesMemError
    { };

    // Data directory entries
    enum DataDir
    {
      DataDir_Export, 
      DataDir_Import, 
      DataDir_Resource, 
      DataDir_Exception, 
      DataDir_Security, 
      DataDir_BaseReloc, 
      DataDir_Debug, 
      DataDir_Architecture, 
      DataDir_GlobalPTR, 
      DataDir_TLS, 
      DataDir_LoadConfig, 
      DataDir_BoundImport, 
      DataDir_IAT, 
      DataDir_DelayImport, 
      DataDir_COMDescriptor
    };

    // Constructor
    explicit NtHeaders(PeFile const& MyPeFile);
      
    // Copy constructor
    NtHeaders(NtHeaders const& Other);
    
    // Copy assignment operator
    NtHeaders& operator=(NtHeaders const& Other);
    
    // Move constructor
    NtHeaders(NtHeaders&& Other);
    
    // Move assignment operator
    NtHeaders& operator=(NtHeaders&& Other);
    
    // Destructor
    ~NtHeaders();
    
    // Equality operator
    bool operator==(NtHeaders const& Rhs) const;
    
    // Inequality operator
    bool operator!=(NtHeaders const& Rhs) const;

    // Get base of NT headers
    PVOID GetBase() const;

    // Whether signature is valid
    bool IsValid() const;

    // Ensure signature is valid
    void EnsureValid() const;

    // Get signature
    DWORD GetSignature() const;

    // Get machine
    WORD GetMachine() const;

    // Get number of sections
    WORD GetNumberOfSections() const;

    // Get time date stamp
    DWORD GetTimeDateStamp() const;

    // Get pointer to symbol table
    DWORD GetPointerToSymbolTable() const;

    // Get number of symbols
    DWORD GetNumberOfSymbols() const;

    // Get size of optional header
    WORD GetSizeOfOptionalHeader() const;

    // Get characteristics
    WORD GetCharacteristics() const;

    // Get magic
    WORD GetMagic() const;

    // Get major linker version
    BYTE GetMajorLinkerVersion() const;

    // Get minor linker version
    BYTE GetMinorLinkerVersion() const;

    // Get minor linker version
    DWORD GetSizeOfCode() const;

    // Get minor linker version
    DWORD GetSizeOfInitializedData() const;

    // Get minor linker version
    DWORD GetSizeOfUninitializedData() const;

    // Get minor linker version
    DWORD GetAddressOfEntryPoint() const;

    // Get base of code
    DWORD GetBaseOfCode() const;

#if defined(_M_IX86) 
    // Get base of data
    DWORD GetBaseOfData() const;
#endif

    // Get base of code
    ULONG_PTR GetImageBase() const;

    // Get base of code
    DWORD GetSectionAlignment() const;

    // Get base of code
    DWORD GetFileAlignment() const;

    // Get base of code
    WORD GetMajorOperatingSystemVersion() const;

    // Get base of code
    WORD GetMinorOperatingSystemVersion() const;

    // Get base of code
    WORD GetMajorImageVersion() const;

    // Get base of code
    WORD GetMinorImageVersion() const;

    // Get base of code
    WORD GetMajorSubsystemVersion() const;

    // Get base of code
    WORD GetMinorSubsystemVersion() const;

    // Get base of code
    DWORD GetWin32VersionValue() const;

    // Get size of image
    DWORD GetSizeOfImage() const;

    // Get base of code
    DWORD GetSizeOfHeaders() const;

    // Get base of code
    DWORD GetCheckSum() const;

    // Get base of code
    WORD GetSubsystem() const;

    // Get base of code
    WORD GetDllCharacteristics() const;

    // Get base of code
    ULONG_PTR GetSizeOfStackReserve() const;

    // Get base of code
    ULONG_PTR GetSizeOfStackCommit() const;

    // Get base of code
    ULONG_PTR GetSizeOfHeapReserve() const;

    // Get base of code
    ULONG_PTR GetSizeOfHeapCommit() const;

    // Get base of code
    DWORD GetLoaderFlags() const;

    // Get base of code
    DWORD GetNumberOfRvaAndSizes() const;

    // Get base of code
    DWORD GetDataDirectoryVirtualAddress(DataDir MyDataDir) const;

    // Get base of code
    DWORD GetDataDirectorySize(DataDir MyDataDir) const;

    // Set signature
    void SetSignature(DWORD Signature) const;

    // Set machine
    void SetMachine(WORD Machine) const;

    // Set number of sections
    void SetNumberOfSections(WORD NumberOfSections) const;

    // Set time date stamp
    void SetTimeDateStamp(DWORD TimeDateStamp) const;

    // Set pointer to symbol table
    void SetPointerToSymbolTable(DWORD PointerToSymbolTable) const;

    // Set number of symbols
    void SetNumberOfSymbols(DWORD NumberOfSymbols) const;

    // Set size of optional header
    void SetSizeOfOptionalHeader(WORD SizeOfOptionalHeader) const;

    // Set characteristics
    void SetCharacteristics(WORD Characteristics) const;

    // Set magic
    void SetMagic(WORD Magic) const;

    // Set major linker version
    void SetMajorLinkerVersion(BYTE MajorLinkerVersion) const;

    // Set major linker version
    void SetMinorLinkerVersion(BYTE MinorLinkerVersion) const;

    // Set major linker version
    void SetSizeOfCode(DWORD SizeOfCode) const;

    // Set major linker version
    void SetSizeOfInitializedData(DWORD SizeOfInitializedData) const;

    // Set major linker version
    void SetSizeOfUninitializedData(DWORD SizeOfUninitializedData) const;

    // Set major linker version
    void SetAddressOfEntryPoint(DWORD AddressOfEntryPoint) const;

    // Set base of code
    void SetBaseOfCode(DWORD BaseOfCode) const;

#if defined(_M_IX86) 
    // Set base of data
    void SetBaseOfData(DWORD BaseOfData) const;
#endif

    // Set base of code
    void SetImageBase(ULONG_PTR ImageBase) const;

    // Set base of code
    void SetSectionAlignment(DWORD SectionAlignment) const;

    // Set base of code
    void SetFileAlignment(DWORD FileAlignment) const;

    // Set base of code
    void SetMajorOperatingSystemVersion(
      WORD MajorOperatingSystemVersion) const;

    // Set base of code
    void SetMinorOperatingSystemVersion(
      WORD MinorOperatingSystemVersion) const;

    // Set base of code
    void SetMajorImageVersion(WORD MajorImageVersion) const;

    // Set base of code
    void SetMinorImageVersion(WORD MinorImageVersion) const;

    // Set base of code
    void SetMajorSubsystemVersion(WORD MajorSubsystemVersion) const;

    // Set base of code
    void SetMinorSubsystemVersion(WORD MinorSubsystemVersion) const;

    // Set base of code
    void SetWin32VersionValue(DWORD Win32VersionValue) const;
    
    // Set size of image
    void SetSizeOfImage(DWORD SizeOfImage) const;

    // Set base of code
    void SetSizeOfHeaders(DWORD SizeOfHeaders) const;

    // Set base of code
    void SetCheckSum(DWORD CheckSum) const;

    // Set base of code
    void SetSubsystem(WORD Subsystem) const;

    // Set base of code
    void SetDllCharacteristics(WORD DllCharacteristics) const;

    // Set base of code
    void SetSizeOfStackReserve(ULONG_PTR SizeOfStackReserve) const;

    // Set base of code
    void SetSizeOfStackCommit(ULONG_PTR SizeOfStackCommit) const;

    // Set base of code
    void SetSizeOfHeapReserve(ULONG_PTR SizeOfHeapReserve) const;

    // Set base of code
    void SetSizeOfHeapCommit(ULONG_PTR SizeOfHeapCommit) const;

    // Set base of code
    void SetLoaderFlags(DWORD LoaderFlags) const;

    // Set base of code
    void SetNumberOfRvaAndSizes(DWORD NumberOfRvaAndSizes) const;

    // Set base of code
    void SetDataDirectoryVirtualAddress(DataDir MyDataDir, 
      DWORD DataDirectoryVirtualAddress) const;

    // Set base of code
    void SetDataDirectorySize(DataDir MyDataDir, 
      DWORD DataDirectorySize) const;

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;

    // Base address
    PBYTE m_pBase;
  };
}
