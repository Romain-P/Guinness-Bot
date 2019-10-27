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

// Hades
#include <HadesMemory/MemoryMgr.hpp>
#include <HadesMemory/Detail/Fwd.hpp>
#include <HadesMemory/Detail/Error.hpp>

// C++ Standard Library
#include <vector>
#include <memory>

// Windows API
#include <Windows.h>

namespace HadesMem
{
  // Patch class.
  // Abstract base class for different patch types.
  class Patch
  {
  public:
    // Patch exception type
    class Error : public virtual HadesMemError
    { };

    // Constructor
    explicit Patch(MemoryMgr const& MyMemory);
    
    // Move constructor
    Patch(Patch&& Other);
    
    // Move assignment operator
    Patch& operator=(Patch&& Other);

    // Destructor
    virtual ~Patch();

    // Apply patch
    virtual void Apply() = 0;
    // Remove patch
    virtual void Remove() = 0;

    // Whether patch is currently applied
    bool IsApplied() const;

  protected:
    // Memory manager instance
    MemoryMgr m_Memory;

    // Whether patch is currently applied
    bool m_Applied;
    
  private:
    // Disable copying
    Patch(Patch const&);
    Patch& operator=(Patch const&);
  };

  // Raw patch (a.k.a. 'byte patch').
  // Used to perform a simple byte patch on a target.
  class PatchRaw : public Patch
  {
  public:
    // Constructor
    PatchRaw(MemoryMgr const& MyMemory, PVOID Target, 
      std::vector<BYTE> const& Data);
    
    // Move constructor
    PatchRaw(PatchRaw&& Other);
    
    // Move assignment operator
    PatchRaw& operator=(PatchRaw&& Other);
        
    // Destructor
    virtual ~PatchRaw();

    // Apply patch
    virtual void Apply();

    // Remove patch
    virtual void Remove();

  private:
    // Patch target
    PVOID m_Target;

    // New data
    std::vector<BYTE> m_Data;

    // Original data
    std::vector<BYTE> m_Orig;
  };

  // Detour patch (a.k.a. 'hook').
  // Performs an 'inline' or 'jump' hook on the target.
  class PatchDetour : public Patch
  {
  public:
    // Constructor
    PatchDetour(MemoryMgr const& MyMemory, PVOID Target, PVOID Detour);
    
    // Move constructor
    PatchDetour(PatchDetour&& Other);
    
    // Move assignment operator
    PatchDetour& operator=(PatchDetour&& Other);
        
    // Destructor
    virtual ~PatchDetour();

    // Apply patch
    virtual void Apply();
    
    // Remove patch
    virtual void Remove();

    // Get pointer to trampoline
    PVOID GetTrampoline() const;

  private:
    // Write jump to target at address
    void WriteJump(PVOID Address, PVOID Target);

    // Get size of jump instruction for current platform
    unsigned int GetJumpSize() const;

    // Target address
    PVOID m_Target;
    // Detour address
    PVOID m_Detour;
    // Trampoline memory
    std::unique_ptr<AllocAndFree> m_Trampoline;
    // Backup code
    std::vector<BYTE> m_Orig;
  };
}
