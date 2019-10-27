// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#include <HadesMemory/PeLib/ExportDir.hpp>
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/PeLib/NtHeaders.hpp>
#include <HadesMemory/PeLib/DosHeader.hpp>

#include <vector>
#include <iterator>

#include <boost/lexical_cast.hpp>

namespace HadesMem
{
  // Constructor
  ExportDir::ExportDir(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Memory(m_PeFile.GetMemoryMgr()), 
    m_pBase(nullptr)
  { }
      
  // Copy constructor
  ExportDir::ExportDir(ExportDir const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_pBase(Other.m_pBase)
  { }
  
  // Copy assignment operator
  ExportDir& ExportDir::operator=(ExportDir const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_pBase = Other.m_pBase;
    
    return *this;
  }
  
  // Move constructor
  ExportDir::ExportDir(ExportDir&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_pBase(Other.m_pBase)
  {
    Other.m_pBase = nullptr;
  }
  
  // Move assignment operator
  ExportDir& ExportDir::operator=(ExportDir&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_pBase = Other.m_pBase;
    Other.m_pBase = nullptr;
    
    return *this;
  }
  
  // Destructor
  ExportDir::~ExportDir()
  { }

  // Get base of export dir
  PVOID ExportDir::GetBase() const
  {
    if (!m_pBase)
    {
      NtHeaders const MyNtHeaders(m_PeFile);
      
      DWORD const DataDirSize = MyNtHeaders.GetDataDirectorySize(NtHeaders::
        DataDir_Export);
      DWORD const DataDirVa = MyNtHeaders.GetDataDirectoryVirtualAddress(
        NtHeaders::DataDir_Export);
      if (!DataDirSize || !DataDirVa)
      {
        BOOST_THROW_EXCEPTION(Error() << 
          ErrorFunction("ExportDir::GetBase") << 
          ErrorString("PE file has no export directory."));
      }
      
      m_pBase = static_cast<PBYTE>(m_PeFile.RvaToVa(DataDirVa));
    }
    
    return m_pBase;
  }

  // Whether export directory is valid
  bool ExportDir::IsValid() const
  {
    NtHeaders const MyNtHeaders(m_PeFile);
    
    DWORD const DataDirSize(MyNtHeaders.GetDataDirectorySize(NtHeaders::
      DataDir_Export));
    DWORD const DataDirVa(MyNtHeaders.GetDataDirectoryVirtualAddress(
      NtHeaders::DataDir_Export));
    
    return DataDirSize && DataDirVa;
  }

  // Ensure export directory is valid
  void ExportDir::EnsureValid() const
  {
    if (!IsValid())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ExportDir::EnsureValid") << 
        ErrorString("Export directory is invalid."));
    }
  }

  // Get characteristics
  DWORD ExportDir::GetCharacteristics() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, Characteristics));
  }

  // Get time date stamp
  DWORD ExportDir::GetTimeDateStamp() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, TimeDateStamp));
  }

  // Get major version
  WORD ExportDir::GetMajorVersion() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<WORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, MajorVersion));
  }

  // Get minor version
  WORD ExportDir::GetMinorVersion() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<WORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, MinorVersion));
  }

  // Get module name
  std::string ExportDir::GetName() const
  {
    PBYTE const pExpDirBase = static_cast<PBYTE>(GetBase());
    
    DWORD const NameRva = m_Memory.Read<DWORD>(pExpDirBase + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, Name));
    
    if (!NameRva)
    {
      return std::string();
    }
    
    return m_Memory.ReadString<std::string>(m_PeFile.RvaToVa(NameRva));
  }

  // Get ordinal base
  DWORD ExportDir::GetOrdinalBase() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, Base));
  }

  // Get number of functions
  DWORD ExportDir::GetNumberOfFunctions() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, NumberOfFunctions));
  }

  // Get number of names
  DWORD ExportDir::GetNumberOfNames() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, NumberOfNames));
  }

  // Get address of functions
  DWORD ExportDir::GetAddressOfFunctions() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, AddressOfFunctions));
  }

  // Get address of names
  DWORD ExportDir::GetAddressOfNames() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, AddressOfNames));
  }

  // Get address of name ordinals
  DWORD ExportDir::GetAddressOfNameOrdinals() const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    return m_Memory.Read<DWORD>(pExportDir + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, AddressOfNameOrdinals));
  }

  // Set characteristics
  void ExportDir::SetCharacteristics(DWORD Characteristics) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      Characteristics), Characteristics);
  }

  // Set time date stamp
  void ExportDir::SetTimeDateStamp(DWORD TimeDateStamp) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      TimeDateStamp), TimeDateStamp);
  }

  // Set major version
  void ExportDir::SetMajorVersion(WORD MajorVersion) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      MajorVersion), MajorVersion);
  }

  // Set major version
  void ExportDir::SetMinorVersion(WORD MinorVersion) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      MinorVersion), MinorVersion);
  }

  // Set name
  void ExportDir::SetName(std::string const& Name) const
  {
    PBYTE const pExpDirBase = static_cast<PBYTE>(GetBase());
    
    DWORD const NameRva = m_Memory.Read<DWORD>(pExpDirBase + FIELD_OFFSET(
      IMAGE_EXPORT_DIRECTORY, Name));
    
    if (!NameRva)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ExportDir::SetName") << 
        ErrorString("Export dir has no name. Cannot overwrite."));
    }
    
    std::string const CurrentName = m_Memory.ReadString<std::string>(
      m_PeFile.RvaToVa(NameRva));
    
    // FIXME: Support allocating space for a new name rather than just 
    // overwriting the existing one.
    if (Name.size() > CurrentName.size())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("ExportDir::SetName") << 
        ErrorString("Cannot overwrite name with longer string."));
    }
    
    m_Memory.WriteString(m_PeFile.RvaToVa(NameRva), Name);
  }

  // Set ordinal base
  void ExportDir::SetOrdinalBase(DWORD Base) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      Base), Base);
  }

  // Set number of functions
  void ExportDir::SetNumberOfFunctions(DWORD NumberOfFunctions) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      NumberOfFunctions), NumberOfFunctions);
  }

  // Set number of names
  void ExportDir::SetNumberOfNames(DWORD NumberOfNames) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      NumberOfNames), NumberOfNames);
  }

  // Set address of functions
  void ExportDir::SetAddressOfFunctions(DWORD AddressOfFunctions) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      AddressOfFunctions), AddressOfFunctions);
  }

  // Set address of names
  void ExportDir::SetAddressOfNames(DWORD AddressOfNames) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      AddressOfNames), AddressOfNames);
  }

  // Set address of name ordinals
  void ExportDir::SetAddressOfNameOrdinals(DWORD AddressOfNameOrdinals) const
  {
    PBYTE const pExportDir = static_cast<PBYTE>(GetBase());
    m_Memory.Write(pExportDir + FIELD_OFFSET(IMAGE_EXPORT_DIRECTORY, 
      AddressOfNameOrdinals), AddressOfNameOrdinals);
  }
  
  // Equality operator
  bool ExportDir::operator==(ExportDir const& Rhs) const
  {
    return m_pBase == Rhs.m_pBase && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool ExportDir::operator!=(ExportDir const& Rhs) const
  {
    return !(*this == Rhs);
  }

  // Constructor
  // FIXME: Refactor constructors to remove duplicated code.
  Export::Export(PeFile const& MyPeFile, DWORD Ordinal) 
    : m_PeFile(MyPeFile), 
    m_Memory(MyPeFile.GetMemoryMgr()), 
    m_Rva(0), 
    m_Va(nullptr), 
    m_Name(), 
    m_Forwarder(), 
    m_ForwarderSplit(), 
    m_Ordinal(0), 
    m_ByName(false), 
    m_Forwarded(false)
  {
    ExportDir const MyExportDir(m_PeFile);
    
    DWORD Offset = Ordinal - MyExportDir.GetOrdinalBase();
    
    if (Offset > MyExportDir.GetNumberOfFunctions())
    {
      BOOST_THROW_EXCEPTION(ExportDir::Error() << 
        ErrorFunction("Export::Export") << 
        ErrorString("Invalid export number."));
    }
    
    NtHeaders const MyNtHeaders(m_PeFile);
    
    DWORD const DataDirSize = MyNtHeaders.GetDataDirectorySize(NtHeaders::
      DataDir_Export);
    DWORD const DataDirVa = MyNtHeaders.GetDataDirectoryVirtualAddress(
      NtHeaders::DataDir_Export);
    
    DWORD const ExportDirStart = DataDirVa;
    DWORD const ExportDirEnd = ExportDirStart + DataDirSize;
    
    DWORD* pFunctions = static_cast<DWORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfFunctions()));
    
    // Find next exported entry
    for (; !m_Memory.Read<DWORD>(pFunctions + Offset) && 
      Offset <= MyExportDir.GetNumberOfFunctions(); ++Offset)
      ;
    
    if (Offset > MyExportDir.GetNumberOfFunctions())
    {
      BOOST_THROW_EXCEPTION(ExportDir::Error() << 
        ErrorFunction("Export::Export") << 
        ErrorString("Invalid export number."));
    }
    
    Ordinal = Offset + MyExportDir.GetOrdinalBase();
    m_Ordinal = static_cast<WORD>(Ordinal);

    WORD* pOrdinals = static_cast<WORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfNameOrdinals()));
    
    DWORD* pNames = static_cast<DWORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfNames()));
    
    if (DWORD const NumberOfNames = MyExportDir.GetNumberOfNames())
    {
      std::vector<WORD> NameOrdinals(m_Memory.ReadList<std::vector<WORD>>(
        pOrdinals, NumberOfNames));
      auto NameOrdIter = std::find(std::begin(NameOrdinals), 
        std::end(NameOrdinals), Offset);
      if (NameOrdIter != std::end(NameOrdinals))
      {
        m_ByName = true;
        DWORD const NameRva = m_Memory.Read<DWORD>(pNames + std::distance(
          std::begin(NameOrdinals), NameOrdIter));
        m_Name = m_Memory.ReadString<std::string>(m_PeFile.RvaToVa(NameRva));
      }
    }
    
    DWORD const FuncRva = m_Memory.Read<DWORD>(pFunctions + Offset);

    // Check function RVA. If it lies inside the export dir region 
    // then it's a forwarded export. Otherwise it's a regular RVA.
    if (FuncRva > ExportDirStart && FuncRva < ExportDirEnd)
    {
      m_Forwarded = true;
      m_Forwarder = m_Memory.ReadString<std::string>(m_PeFile.RvaToVa(FuncRva));
      
      std::string::size_type SplitPos = m_Forwarder.rfind('.');
      if (SplitPos != std::string::npos)
      {
        m_ForwarderSplit = std::make_pair(m_Forwarder.substr(0, SplitPos), 
          m_Forwarder.substr(SplitPos + 1));
      }
      else
      {
        BOOST_THROW_EXCEPTION(ExportDir::Error() << 
          ErrorFunction("Export::Export") << 
          ErrorString("Invalid forwarder string format."));
      }
    }
    else
    {
      m_Rva = FuncRva;
      m_Va = m_PeFile.RvaToVa(FuncRva);
    }
  }

  // Constructor
  Export::Export(PeFile const& MyPeFile, std::string const& Name) 
    : m_PeFile(MyPeFile), 
    m_Memory(MyPeFile.GetMemoryMgr()), 
    m_Rva(0), 
    m_Va(nullptr), 
    m_Name(), 
    m_Forwarder(), 
    m_ForwarderSplit(), 
    m_Ordinal(0), 
    m_ByName(false), 
    m_Forwarded(false)
  {
    ExportDir const MyExportDir(m_PeFile);
    
    if (!MyExportDir.GetNumberOfNames())
    {
      BOOST_THROW_EXCEPTION(ExportDir::Error() << 
        ErrorFunction("Export::Export") << 
        ErrorString("No named exports found."));
    }

    DWORD* pNames = static_cast<DWORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfNames()));
    
    WORD* pOrdinals = static_cast<WORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfNameOrdinals()));
    
    // Start of search region for lower bound search (binary search)
    DWORD* pFirst = static_cast<DWORD*>(MyPeFile.RvaToVa(
      MyExportDir.GetAddressOfNames()));
    
    // End of search region for lower bound search (binary search)
    DWORD* pLast = static_cast<DWORD*>(MyPeFile.RvaToVa(
      MyExportDir.GetAddressOfNames()));
    
    // Number of entries in export name array
    DWORD Count = MyExportDir.GetNumberOfNames();
    
    // Search step
    DWORD Step = 0;
    
    // Perform binary search of export dir for target
    while (Count > 0)
    {
      // Calculate current bounds
      pLast = pFirst;
      Step = Count / 2;
      pLast += Step;
      
      // Get current entry name
      DWORD const NameRva = m_Memory.Read<DWORD>(pLast);
      std::string const CurName = m_Memory.ReadString<std::string>(
        MyPeFile.RvaToVa(NameRva));
      
      // Perform lexical lower bound check on entry
      if (CurName < Name)
      {
        pFirst = ++pLast;
        Count -= Step + 1;
      }
      else
      {
        Count = Step;
      }
    }
    
    DWORD const NameRva = m_Memory.Read<DWORD>(pFirst);
    std::string const CurName = m_Memory.ReadString<std::string>(
      MyPeFile.RvaToVa(NameRva));
    m_ByName = true;
    m_Name = CurName;
    
    if (CurName != Name)
    {
      BOOST_THROW_EXCEPTION(ExportDir::Error() << 
        ErrorFunction("Export::Export") << 
        ErrorString("Could not find target."));
    }
    
    DWORD_PTR Index = (reinterpret_cast<DWORD_PTR>(pFirst) - 
      reinterpret_cast<DWORD_PTR>(pNames)) / sizeof(DWORD);
    
    WORD const NameOrdinal = m_Memory.Read<WORD>(pOrdinals + Index);
    m_Ordinal = NameOrdinal;
    
    if (m_Ordinal > MyExportDir.GetNumberOfFunctions())
    {
      BOOST_THROW_EXCEPTION(ExportDir::Error() << 
        ErrorFunction("Export::Export") << 
        ErrorString("Ordinal invalid."));
    }

    DWORD* pFunctions = static_cast<DWORD*>(m_PeFile.RvaToVa(MyExportDir.
      GetAddressOfFunctions()));
    
    DWORD const FuncRva = m_Memory.Read<DWORD>(pFunctions + m_Ordinal);
    
    NtHeaders const MyNtHeaders(m_PeFile);
    
    DWORD const DataDirSize = MyNtHeaders.GetDataDirectorySize(NtHeaders::
      DataDir_Export);
    DWORD const DataDirVa = MyNtHeaders.GetDataDirectoryVirtualAddress(
      NtHeaders::DataDir_Export);
    
    DWORD const ExportDirStart = DataDirVa;
    DWORD const ExportDirEnd = ExportDirStart + DataDirSize;
    
    // Check function RVA. If it lies inside the export dir region 
    // then it's a forwarded export. Otherwise it's a regular RVA.
    if (FuncRva > ExportDirStart && FuncRva < ExportDirEnd)
    {
      m_Forwarded = true;
      m_Forwarder = m_Memory.ReadString<std::string>(m_PeFile.RvaToVa(FuncRva));
      
      std::string::size_type SplitPos = m_Forwarder.rfind('.');
      if (SplitPos != std::string::npos)
      {
        m_ForwarderSplit = std::make_pair(m_Forwarder.substr(0, SplitPos), 
          m_Forwarder.substr(SplitPos + 1));
      }
      else
      {
        BOOST_THROW_EXCEPTION(ExportDir::Error() << 
          ErrorFunction("Export::Export") << 
          ErrorString("Invalid forwarder string format."));
      }
    }
    else
    {
      m_Rva = FuncRva;
      m_Va = m_PeFile.RvaToVa(FuncRva);
    }
  }
      
  // Copy constructor
  Export::Export(Export const& Other)
    : m_PeFile(Other.m_PeFile), 
    m_Memory(Other.m_Memory), 
    m_Rva(Other.m_Rva), 
    m_Va(Other.m_Va), 
    m_Name(Other.m_Name), 
    m_Forwarder(Other.m_Forwarder), 
    m_ForwarderSplit(Other.m_ForwarderSplit), 
    m_Ordinal(Other.m_Ordinal), 
    m_ByName(Other.m_ByName), 
    m_Forwarded(Other.m_Forwarded)
  { }
  
  // Copy assignment operator
  Export& Export::operator=(Export const& Other)
  {
    this->m_PeFile = Other.m_PeFile;
    this->m_Memory = Other.m_Memory;
    this->m_Rva = Other.m_Rva;
    this->m_Va = Other.m_Va;
    this->m_Name = Other.m_Name;
    this->m_Forwarder = Other.m_Forwarder;
    this->m_ForwarderSplit = Other.m_ForwarderSplit;
    this->m_Ordinal = Other.m_Ordinal;
    this->m_ByName = Other.m_ByName;
    this->m_Forwarded = Other.m_Forwarded;
    
    return *this;
  }
  
  // Move constructor
  Export::Export(Export&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Memory(std::move(Other.m_Memory)), 
    m_Rva(Other.m_Rva), 
    m_Va(Other.m_Va), 
    m_Name(std::move(Other.m_Name)), 
    m_Forwarder(std::move(Other.m_Forwarder)), 
    m_ForwarderSplit(std::move(Other.m_ForwarderSplit)), 
    m_Ordinal(Other.m_Ordinal), 
    m_ByName(Other.m_ByName), 
    m_Forwarded(Other.m_Forwarded)
  {
    Other.m_Rva = 0;
    Other.m_Va = nullptr;
    Other.m_Ordinal = 0;
    Other.m_ByName = false;
    Other.m_Forwarded = false;
  }
  
  // Move assignment operator
  Export& Export::operator=(Export&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_Rva = Other.m_Rva;
    Other.m_Rva = 0;
    
    this->m_Va = Other.m_Va;
    Other.m_Va = nullptr;
    
    this->m_Name = std::move(Other.m_Name);
    
    this->m_Forwarder = std::move(Other.m_Forwarder);
    
    this->m_ForwarderSplit = std::move(Other.m_ForwarderSplit);
    
    this->m_Ordinal = Other.m_Ordinal;
    Other.m_Ordinal = 0;
    
    this->m_ByName = Other.m_ByName;
    Other.m_ByName = false;
    
    this->m_Forwarded = Other.m_Forwarded;
    Other.m_Forwarded = false;
    
    return *this;
  }
  
  // Destructor
  Export::~Export()
  { }
  
  // Get RVA
  DWORD Export::GetRva() const
  {
    return m_Rva;
  }

  // Get VA
  PVOID Export::GetVa() const
  {
    return m_Va;
  }

  // Get name
  std::string Export::GetName() const
  {
    return m_Name;
  }

  // Get forwarder
  std::string Export::GetForwarder() const
  {
    return m_Forwarder;
  }
  
  // Get forwarder module name
  std::string Export::GetForwarderModule() const
  {
    return m_ForwarderSplit.first;
  }
  
  // Get forwarder function name
  std::string Export::GetForwarderFunction() const
  {
    return m_ForwarderSplit.second;
  }

  // Get ordinal
  WORD Export::GetOrdinal() const
  {
    return m_Ordinal;
  }

  // If entry is exported by name
  bool Export::ByName() const
  {
    return m_ByName;
  }

  // If entry is forwarded
  bool Export::Forwarded() const
  {
    return m_Forwarded;
  }
    
  // If entry is forwarded by ordinal
  bool Export::IsForwardedByOrdinal() const
  {
    return (GetForwarderFunction()[0] == '#');
  }
  
  // Get forwarder function ordinal
  WORD Export::GetForwarderOrdinal() const
  {
    if (IsForwardedByOrdinal())
    {
      WORD ForwarderOrdinal = 0;
      
      try
      {
        std::string const ForwarderFunction(GetForwarderFunction());
        ForwarderOrdinal = boost::lexical_cast<WORD>(ForwarderFunction.substr(1));
      }
      catch (std::exception const& /*e*/)
      {
        BOOST_THROW_EXCEPTION(ExportDir::Error() << 
          ErrorFunction("Export::GetForwarderOrdinal") << 
          ErrorString("Invalid forwarder ordinal detected."));
      }
      
      return ForwarderOrdinal;
    }
    
    BOOST_THROW_EXCEPTION(ExportDir::Error() << 
      ErrorFunction("Export::GetForwarderOrdinal") << 
      ErrorString("Function is not exported by ordinal."));
  }
  
  // Equality operator
  bool Export::operator==(Export const& Rhs) const
  {
    return m_Ordinal == Rhs.m_Ordinal && m_Memory == Rhs.m_Memory;
  }
  
  // Inequality operator
  bool Export::operator!=(Export const& Rhs) const
  {
    return !(*this == Rhs);
  }
  
  // Constructor
  ExportList::ExportList(PeFile const& MyPeFile)
    : m_PeFile(MyPeFile), 
    m_Cache()
  { }
    
  // Move constructor
  ExportList::ExportList(ExportList&& Other)
    : m_PeFile(std::move(Other.m_PeFile)), 
    m_Cache(std::move(Other.m_Cache))
  { }
  
  // Move assignment operator
  ExportList& ExportList::operator=(ExportList&& Other)
  {
    this->m_PeFile = std::move(Other.m_PeFile);
    this->m_Cache = std::move(Other.m_Cache);
    
    return *this;
  }
  
  // Get start of export list
  ExportList::iterator ExportList::begin()
  {
    return iterator(*this);
  }
  
  // Get end of export list
  ExportList::iterator ExportList::end()
  {
    return iterator();
  }
  
  // Get start of export list
  ExportList::const_iterator ExportList::begin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of export list
  ExportList::const_iterator ExportList::end() const
  {
    return const_iterator();
  }
  
  // Get start of export list
  ExportList::const_iterator ExportList::cbegin() const
  {
    return const_iterator(*this);
  }
  
  // Get end of export list
  ExportList::const_iterator ExportList::cend() const
  {
    return const_iterator();
  }
  
  // Get export from cache by number
  boost::optional<Export&> ExportList::GetByNum(DWORD Num) const
  {
    while (Num >= m_Cache.size())
    {
      ExportDir const MyExportDir(m_PeFile);
      if (!MyExportDir.IsValid() || !MyExportDir.GetNumberOfFunctions())
      {
        return boost::optional<Export&>();
      }
      else
      {
        DWORD const OrdinalBase = MyExportDir.GetOrdinalBase();
        DWORD const NextOrdinal = m_Cache.empty() ? OrdinalBase : 
          (m_Cache.back().GetOrdinal() + 1);
        if (NextOrdinal - OrdinalBase < MyExportDir.GetNumberOfFunctions())
        {
          m_Cache.push_back(Export(m_PeFile, NextOrdinal));
        }
        else
        {
          return boost::optional<Export&>();
        }
      }
    }
    
    return m_Cache[Num];
  }
}
