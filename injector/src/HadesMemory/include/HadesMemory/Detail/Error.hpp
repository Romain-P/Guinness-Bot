// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <string>
#include <stdexcept>

#include <boost/exception/all.hpp>

#include <Windows.h>

namespace HadesMem
{
  // Base exception class
  class HadesMemError : public virtual std::exception, 
    public virtual boost::exception
  { };
  
  // Error info (function name)
  typedef boost::error_info<struct TagErrorFunc, std::string> ErrorFunction;
  // Error info (error string)
  typedef boost::error_info<struct TagErrorString, std::string> ErrorString;
  // Error info (Windows return error code)
  typedef boost::error_info<struct TagErrorCodeWinRet, DWORD_PTR> 
    ErrorCodeWinRet;
  // Error info (Windows last thread error code)
  typedef boost::error_info<struct TagErrorCodeWinLast, DWORD> 
    ErrorCodeWinLast;
  // Error info (Windows other error code)
  typedef boost::error_info<struct TagErrorCodeWinOther, DWORD_PTR> 
    ErrorCodeWinOther;
  // Error info (Other error code)
  typedef boost::error_info<struct TagErrorCodeOther, DWORD_PTR> 
    ErrorCodeOther;
}
