/*
This file is part of HadesMem.
Copyright (C) 2011 Joshua Boyce (a.k.a. RaptorFactor).
<http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

HadesMem is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

HadesMem is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with HadesMem.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <HadesMemory/FindPattern.hpp>
#include <HadesMemory/Module.hpp>
#include <HadesMemory/Detail/I18n.hpp>
#include <HadesMemory/PeLib/PeFile.hpp>
#include <HadesMemory/Detail/Config.hpp>
#include <HadesMemory/PeLib/Section.hpp>
#include <HadesMemory/PeLib/DosHeader.hpp>
#include <HadesMemory/PeLib/NtHeaders.hpp>

#include <limits>
#include <vector>
#include <string>
#include <locale>
#include <iterator>
#include <algorithm>

#ifdef HADES_MSVC
#pragma warning(push, 1)
#endif
#include <boost/assert.hpp>
#include <boost/variant.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#ifdef HADES_MSVC
#pragma warning(pop)
#endif

namespace HadesMem
{
  // Pattern info for parser
  struct PatternInfo
  {
    std::wstring Name;
    std::wstring Data;
  };
  
  // Pattern manipulator info for parser
  struct ManipInfo
  {
    enum Manipulator
    {
      Manip_Add, 
      Manip_Sub, 
      Manip_Rel, 
      Manip_Lea
    };
    
    Manipulator Type;
    std::vector<unsigned> Operands;
  };
  
  // Full pattern info for parser (both pattern and manipulator info)
  struct PatternInfoFull
  {
    PatternInfo Pattern;
    std::vector<ManipInfo> Manipulators;
  };
}

// Adapt pattern info struct for parser
BOOST_FUSION_ADAPT_STRUCT(HadesMem::PatternInfo, 
  (std::wstring, Name)
  (std::wstring, Data))

// Adapt pattern manipulator info struct for parser
BOOST_FUSION_ADAPT_STRUCT(HadesMem::ManipInfo, 
  (HadesMem::ManipInfo::Manipulator, Type)
  (std::vector<unsigned>, Operands))

// Adapt full pattern info struct for parser
BOOST_FUSION_ADAPT_STRUCT(HadesMem::PatternInfoFull, 
  (HadesMem::PatternInfo, Pattern)
  (std::vector<HadesMem::ManipInfo>, Manipulators))

namespace HadesMem
{
  // Constructor
  FindPattern::FindPattern(MemoryMgr const& MyMemory, HMODULE Module) 
    : m_Memory(MyMemory), 
    m_Base(0), 
    m_CodeRegions(), 
    m_DataRegions(), 
    m_Addresses()
  {
    if (!Module)
    {
      ModuleList Modules(m_Memory);
      Module = std::begin(Modules)->GetHandle();
    }
    
    PBYTE const pBase = reinterpret_cast<PBYTE>(Module);
    m_Base = reinterpret_cast<DWORD_PTR>(pBase);
    PeFile const MyPeFile(m_Memory, pBase);
    DosHeader const MyDosHeader(MyPeFile);
    NtHeaders const MyNtHeaders(MyPeFile);
    
    SectionList Sections(MyPeFile);
    std::for_each(std::begin(Sections), std::end(Sections), 
      [&, this] (Section const& S)
      {
        if ((S.GetCharacteristics() & IMAGE_SCN_CNT_CODE) == 
          IMAGE_SCN_CNT_CODE)
        {
          PBYTE const SBegin = static_cast<PBYTE>(MyPeFile.RvaToVa(
            S.GetVirtualAddress()));
          if (SBegin == nullptr)
          {
            BOOST_THROW_EXCEPTION(FindPattern::Error() << 
              ErrorFunction("FindPattern::FindPattern") << 
              ErrorString("Could not get section base address."));
          }
          
          PBYTE const SEnd = SBegin + S.GetSizeOfRawData();
          
          m_CodeRegions.push_back(std::make_pair(SBegin, SEnd));
        }
        
        if ((S.GetCharacteristics() & IMAGE_SCN_CNT_INITIALIZED_DATA) == 
          IMAGE_SCN_CNT_INITIALIZED_DATA)
        {
          PBYTE SBegin = static_cast<PBYTE>(MyPeFile.RvaToVa(
            S.GetVirtualAddress()));
          if (SBegin == nullptr)
          {
            BOOST_THROW_EXCEPTION(FindPattern::Error() << 
              ErrorFunction("FindPattern::FindPattern") << 
              ErrorString("Could not get section base address."));
          }
          
          PBYTE SEnd = SBegin + S.GetSizeOfRawData();
          
          m_DataRegions.push_back(std::make_pair(SBegin, SEnd));
        }
      });
    
    if (m_CodeRegions.empty() && m_DataRegions.empty())
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("FindPattern::FindPattern") << 
        ErrorString("No valid sections to scan found."));
    }
  }
      
  // Copy constructor
  FindPattern::FindPattern(FindPattern const& Other)
    : m_Memory(Other.m_Memory), 
    m_Base(Other.m_Base), 
    m_CodeRegions(Other.m_CodeRegions), 
    m_DataRegions(Other.m_DataRegions), 
    m_Addresses(Other.m_Addresses)
  { }
  
  // Copy assignment operator
  FindPattern& FindPattern::operator=(FindPattern const& Other)
  {
    this->m_Memory = Other.m_Memory;
    this->m_Base = Other.m_Base;
    this->m_CodeRegions = Other.m_CodeRegions;
    this->m_DataRegions = Other.m_DataRegions;
    this->m_Addresses = Other.m_Addresses;
    
    return *this;
  }
  
  // Move constructor
  FindPattern::FindPattern(FindPattern&& Other)
    : m_Memory(std::move(Other.m_Memory)), 
    m_Base(Other.m_Base), 
    m_CodeRegions(std::move(Other.m_CodeRegions)), 
    m_DataRegions(std::move(Other.m_DataRegions)), 
    m_Addresses(std::move(Other.m_Addresses))
  { }
  
  // Move assignment operator
  FindPattern& FindPattern::operator=(FindPattern&& Other)
  {
    this->m_Memory = std::move(Other.m_Memory);
    this->m_Base = Other.m_Base;
    Other.m_Base = 0;
    this->m_CodeRegions = std::move(Other.m_CodeRegions);
    this->m_DataRegions = std::move(Other.m_DataRegions);
    this->m_Addresses = std::move(Other.m_Addresses);
    
    return *this;
  }
  
  // Destructor
  FindPattern::~FindPattern()
  { }
  
  // Find pattern
  PVOID FindPattern::Find(std::wstring const& Data, FindFlags Flags) const
  {
    namespace qi = boost::spirit::qi;
    
    typedef std::wstring::const_iterator DataIter;
    typedef qi::standard::space_type SkipWsT;
    
    boost::spirit::qi::rule<DataIter, unsigned int(), SkipWsT> DataRule;
    DataRule %= (qi::hex | 
      qi::lit(L"??")[qi::_val = static_cast<unsigned int>(-1)]);
    
    qi::rule<DataIter, std::vector<unsigned int>(), SkipWsT> DataListRule = 
      +(DataRule);
    
    std::vector<unsigned int> DataParsed;
    auto DataBeg = std::begin(Data);
    auto DataEnd = std::end(Data);
    bool const Converted = boost::spirit::qi::phrase_parse(
      DataBeg, DataEnd, 
      DataListRule, 
      boost::spirit::qi::space, 
      DataParsed);
    if (!Converted || DataBeg != DataEnd)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("FindPattern::Find") << 
        ErrorString("Data parsing failed."));
    }
    
    std::vector<std::pair<BYTE, bool>> DataReal;
    std::transform(std::begin(DataParsed), std::end(DataParsed), 
      std::back_inserter(DataReal), 
      [] (unsigned int Current) -> std::pair<BYTE, bool>
      {
        bool const IsWildcard = (Current == static_cast<unsigned int>(-1));
        
        BYTE CurrentByte = 0;
        if (!IsWildcard)
        {
          try
          {
            CurrentByte = boost::numeric_cast<BYTE>(Current);
          }
          catch (std::exception const& /*e*/)
          {
            BOOST_THROW_EXCEPTION(FindPattern::Error() << 
              ErrorFunction("FindPattern::Find") << 
              ErrorString("Data conversion failed (numeric)."));
          }
        }
        
        return std::make_pair(CurrentByte, !IsWildcard);
      });
    
    bool const ScanDataSecs = ((Flags & FindFlags_ScanData) == 
      FindFlags_ScanData);
    
    PVOID Address = Find(DataReal, ScanDataSecs);
    
    if (!Address && ((Flags & FindFlags_ThrowOnUnmatch) == 
      FindFlags_ThrowOnUnmatch))
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("FindPattern::Find") << 
        ErrorString("Could not match pattern."));
    }
    
    if (Address && ((Flags & FindFlags_RelativeAddress) == 
      FindFlags_RelativeAddress))
    {
      Address = static_cast<PBYTE>(Address) - m_Base;
    }
    
    return Address;
  }

  // Find pattern
  PVOID FindPattern::Find(std::wstring const& Data, 
    std::wstring const& Name, FindFlags Flags)
  {
    PVOID const Address = Find(Data, Flags);
    
    if (!Name.empty())
    {
      m_Addresses[Name] = Address;
    }
    
    return Address;
  }

  // Search memory
  PVOID FindPattern::Find(std::vector<std::pair<BYTE, bool>> const& Data, 
    bool ScanDataSecs) const
  {
    BOOST_ASSERT(!Data.empty());
    
    std::vector<std::pair<PBYTE, PBYTE>> const& ScanRegions = 
      ScanDataSecs ? m_DataRegions : m_CodeRegions;
    for (auto i = std::begin(ScanRegions); i != std::end(ScanRegions); ++i)
    {
      PBYTE const SBegin = i->first;
      PBYTE const SEnd = i->second;
      BOOST_ASSERT(SEnd > SBegin);
      
      std::size_t const MemSize = SEnd - SBegin;
      std::vector<BYTE> const Buffer(m_Memory.ReadList<std::vector<BYTE>>(
        SBegin, MemSize));
      
      auto const Iter = std::search(std::begin(Buffer), std::end(Buffer), 
        std::begin(Data), std::end(Data), 
        [] (BYTE HCur, std::pair<BYTE, bool> const& NCur)
        {
          return (!NCur.second) || (HCur == NCur.first);
        });
      
      if (Iter != Buffer.cend())
      {
        return (SBegin + std::distance(std::begin(Buffer), Iter));
      }
    }
    
    return nullptr;
  }

  // Get address map
  std::map<std::wstring, PVOID> FindPattern::GetAddresses() const
  {
    return m_Addresses;
  }

  // Operator[] overload to allow retrieving addresses by name
  PVOID FindPattern::operator[](std::wstring const& Name) const
  {
    auto const Iter = m_Addresses.find(Name);
    return Iter != m_Addresses.end() ? Iter->second : nullptr;
  }
    
  // Read patterns from file
  void FindPattern::LoadFile(std::wstring const& Path)
  {
    boost::filesystem::wifstream PatternFile(Path);
    if (!PatternFile)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("FindPattern::LoadFile") << 
        ErrorString("Could not open pattern file."));
    }
    
    std::istreambuf_iterator<wchar_t> const PatFileBeg(PatternFile);
    std::istreambuf_iterator<wchar_t> const PatFileEnd;
    std::vector<wchar_t> PatFileBuf(PatFileBeg, PatFileEnd);
    PatFileBuf.push_back(L'\0');
    
    LoadFileMemory(PatFileBuf.data());
  }
  
  // Read patterns from memory
  void FindPattern::LoadFileMemory(std::wstring const& Data)
  {
    namespace qi = boost::spirit::qi;
    
    typedef std::wstring::const_iterator DataIter;
    typedef qi::standard::space_type SkipWsT;
    
    struct FlagsParserT : qi::symbols<wchar_t, FindFlags> 
    {
      FlagsParserT()
      {
        add
          (L"None", FindFlags_None)
          (L"ThrowOnUnmatch", FindFlags_ThrowOnUnmatch)
          (L"RelativeAddress", FindFlags_RelativeAddress)
          (L"ScanData", FindFlags_ScanData);
      }
    } FlagsParser;
    
    qi::rule<DataIter, std::vector<FindFlags>(), SkipWsT> FlagsRule = 
      '(' >> *(FlagsParser % ',') >> ')';
    qi::rule<DataIter, std::wstring()> NameRule = 
      qi::lexeme[*(~qi::char_(','))] >> ',';    
    qi::rule<DataIter, std::wstring()> DataRule = 
      qi::lexeme[*(~qi::char_('}'))];    
    qi::rule<DataIter, PatternInfo(), SkipWsT> PatternRule = 
      '{' >> NameRule >> DataRule >> '}';
    
    struct ManipParserT : qi::symbols<wchar_t, ManipInfo::Manipulator> 
    {
      ManipParserT()
      {
        add
          (L"Add", ManipInfo::Manip_Add)
          (L"Sub", ManipInfo::Manip_Sub)
          (L"Rel", ManipInfo::Manip_Rel)
          (L"Lea", ManipInfo::Manip_Lea);
      }
    } ManipParser;
    
    qi::rule<DataIter, ManipInfo::Manipulator(), SkipWsT> ManipNameRule = 
      ManipParser >> ',';
    qi::rule<DataIter, std::vector<unsigned>(), SkipWsT> OperandRule = 
      *(qi::uint_ % ',');
    qi::rule<DataIter, ManipInfo(), SkipWsT> ManipRule = 
      '[' >> ManipNameRule >> OperandRule >> ']';
    qi::rule <DataIter, PatternInfoFull(), SkipWsT> PatternFullRule = 
      (PatternRule >> *ManipRule);
    
    std::vector<FindFlags> FlagsList;
    std::vector<PatternInfoFull> PatternList;
    
    auto DataBeg = std::begin(Data);
    auto DataEnd = std::end(Data);
    bool const Parsed = qi::phrase_parse(DataBeg, DataEnd, 
      (
        L"HadesMem Patterns" >> FlagsRule >> 
        *PatternFullRule
      ), 
      qi::space, 
      FlagsList, PatternList);
    
    if (!Parsed || DataBeg != DataEnd)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("FindPattern::LoadFileMemory") << 
        ErrorString("Parsing failed."));
    }
    
    unsigned int Flags = FindFlags_None;
    std::for_each(std::begin(FlagsList), std::end(FlagsList), 
      [&] (FindFlags Flag)
      {
        Flags |= Flag;
      });
    
    std::for_each(std::begin(PatternList), std::end(PatternList), 
      [&, this] (PatternInfoFull const& P)
      {
        PatternInfo const& PatInfo = P.Pattern;
        Pattern MyPattern(*this, PatInfo.Data, PatInfo.Name, 
          static_cast<FindFlags>(Flags));
        
        std::vector<ManipInfo> const& ManipList = P.Manipulators;
        std::for_each(std::begin(ManipList), std::end(ManipList), 
          [&] (ManipInfo const& M)
          {
            switch (M.Type)
            {
            case ManipInfo::Manip_Add:
              if (M.Operands.size() != 1)
              {
                BOOST_THROW_EXCEPTION(FindPattern::Error() << 
                  ErrorFunction("FindPattern::LoadFileMemory") << 
                  ErrorString("Invalid manipulator operands for 'Add'."));
              }
              
              MyPattern << PatternManipulators::Add(M.Operands[0]);
                
              break;
              
            case ManipInfo::Manip_Sub:
              if (M.Operands.size() != 1)
              {
                BOOST_THROW_EXCEPTION(FindPattern::Error() << 
                  ErrorFunction("FindPattern::LoadFileMemory") << 
                  ErrorString("Invalid manipulator operands for 'Sub'."));
              }
              
              MyPattern << PatternManipulators::Sub(M.Operands[0]);
              
              break;
            
            case ManipInfo::Manip_Rel:
              if (M.Operands.size() != 2)
              {
                BOOST_THROW_EXCEPTION(FindPattern::Error() << 
                  ErrorFunction("FindPattern::LoadFileMemory") << 
                  ErrorString("Invalid manipulator operands for 'Rel'."));
              }
              
              MyPattern << PatternManipulators::Rel(M.Operands[0], 
                M.Operands[1]);
              
              break;
            
            case ManipInfo::Manip_Lea:
              if (M.Operands.size() != 0)
              {
                BOOST_THROW_EXCEPTION(FindPattern::Error() << 
                  ErrorFunction("FindPattern::LoadFileMemory") << 
                  ErrorString("Invalid manipulator operands for 'Lea'."));
              }
              
              MyPattern << PatternManipulators::Lea();
              
              break;
            
            default:
              BOOST_THROW_EXCEPTION(FindPattern::Error() << 
                ErrorFunction("FindPattern::LoadFileMemory") << 
                ErrorString("Unknown manipulator."));
            }
          });
          
          MyPattern << PatternManipulators::Save();
      });
  }
    
  // Equality operator
  bool FindPattern::operator==(FindPattern const& Rhs) const
  {
    return this->m_Memory == Rhs.m_Memory && 
      this->m_Base == Rhs.m_Base && 
      this->m_Addresses == Rhs.m_Addresses;
  }
  
  // Inequality operator
  bool FindPattern::operator!=(FindPattern const& Rhs) const
  {
    return !(*this == Rhs);
  }
  
  // Constructor
  Pattern::Pattern(FindPattern& Finder, std::wstring const& Data, 
    std::wstring const& Name, FindPattern::FindFlags Flags)
    : m_Finder(Finder), 
    m_Name(Name), 
    m_Address(static_cast<PBYTE>(Finder.Find(Data, Flags))), 
    m_Flags(Flags)
  { }
  
  // Constructor
  Pattern::Pattern(FindPattern& Finder, std::wstring const& Data, 
    FindPattern::FindFlags Flags)
    : m_Finder(Finder), 
    m_Name(), 
    m_Address(static_cast<PBYTE>(Finder.Find(Data, Flags))), 
    m_Flags(Flags)
  { }
  
  // Move constructor
  Pattern::Pattern(Pattern&& Other) 
    : m_Finder(Other.m_Finder), 
    m_Name(std::move(Other.m_Name)), 
    m_Address(Other.m_Address), 
    m_Flags(Other.m_Flags)
  {
    m_Address = nullptr;
    m_Flags = FindPattern::FindFlags_None;
  }
  
  // Move assignment operator
  Pattern& Pattern::operator=(Pattern&& Other)
  {
    this->m_Finder = Other.m_Finder;
    
    this->m_Name = std::move(Other.m_Name);
      
    this->m_Address = Other.m_Address;
    Other.m_Address = nullptr;
    
    this->m_Flags = Other.m_Flags;
    Other.m_Flags = FindPattern::FindFlags_None;
    
    return *this;
  }
  
  // Destructor
  Pattern::~Pattern()
  { }
  
  // Save back to parent
  void Pattern::Save()
  {
    if (m_Name.empty())
    {
      return;
    }
    
    m_Finder.m_Addresses[m_Name] = m_Address;
  }
  
  // Update address
  void Pattern::Update(PBYTE Address)
  {
    m_Address = Address;
  }
  
  // Get address
  PBYTE Pattern::GetAddress() const
  {
    return m_Address;
  }
  
  // Get memory manager
  MemoryMgr Pattern::GetMemory() const
  {
    return m_Finder.m_Memory;
  }
  
  // Get find flags
  FindPattern::FindFlags Pattern::GetFlags() const
  {
    return m_Flags;
  }
  
  // Get base
  DWORD_PTR Pattern::GetBase() const
  {
    return m_Finder.m_Base;
  }
  
  namespace PatternManipulators
  {
    // Manipulate pattern
    void Manipulator::Manipulate(Pattern& /*Pat*/) const
    { }

    // Manipulator chaining operator overload
    Pattern& operator<< (Pattern& Pat, Manipulator const& Manip)
    {
      Manip.Manipulate(Pat);
      return Pat;
    }
    
    // Manipulate pattern
    void Save::Manipulate(Pattern& Pat) const
    {
      Pat.Save();
    }
    
    // Constructor
    Add::Add(DWORD_PTR Offset)
      : m_Offset(Offset)
    { }
    
    // Manipulate pattern
    void Add::Manipulate(Pattern& Pat) const
    {
      PBYTE Address = Pat.GetAddress();
      if (Address)
      {
        Pat.Update(Address + m_Offset);
      }
    }
    
    // Constructor
    Sub::Sub(DWORD_PTR Offset)
      : m_Offset(Offset)
    { }
    
    // Manipulate pattern
    void Sub::Manipulate(Pattern& Pat) const
    {
      PBYTE Address = Pat.GetAddress();
      if (Address)
      {
        Pat.Update(Address - m_Offset);
      }
    }
    
    // Manipulate pattern
    void Lea::Manipulate(Pattern& Pat) const
    {
      PBYTE Address = Pat.GetAddress();
      if (Address)
      {
        try
        {
          DWORD_PTR Base = ((Pat.GetFlags() & 
            FindPattern::FindFlags_RelativeAddress) == 
            FindPattern::FindFlags_RelativeAddress) ? Pat.GetBase() : 0;
          Address = Pat.GetMemory().Read<PBYTE>(Pat.GetAddress() + Base);
          Pat.Update(Address - Base);
        }
        catch (std::exception const& /*e*/)
        {
          Pat.Update(nullptr);
        }
      }
    }
    
    // Constructor
    Rel::Rel(DWORD_PTR Size, DWORD_PTR Offset)
      : m_Size(Size), 
      m_Offset(Offset)
    { }
    
    // Manipulate pattern
    void Rel::Manipulate(Pattern& Pat) const
    {
      PBYTE Address = Pat.GetAddress();
      if (Address)
      {
        try
        {
          DWORD_PTR Base = ((Pat.GetFlags() & 
            FindPattern::FindFlags_RelativeAddress) == 
            FindPattern::FindFlags_RelativeAddress) ? Pat.GetBase() : 0;
          Address = Pat.GetMemory().Read<PBYTE>(Address + Base) + 
            reinterpret_cast<DWORD_PTR>(Address + Base) + m_Size - m_Offset;
          Pat.Update(Address - Base);
        }
        catch (std::exception const& /*e*/)
        {
          Pat.Update(nullptr);
        }
      }
    }
  }
}
