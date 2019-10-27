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
#include <vector>
#include <utility>
#include <iterator>

#include <boost/optional.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <Windows.h>

namespace HadesMem
{
  // PE file export directory
  class ExportDir
  {
  public:
    // ExportDir error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    explicit ExportDir(PeFile const& MyPeFile);
      
    // Copy constructor
    ExportDir(ExportDir const& Other);
    
    // Copy assignment operator
    ExportDir& operator=(ExportDir const& Other);
    
    // Move constructor
    ExportDir(ExportDir&& Other);
    
    // Move assignment operator
    ExportDir& operator=(ExportDir&& Other);
    
    // Destructor
    ~ExportDir();
    
    // Equality operator
    bool operator==(ExportDir const& Rhs) const;
    
    // Inequality operator
    bool operator!=(ExportDir const& Rhs) const;

    // Get base of export dir
    PVOID GetBase() const;

    // Whether export directory is valid
    bool IsValid() const;

    // Ensure export directory is valid
    void EnsureValid() const;

    // Get characteristics
    DWORD GetCharacteristics() const;

    // Get time date stamp
    DWORD GetTimeDateStamp() const;

    // Get major version
    WORD GetMajorVersion() const;

    // Get minor version
    WORD GetMinorVersion() const;

    // Get module name
    std::string GetName() const;

    // Get ordinal base
    DWORD GetOrdinalBase() const;

    // Get number of functions
    DWORD GetNumberOfFunctions() const;

    // Get number of names
    DWORD GetNumberOfNames() const;

    // Get address of functions
    DWORD GetAddressOfFunctions() const;

    // Get address of names
    DWORD GetAddressOfNames() const;

    // Get address of name ordinals
    DWORD GetAddressOfNameOrdinals() const;

    // Set characteristics
    void SetCharacteristics(DWORD Characteristics) const;

    // Set time date stamp
    void SetTimeDateStamp(DWORD TimeDateStamp) const;

    // Set major version
    void SetMajorVersion(WORD MajorVersion) const;

    // Set minor version
    void SetMinorVersion(WORD MinorVersion) const;

    // Set module name
    void SetName(std::string const& Name) const;

    // Set ordinal base
    void SetOrdinalBase(DWORD OrdinalBase) const;

    // Set number of functions
    void SetNumberOfFunctions(DWORD NumberOfFunctions) const;

    // Set number of names
    void SetNumberOfNames(DWORD NumberOfNames) const;

    // Set address of functions
    void SetAddressOfFunctions(DWORD AddressOfFunctions) const;

    // Set address of names
    void SetAddressOfNames(DWORD AddressOfNames) const;

    // Set address of name ordinals
    void SetAddressOfNameOrdinals(DWORD AddressOfNameOrdinals) const;

  private:
    // PE file
    PeFile m_PeFile;

    // Memory instance
    MemoryMgr m_Memory;

    // Base of export dir
    mutable PBYTE m_pBase;
  };

  // PE file export data
  class Export
  {
  public:
    // Constructor
    Export(PeFile const& MyPeFile, DWORD Ordinal);
    
    // Constructor
    Export(PeFile const& MyPeFile, std::string const& Name);
      
    // Copy constructor
    Export(Export const& Other);
    
    // Copy assignment operator
    Export& operator=(Export const& Other);
    
    // Move constructor
    Export(Export&& Other);
    
    // Move assignment operator
    Export& operator=(Export&& Other);
    
    // Destructor
    ~Export();

    // Get RVA
    DWORD GetRva() const;

    // Get VA
    PVOID GetVa() const;

    // Get name
    std::string GetName() const;

    // Get ordinal
    WORD GetOrdinal() const;

    // If entry is exported by name
    bool ByName() const;

    // If entry is forwarded
    bool Forwarded() const;

    // Get forwarder
    std::string GetForwarder() const;
    
    // Get forwarder module name
    std::string GetForwarderModule() const;
    
    // Get forwarder function name
    std::string GetForwarderFunction() const;
    
    // If entry is forwarded by ordinal
    bool IsForwardedByOrdinal() const;
    
    // Get forwarder function ordinal
    WORD GetForwarderOrdinal() const;
    
    // Equality operator
    bool operator==(Export const& Rhs) const;
    
    // Inequality operator
    bool operator!=(Export const& Rhs) const;

  private:
    // PE file instance
    PeFile m_PeFile;
    
    // Memory instance
    MemoryMgr m_Memory;

    // RVA
    DWORD m_Rva;
    
    // VA
    PVOID m_Va;
    
    // Name
    std::string m_Name;
    
    // Forwarder
    std::string m_Forwarder;
    
    // Split forwarder
    std::pair<std::string, std::string> m_ForwarderSplit;
    
    // Ordinal
    WORD m_Ordinal;
    
    // If entry is exported by name
    bool m_ByName;
    
    // If entry is forwarded
    bool m_Forwarded;
    
    // If entry is forwarded by ordinal
    bool m_ForwardedByOrdinal;
    
    // Forwarder ordinal
    WORD m_ForwarderOrdinal;
  };
    
  // Forward declaration of ExportIter
  template <typename ExportT>
  class ExportIter;
  
  // Export enumeration class
  class ExportList
  {
  public:
    // ExportList exception type
    class Error : public virtual HadesMemError
    { };
    
    // Export list iterator types
    typedef ExportIter<Export> iterator;
    typedef ExportIter<Export const> const_iterator;
    
    // Constructor
    explicit ExportList(PeFile const& MyPeFile);
    
    // Move constructor
    ExportList(ExportList&& Other);
    
    // Move assignment operator
    ExportList& operator=(ExportList&& Other);
    
    // Get start of export list
    iterator begin();
    
    // Get end of export list
    iterator end();
    
    // Get start of export list
    const_iterator begin() const;
     
    // Get end of export list
    const_iterator end() const;
    
    // Get start of export list
    const_iterator cbegin() const;
     
    // Get end of export list
    const_iterator cend() const;
    
  protected:
    // Disable copying and copy-assignment
    ExportList(ExportList const& Other);
    ExportList& operator=(ExportList const& Other);
    
  private:
    // Give ExportIter access to internals
    template <typename> friend class ExportIter;
    
    // Get export from cache by number
    boost::optional<Export&> GetByNum(DWORD Num) const;
    
    // PeFile instance
    PeFile m_PeFile;
    
    // Export cache
    mutable std::vector<Export> m_Cache;
  };

  // Export iterator
  template <typename ExportT>
  class ExportIter : public boost::iterator_facade<ExportIter<ExportT>, 
    ExportT, boost::forward_traversal_tag>
  {
  public:
    // Export iterator error class
    class Error : public virtual HadesMemError
    { };

    // Constructor
    ExportIter()
      : m_pParent(nullptr), 
      m_Number(static_cast<DWORD>(-1)), 
      m_Current()
    { }
    
    // Constructor
    ExportIter(class ExportList const& Parent)
      : m_pParent(&Parent), 
      m_Number(0), 
      m_Current()
    {
      boost::optional<Export&> Temp = m_pParent->GetByNum(m_Number);
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
      boost::optional<Export&> Temp = m_pParent->GetByNum(++m_Number);
      m_Current = Temp ? *Temp : boost::optional<Export>();
      if (!Temp)
      {
        m_pParent = nullptr;
        m_Number = static_cast<DWORD>(-1);
      }
    }
    
    // Check iterator for equality
    bool equal(ExportIter const& Rhs) const
    {
      return this->m_pParent == Rhs.m_pParent && 
        this->m_Number == Rhs.m_Number;
    }

    // Dereference iterator
    ExportT& dereference() const
    {
      return *m_Current;
    }

    // Parent list instance
    class ExportList const* m_pParent;
    
    // Export number
    DWORD m_Number;
    
    // Current export instance
    mutable boost::optional<Export> m_Current;
  };
}
