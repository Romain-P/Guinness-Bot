// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Error.hpp>

#include <string>
#include <vector>

#include <Windows.h>

namespace HadesMem
{
  // DLL injection class
  class Injector
  {
  public:
    // Injector exception type
    class Error : public virtual HadesMemError 
    { };

    // Constructor
    explicit Injector(MemoryMgr const& MyMemory);
      
    // Copy constructor
    Injector(Injector const& Other);
    
    // Copy assignment operator
    Injector& operator=(Injector const& Other);
    
    // Move constructor
    Injector(Injector&& Other);
    
    // Move assignment operator
    Injector& operator=(Injector&& Other);
    
    // Destructor
    ~Injector();
    
    // Equality operator
    bool operator==(Injector const& Rhs) const;
    
    // Inequality operator
    bool operator!=(Injector const& Rhs) const;
    
    // Injection flags
    enum InjectFlags
    {
      InjectFlag_None, 
      InjectFlag_PathResolution
    };

    // Inject DLL
    HMODULE InjectDll(std::wstring const& Path, 
      InjectFlags Flags = InjectFlag_None) const;
      
    // Free DLL
    void FreeDll(HMODULE Module) const;
    
    // Call export
    MemoryMgr::RemoteFunctionRet CallExport(HMODULE RemoteModule, 
      std::string const& Export) const;

  private:
    // MemoryMgr instance
    MemoryMgr m_Memory;
  };
    
  // Return data for CreateAndInject
  class CreateAndInjectData
  {
  public:
    // Constructor
    CreateAndInjectData(MemoryMgr const& MyMemory, HMODULE Module, 
      DWORD_PTR ExportRet, DWORD ExportLastError);
      
    // Copy constructor
    CreateAndInjectData(CreateAndInjectData const& Other);
    
    // Copy assignment operator
    CreateAndInjectData& operator=(CreateAndInjectData const& Other);
    
    // Move constructor
    CreateAndInjectData(CreateAndInjectData&& Other);
    
    // Move assignment operator
    CreateAndInjectData& operator=(CreateAndInjectData&& Other);
    
    // Destructor
    ~CreateAndInjectData();
    
    // Get memory instance
    MemoryMgr GetMemoryMgr() const;
    
    // Get module handle
    HMODULE GetModule() const;
    
    // Get export return value
    DWORD_PTR GetExportRet() const;
    
    // Get export last error code
    DWORD GetExportLastError() const;
    
  private:
    MemoryMgr m_Memory;
    HMODULE m_Module;
    DWORD_PTR m_ExportRet;
    DWORD m_ExportLastError;
  };
  
  // Create process (as suspended) and inject DLL
  CreateAndInjectData CreateAndInject(
    std::wstring const& Path, 
    std::wstring const& WorkDir, 
    std::vector<std::wstring> const& Args, 
    std::wstring const& Module, 
    std::string const& Export, 
    Injector::InjectFlags Flags = Injector::InjectFlag_None);
}
