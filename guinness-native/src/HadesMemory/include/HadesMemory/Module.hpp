// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Error.hpp>
#include <HadesMemory/Detail/EnsureCleanup.hpp>

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <Windows.h>
#include <TlHelp32.h>

namespace HadesMem
{
  // Module managing class
  class Module
  {
  public:
    // Module exception type
    class Error : public virtual HadesMemError
    { };

    // Find module by handle
    Module(MemoryMgr const& MyMemory, HMODULE Handle);

    // Find module by name
    Module(MemoryMgr const& MyMemory, std::wstring const& ModuleName);

    // Create module
    Module(MemoryMgr const& MyMemory, MODULEENTRY32 const& ModuleEntry);
      
    // Copy constructor
    Module(Module const& Other);
    
    // Copy assignment operator
    Module& operator=(Module const& Other);
    
    // Move constructor
    Module(Module&& Other);
    
    // Move assignment operator
    Module& operator=(Module&& Other);
    
    // Destructor
    ~Module();
    
    // Equality operator
    bool operator==(Module const& Rhs) const;
    
    // Inequality operator
    bool operator!=(Module const& Rhs) const;

    // Get module handle
    HMODULE GetHandle() const;
    
    // Get module size
    DWORD GetSize() const;

    // Get module name
    std::wstring GetName() const;
    
    // Get module path
    std::wstring GetPath() const;
      
    // Find procedure by name
    FARPROC FindProcedure(std::string const& Name) const;
    
    // Find procedure by ordinal
    FARPROC FindProcedure(WORD Ordinal) const;

  private:
    // Memory instance
    MemoryMgr m_Memory;

    // Module base address
    HMODULE m_Base;
    // Module size
    DWORD m_Size;
    // Module name
    std::wstring m_Name;
    // Module path
    std::wstring m_Path;
  };
  
  // Get remote module handle
  Module GetRemoteModule(MemoryMgr const& MyMemory, LPCWSTR ModuleName);
    
  // Forward declaration of ModuleIter
  template <typename ModuleT>
  class ModuleIter;
  
  // Module enumeration class
  class ModuleList
  {
  public:
    // ModuleList exception type
    class Error : public virtual HadesMemError
    { };
    
    // Module list iterator types
    typedef ModuleIter<Module> iterator;
    typedef ModuleIter<Module const> const_iterator;
    
    // Constructor
    explicit ModuleList(MemoryMgr const& MyMemory);
    
    // Move constructor
    ModuleList(ModuleList&& Other);
    
    // Move assignment operator
    ModuleList& operator=(ModuleList&& Other);
    
    // Get start of module list
    iterator begin();
    
    // Get end of module list
    iterator end();
    
    // Get start of module list
    const_iterator begin() const;
     
    // Get end of module list
    const_iterator end() const;
    
    // Get start of module list
    const_iterator cbegin() const;
     
    // Get end of module list
    const_iterator cend() const;
    
  protected:
    // Disable copying and copy-assignment
    ModuleList(ModuleList const& Other);
    ModuleList& operator=(ModuleList const& Other);
    
  private:
    // Give ModuleIter access to internals
    template <typename> friend class ModuleIter;
    
    // Get module from cache by number
    boost::optional<Module&> GetByNum(DWORD Num) const;
    
    // Memory instance
    MemoryMgr m_Memory;
    
    // Snapshot handle
    mutable Detail::EnsureCloseSnap m_Snap;
    
    // Module cache
    mutable std::vector<Module> m_Cache;
  };

  // Module iterator
  template <typename ModuleT>
  class ModuleIter : public boost::iterator_facade<ModuleIter<ModuleT>, 
    ModuleT, boost::forward_traversal_tag>
  {
  public:
    // Module iterator error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    ModuleIter()
      : m_pParent(nullptr), 
      m_Number(static_cast<DWORD>(-1)), 
      m_Current()
    { }
    
    // Constructor
    ModuleIter(class ModuleList const& Parent)
      : m_pParent(&Parent), 
      m_Number(0), 
      m_Current()
    {
      boost::optional<Module&> Temp = m_pParent->GetByNum(m_Number);
      if (Temp)
      {
        m_Current = *Temp;
      }
      else
      {
        m_pParent = nullptr;
        m_Number = static_cast<DWORD>(-1);
      }
    }
    
  private:
    // Give Boost.Iterator access to internals
    friend class boost::iterator_core_access;

    // Increment iterator
    void increment()
    {
      boost::optional<Module&> Temp = m_pParent->GetByNum(++m_Number);
      m_Current = Temp ? *Temp : boost::optional<Module>();
      if (!Temp)
      {
        m_pParent = nullptr;
        m_Number = static_cast<DWORD>(-1);
      }
    }
    
    // Check iterator for equality
    bool equal(ModuleIter const& Rhs) const
    {
      return this->m_pParent == Rhs.m_pParent && 
        this->m_Number == Rhs.m_Number;
    }

    // Dereference iterator
    ModuleT& dereference() const
    {
      return *m_Current;
    }

    // Parent list instance
    class ModuleList const* m_pParent;
    
    // Module number
    DWORD m_Number;
    
    // Current module instance
    mutable boost::optional<Module> m_Current;
  };
}
