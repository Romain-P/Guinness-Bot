// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <string>
#include <iterator>

namespace HadesMem
{
  namespace Detail
  {
    // This routine appends the given argument to a command line such
    // that CommandLineToArgvW will return the argument string unchanged.
    // Arguments in a command line should be separated by spaces; this
    // function does not add these spaces.
    inline void ArgvQuote(const std::wstring& Argument, 
      std::wstring& CommandLine, bool Force)
    {
      // Unless we're told otherwise, don't quote unless we actually
      // need to do so --- hopefully avoid problems if programs won't
      // parse quotes properly
      if (!Force && !Argument.empty () && Argument.find_first_of(L" \t\n\v\"") 
        == Argument.npos)
      {
        CommandLine.append(Argument);
      }
      else 
      {
        CommandLine.push_back(L'"');
        
        for (auto It = std::begin(Argument); ;++It)
        {
          unsigned NumberBackslashes = 0;
          
          while (It != std::end(Argument) && *It == L'\\') 
          {
            ++It;
            ++NumberBackslashes;
          }
      
          if (It == std::end(Argument))
          {
            // Escape all backslashes, but let the terminating
            // double quotation mark we add below be interpreted
            // as a metacharacter.
            CommandLine.append(NumberBackslashes * 2, L'\\');
            break;
          }
          else if (*It == L'"')
          {
            // Escape all backslashes and the following
            // double quotation mark.
            CommandLine.append(NumberBackslashes * 2 + 1, L'\\');
            CommandLine.push_back(*It);
          }
          else
          {
            // Backslashes aren't special here.
            CommandLine.append(NumberBackslashes, L'\\');
            CommandLine.push_back(*It);
          }
        }
        
        CommandLine.push_back(L'"');
      }
    }
  }
}
