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

#pragma once

#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Error.hpp>

#include <memory>

#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <Windows.h>

namespace HadesMem
{
  // Memory region managing class
  class Region
  {
  public:
    // Region exception type
    class Error : public virtual HadesMemError 
    { };

    // Constructor
    Region(MemoryMgr const& MyMemory, PVOID Address);

    // Constructor
    Region(MemoryMgr const& MyMemory, MEMORY_BASIC_INFORMATION const& MyMbi);
    
    // Copy constructor
    Region(Region const& Other);
    
    // Copy assignment operator
    Region& operator=(Region const& Other);
    
    // Move constructor
    Region(Region&& Other);
    
    // Move assignment operator
    Region& operator=(Region&& Other);
    
    // Destructor
    ~Region();
    
    // Equality operator
    bool operator==(Region const& Rhs) const;
    
    // Inequality operator
    bool operator!=(Region const& Rhs) const;

    // Get base address
    PVOID GetBase() const;
    
    // Get allocation base
    PVOID GetAllocBase() const;
    
    // Get allocation protection
    DWORD GetAllocProtect() const;
    
    // Get size
    SIZE_T GetSize() const;
    
    // Get state
    DWORD GetState() const;
    
    // Get protection
    DWORD GetProtect() const;
    
    // Get type
    DWORD GetType() const;
    
    // Set protection
    DWORD SetProtect(DWORD Protect) const;
    
    // Dump to file
    void Dump(std::wstring const& Path) const;

  private:
    // MemoryMgr instance
    MemoryMgr m_Memory;

    // Region information
    MEMORY_BASIC_INFORMATION m_RegionInfo;
  };
  
  // Forward declaration of RegionIter
  template <typename RegionT>
  class RegionIter;
  
  // Region enumeration class
  class RegionList
  {
  public:
    // Region list error class
    class Error : public virtual HadesMemError
    { };
      
    // Region list iterator types
    typedef RegionIter<Region> iterator;
    typedef RegionIter<Region const> const_iterator;
        
    // Constructor
    RegionList(MemoryMgr const& MyMemory);
    
    // Move constructor
    RegionList(RegionList&& Other);
    
    // Move assignment operator
    RegionList& operator=(RegionList&& Other);
    
    // Get start of region list
    iterator begin();
    
    // Get end of region list
    iterator end();
    
    // Get start of region list
    const_iterator begin() const;
     
    // Get end of region list
    const_iterator end() const;
    
    // Get start of region list
    const_iterator cbegin() const;
     
    // Get end of region list
    const_iterator cend() const;
    
  protected:
    // Disable copying and copy-assignment
    RegionList(RegionList const& Other);
    RegionList& operator=(RegionList const& Other);
    
  private:
    // Give RegionIter access to internals
    template <typename> friend class RegionIter;
    
    // Memory instance
    MemoryMgr m_Memory;
  };
  
  // Region iterator
  template <typename RegionT>
  class RegionIter : public boost::iterator_facade<RegionIter<RegionT>, 
    RegionT, boost::forward_traversal_tag>
  {
  public:
    // Region iterator error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    RegionIter() 
      : m_pParent(nullptr), 
      m_Memory(), 
      m_Address(reinterpret_cast<PVOID>(-1)), 
      m_Region(), 
      m_RegionSize(0)
    { }
    
    // Constructor
    RegionIter(RegionList const& Parent) 
      : m_pParent(&Parent), 
      m_Memory(Parent.m_Memory), 
      m_Address(nullptr), 
      m_Region(), 
      m_RegionSize()
    {
      // Get region info
      MEMORY_BASIC_INFORMATION MyMbi;
      ZeroMemory(&MyMbi, sizeof(MyMbi));
      if (VirtualQueryEx(m_Memory->GetProcessHandle(), m_Address, &MyMbi, 
        sizeof(MyMbi)))
      {
        m_Address = MyMbi.BaseAddress;
        m_RegionSize = MyMbi.RegionSize;

        m_Region = Region(*m_Memory, MyMbi);
      }
      else
      {
        m_pParent = nullptr;
        m_Memory = boost::optional<MemoryMgr>();
        m_Address = reinterpret_cast<PVOID>(-1);
        m_Region = boost::optional<Region>();
        m_RegionSize = 0;
      }
    }
    
    // Copy constructor
    RegionIter(RegionIter const& Rhs) 
      : m_pParent(Rhs.m_pParent), 
      m_Memory(Rhs.m_Memory), 
      m_Address(Rhs.m_Address), 
      m_Region(Rhs.m_Region), 
      m_RegionSize(Rhs.m_RegionSize)
    { }
    
    // Assignment operator
    RegionIter& operator=(RegionIter const& Rhs) 
    {
      m_pParent = Rhs.m_pParent;
      m_Memory = Rhs.m_Memory;
      m_Address = Rhs.m_Address;
      m_Region = Rhs.m_Region;
      m_RegionSize = Rhs.m_RegionSize;
      
      return *this;
    }

  private:
    // Give Boost.Iterator access to internals
    friend class boost::iterator_core_access;

    // Increment iterator
    void increment() 
    {
      // Advance to next region
      m_Address = static_cast<PBYTE>(m_Address) + m_RegionSize;

      // Get region info
      MEMORY_BASIC_INFORMATION MyMbi;
      ZeroMemory(&MyMbi, sizeof(MyMbi));
      if (VirtualQueryEx(m_Memory->GetProcessHandle(), m_Address, &MyMbi, 
        sizeof(MyMbi)))
      {
        m_Address = MyMbi.BaseAddress;
        m_RegionSize = MyMbi.RegionSize;

        m_Region = Region(*m_Memory, MyMbi);
      }
      else
      {
        m_pParent = nullptr;
        m_Memory = boost::optional<MemoryMgr>();
        m_Address = reinterpret_cast<PVOID>(-1);
        m_Region = boost::optional<Region>();
        m_RegionSize = 0;
      }
    }
    
    // Check iterator for equality
    bool equal(RegionIter const& Rhs) const
    {
      return this->m_pParent == Rhs.m_pParent && 
        this->m_Address == Rhs.m_Address;
    }

    // Dereference iterator
    RegionT& dereference() const 
    {
      return *m_Region;
    }

    // Parent
    class RegionList const* m_pParent;
    // Memory instance
    boost::optional<MemoryMgr> m_Memory;
    // Region address
    PVOID m_Address;
    // Region object
    mutable boost::optional<Region> m_Region;
    // Region size
    SIZE_T m_RegionSize;
  };
}
