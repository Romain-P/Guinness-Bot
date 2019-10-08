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

#include <string>
#include <iterator>

#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <Windows.h>

namespace HadesMem
{
  // Import directory wrapper
  class ImportDir
  {
  public:
    // ImportDir error type
    class Error : public virtual HadesMemError
    { };

    // Constructor
    explicit ImportDir(PeFile const& MyPeFile, 
      PIMAGE_IMPORT_DESCRIPTOR pImpDesc = nullptr);
      
    // Copy constructor
    ImportDir(ImportDir const& Other);
    
    // Copy assignment operator
    ImportDir& operator=(ImportDir const& Other);
    
    // Move constructor
    ImportDir(ImportDir&& Other);
    
    // Move assignment operator
    ImportDir& operator=(ImportDir&& Other);
    
    // Destructor
    ~ImportDir();
    
    // Equality operator
    bool operator==(ImportDir const& Rhs) const;
    
    // Inequality operator
    bool operator!=(ImportDir const& Rhs) const;

    // Get import directory base
    PVOID GetBase() const;

    // Whether import directory is valid
    bool IsValid() const;

    // Ensure import directory is valid
    void EnsureValid() const;

    // Get characteristics
    DWORD GetCharacteristics() const;

    // Get time and date stamp
    DWORD GetTimeDateStamp() const;

    // Get forwarder chain
    DWORD GetForwarderChain() const;

    // Get name (raw)
    DWORD GetNameRaw() const;

    // Get name
    std::string GetName() const;

    // Get first thunk
    DWORD GetFirstThunk() const;

    // Set characteristics
    void SetCharacteristics(DWORD Characteristics) const;

    // Set time and date stamp
    void SetTimeDateStamp(DWORD TimeDateStamp) const;

    // Set forwarder chain
    void SetForwarderChain(DWORD ForwarderChain) const;

    // Set name (raw)
    void SetNameRaw(DWORD Name) const;
    
    // Set name
    void SetName(std::string const& Name) const;

    // Set first thunk
    void SetFirstThunk(DWORD FirstThunk) const;

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;

    // Import descriptor
    mutable PIMAGE_IMPORT_DESCRIPTOR m_pBase;
  };
    
  // Forward declaration of ImportDirIter
  template <typename ImportDirT>
  class ImportDirIter;
  
  // Import dir enumeration class
  class ImportDirList
  {
  public:
    // ImportDirList exception type
    class Error : public virtual HadesMemError
    { };
    
    // Import dir list iterator types
    typedef ImportDirIter<ImportDir> iterator;
    typedef ImportDirIter<ImportDir const> const_iterator;
    
    // Constructor
    explicit ImportDirList(PeFile const& MyPeFile);
    
    // Move constructor
    ImportDirList(ImportDirList&& Other);
    
    // Move assignment operator
    ImportDirList& operator=(ImportDirList&& Other);
    
    // Get start of import dir list
    iterator begin();
    
    // Get end of import dir list
    iterator end();
    
    // Get start of import dir list
    const_iterator begin() const;
     
    // Get end of import dir list
    const_iterator end() const;
    
    // Get start of import dir list
    const_iterator cbegin() const;
     
    // Get end of import dir list
    const_iterator cend() const;
    
  protected:
    // Disable copying and copy-assignment
    ImportDirList(ImportDirList const& Other);
    ImportDirList& operator=(ImportDirList const& Other);
    
  private:
    // Give ExportIter access to internals
    template <typename> friend class ImportDirIter;
    
    // Get import dir from cache by number
    boost::optional<ImportDir&> GetByNum(DWORD Num) const;
    
    // PeFile instance
    PeFile m_PeFile;
    
    // Import dir cache
    mutable std::vector<ImportDir> m_Cache;
  };

  // Import dir iterator
  template <typename ImportDirT>
  class ImportDirIter : public boost::iterator_facade<
    ImportDirIter<ImportDirT>, ImportDirT, boost::forward_traversal_tag>
  {
  public:
    // ImportDirIter error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    ImportDirIter()
      : m_pParent(nullptr), 
      m_Number(static_cast<DWORD>(-1)), 
      m_Current()
    { }
    
    // Constructor
    ImportDirIter(class ImportDirList const& Parent)
      : m_pParent(&Parent), 
      m_Number(0), 
      m_Current()
    {
      boost::optional<ImportDir&> Temp = m_pParent->GetByNum(m_Number);
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
      boost::optional<ImportDir&> Temp = m_pParent->GetByNum(++m_Number);
      m_Current = Temp ? *Temp : boost::optional<ImportDir>();
      if (!Temp)
      {
        m_pParent = nullptr;
        m_Number = static_cast<DWORD>(-1);
      }
    }
    
    // Check iterator for equality
    bool equal(ImportDirIter const& Rhs) const
    {
      return this->m_pParent == Rhs.m_pParent && 
        this->m_Number == Rhs.m_Number;
    }

    // Dereference iterator
    ImportDirT& dereference() const
    {
      return *m_Current;
    }

    // Parent list instance
    class ImportDirList const* m_pParent;
    
    // Import dir number
    DWORD m_Number;
    
    // Current import dir instance
    mutable boost::optional<ImportDir> m_Current;
  };
  
  // Import thunk wrapper
  class ImportThunk
  {
  public:
    // ImportThunk error type
    class Error : public virtual HadesMemError
    { };

    // Constructor
    ImportThunk(PeFile const& MyPeFile, PVOID pThunk);
      
    // Copy constructor
    ImportThunk(ImportThunk const& Other);
    
    // Copy assignment operator
    ImportThunk& operator=(ImportThunk const& Other);
    
    // Move constructor
    ImportThunk(ImportThunk&& Other);
    
    // Move assignment operator
    ImportThunk& operator=(ImportThunk&& Other);
    
    // Destructor
    ~ImportThunk();
    
    // Equality operator
    bool operator==(ImportThunk const& Rhs) const;
    
    // Inequality operator
    bool operator!=(ImportThunk const& Rhs) const;
    
    // Get base
    PVOID GetBase() const;

    // Whether thunk is valid
    bool IsValid() const;

    // Ensure thunk is valid
    void EnsureValid() const;

    // Get address of data
    DWORD_PTR GetAddressOfData() const;

    // Get ordinal (raw)
    DWORD_PTR GetOrdinalRaw() const;
    
    // Whether import is by ordinal
    bool ByOrdinal() const;

    // Get ordinal
    WORD GetOrdinal() const;

    // Get function
    DWORD_PTR GetFunction() const;

    // Get hint
    WORD GetHint() const;

    // Get name
    std::string GetName() const;

    // Set address of data
    void SetAddressOfData(DWORD_PTR AddressOfData) const;

    // Set ordinal (raw)
    void SetOrdinalRaw(DWORD_PTR OrdinalRaw) const;
      
    // Todo: SetOrdinal function

    // Set function
    void SetFunction(DWORD_PTR Function) const;

    // Set hint
    void SetHint(WORD Hint) const;
      
    // TODO: SetName function

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;

    // Base pointer
    mutable PBYTE m_pBase;
  };
    
  // Forward declaration of ImportThunkIter
  template <typename ImportThunkT>
  class ImportThunkIter;
  
  // Import thunk enumeration class
  class ImportThunkList
  {
  public:
    // ImportThunkList exception type
    class Error : public virtual HadesMemError
    { };
    
    // Import dir list iterator types
    typedef ImportThunkIter<ImportThunk> iterator;
    typedef ImportThunkIter<ImportThunk const> const_iterator;
    
    // Constructor
    explicit ImportThunkList(PeFile const& MyPeFile, DWORD FirstThunk);
    
    // Move constructor
    ImportThunkList(ImportThunkList&& Other);
    
    // Move assignment operator
    ImportThunkList& operator=(ImportThunkList&& Other);
    
    // Get start of import thunk list
    iterator begin();
    
    // Get end of import thunk list
    iterator end();
    
    // Get start of import thunk list
    const_iterator begin() const;
     
    // Get end of import thunk list
    const_iterator end() const;
    
    // Get start of import thunk list
    const_iterator cbegin() const;
     
    // Get end of import thunk list
    const_iterator cend() const;
    
  protected:
    // Disable copying and copy-assignment
    ImportThunkList(ImportThunkList const& Other);
    ImportThunkList& operator=(ImportThunkList const& Other);
    
  private:
    // Give ImportThunkIter access to internals
    template <typename> friend class ImportThunkIter;
    
    // Get import thunk from cache by number
    boost::optional<ImportThunk&> GetByNum(DWORD Num) const;
    
    // PeFile instance
    PeFile m_PeFile;
    
    // First thunk
    DWORD m_FirstThunk;
    
    // Import thunk cache
    mutable std::vector<ImportThunk> m_Cache;
  };

  // Import thunk iterator
  template <typename ImportThunkT>
  class ImportThunkIter : public boost::iterator_facade<
    ImportThunkIter<ImportThunkT>, ImportThunkT, boost::forward_traversal_tag>
  {
  public:
    // ImportThunkIter error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    ImportThunkIter()
      : m_pParent(nullptr), 
      m_Number(static_cast<DWORD>(-1)), 
      m_Current()
    { }
    
    // Constructor
    ImportThunkIter(class ImportThunkList const& Parent)
      : m_pParent(&Parent), 
      m_Number(0), 
      m_Current()
    {
      boost::optional<ImportThunk&> Temp = m_pParent->GetByNum(m_Number);
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
      boost::optional<ImportThunk&> Temp = m_pParent->GetByNum(++m_Number);
      m_Current = Temp ? *Temp : boost::optional<ImportThunk>();
      if (!Temp)
      {
        m_pParent = nullptr;
        m_Number = static_cast<DWORD>(-1);
      }
    }
    
    // Check iterator for equality
    bool equal(ImportThunkIter const& Rhs) const
    {
      return this->m_pParent == Rhs.m_pParent && 
        this->m_Number == Rhs.m_Number;
    }

    // Dereference iterator
    ImportThunkT& dereference() const
    {
      return *m_Current;
    }

    // Parent list instance
    class ImportThunkList const* m_pParent;
    
    // Import dir number
    DWORD m_Number;
    
    // Current import thunk instance
    mutable boost::optional<ImportThunk> m_Current;
  };
}
