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

#include <HadesMemory/Region.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace HadesMem
{
  // Constructor
  Region::Region(MemoryMgr const& MyMemory, PVOID Address) 
    : m_Memory(MyMemory), 
    m_RegionInfo() 
  {
    ZeroMemory(&m_RegionInfo, sizeof(m_RegionInfo));
    
    if (!VirtualQueryEx(m_Memory.GetProcessHandle(), Address, 
      &m_RegionInfo, sizeof(m_RegionInfo)))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Region::Region") << 
        ErrorString("Could not query memory region.") << 
        ErrorCodeWinLast(LastError));
    }
  }

  // Constructor
  Region::Region(MemoryMgr const& MyMemory, 
    MEMORY_BASIC_INFORMATION const& MyMbi) 
    : m_Memory(MyMemory), 
    m_RegionInfo(MyMbi)
  { }
    
  // Copy constructor
  Region::Region(Region const& Other) 
    : m_Memory(Other.m_Memory), 
    m_RegionInfo(Other.m_RegionInfo)
  { }
  
  // Copy assignment operator
  Region& Region::operator=(Region const& Other)
  {
    this->m_Memory = Other.m_Memory;
    this->m_RegionInfo = Other.m_RegionInfo;
    
    return *this;
  }
  
  // Move constructor
  Region::Region(Region&& Other) 
    : m_Memory(std::move(Other.m_Memory)), 
    m_RegionInfo(Other.m_RegionInfo)
  {
    ZeroMemory(&Other.m_RegionInfo, sizeof(Other.m_RegionInfo));
  }
  
  // Move assignment operator
  Region& Region::operator=(Region&& Other)
  {
    this->m_Memory = std::move(Other.m_Memory);
    
    this->m_RegionInfo = Other.m_RegionInfo;
    ZeroMemory(&Other.m_RegionInfo, sizeof(Other.m_RegionInfo));
    
    return *this;
  }
  
  // Destructor
  Region::~Region()
  { }

  // Get base address
  PVOID Region::GetBase() const
  {
    return m_RegionInfo.BaseAddress;
  }

  // Get allocation base
  PVOID Region::GetAllocBase() const
  {
    return m_RegionInfo.AllocationBase;
  }

  // Get allocation protection
  DWORD Region::GetAllocProtect() const
  {
    return m_RegionInfo.AllocationProtect;
  }

  // Get size
  SIZE_T Region::GetSize() const
  {
    return m_RegionInfo.RegionSize;
  }

  // Get state
  DWORD Region::GetState() const
  {
    return m_RegionInfo.State;
  }

  // Get protection
  DWORD Region::GetProtect() const
  {
    return m_RegionInfo.Protect;
  }

  // Get type
  DWORD Region::GetType() const
  {
    return m_RegionInfo.Type;
  }
  
  // Dump to file
  void Region::Dump(std::wstring const& Path) const
  {
    boost::filesystem::ofstream Out(Path, std::ios::binary | std::ios::trunc);
    if (!Out)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Region::Dump") << 
        ErrorString("Could not open dump file."));
    }
    
    auto RegionBuf(m_Memory.ReadList<std::vector<char>>(GetBase(), GetSize()));
    Out.write(RegionBuf.data(), RegionBuf.size());
    if (!Out)
    {
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Region::Dump") << 
        ErrorString("Error writing to dump file."));
    }
  }
  
  // Set protection
  DWORD Region::SetProtect(DWORD Protect) const
  {
    DWORD OldProtect = 0;
    if (!VirtualProtectEx(m_Memory.GetProcessHandle(), GetBase(), 
      GetSize(), Protect, &OldProtect))
    {
      DWORD const LastError = GetLastError();
      BOOST_THROW_EXCEPTION(Error() << 
        ErrorFunction("Region::SetProtect") << 
        ErrorString("Could not change region memory protection.") << 
        ErrorCodeWinLast(LastError));
    }
    
    return OldProtect;
  }
    
  // Equality operator
  bool Region::operator==(Region const& Rhs) const
  {
    return this->m_Memory == Rhs.m_Memory && this->GetBase() == Rhs.GetBase();
  }
  
  // Inequality operator
  bool Region::operator!=(Region const& Rhs) const
  {
    return !(*this == Rhs);
  }

  // Constructor
  RegionList::RegionList(MemoryMgr const& MyMemory)
    : m_Memory(MyMemory)
  { }
  
  // Move constructor
  RegionList::RegionList(RegionList&& Other)
    : m_Memory(std::move(Other.m_Memory))
  { }
  
  // Move assignment operator
  RegionList& RegionList::operator=(RegionList&& Other)
  {
    m_Memory = std::move(Other.m_Memory);
      
    return *this;
  }
  
  // Get start of region list
  RegionList::iterator RegionList::begin()
  {
    return iterator(*this);
  }
  
  // Get end of region list
  RegionList::iterator RegionList::end()
  {
    return iterator();
  }
  
  // Get start of region list
  RegionList::const_iterator RegionList::begin() const
  {
    return const_iterator(*this);
  }
   
  // Get end of region list
  RegionList::const_iterator RegionList::end() const
  {
    return const_iterator();
  }
  
  // Get start of region list
  RegionList::const_iterator RegionList::cbegin() const
  {
    return const_iterator(*this);
  }
   
  // Get end of region list
  RegionList::const_iterator RegionList::cend() const
  {
    return const_iterator();
  }
}
