// Copyright Joshua Boyce 2011.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// This file is part of HadesMem.
// <http://www.raptorfactor.com/> <raptorfactor@raptorfactor.com>

#pragma once

#include <string>
#include <vector>

namespace HadesMem
{
  namespace Detail
  {
    // Utility class to 'bind' a temporary buffer to a string and allow 
    // easy interoperability with C-style APIs.
    template <class CharT>
    class StringBuffer
    {
    public:
      // Constructor
      StringBuffer(std::basic_string<CharT>& String, 
        typename std::vector<CharT>::size_type Size) 
        : m_String(&String), 
        m_Buffer(Size + 1) 
      { }
  
      // Move constructor
      StringBuffer(StringBuffer&& Other)
      {
        *this = std::move(Other);
      }
  
      // Move assignment operator
      StringBuffer& operator=(StringBuffer&& Other)
      {
        this->m_String = Other.m_String;
        Other.m_String = nullptr;
  
        this->m_Buffer = std::move(Other.m_Buffer);
  
        return *this;
      }
  
      // Destructor
      ~StringBuffer()
      {
        try
        {
          Commit();
        }
        catch (...)
        { }
      }
  
      // Get pointer to internal buffer
      CharT* Get()
      {
        return m_Buffer.data();
      }
  
      // Get pointer to internal buffer
      CharT const* Get() const
      {
        return m_Buffer.data();
      }
  
      // Implicit conversion operator to allow for easy C-style API 
      // interoperability
      operator CharT* ()
      {
        return Get();
      }
  
      // Implicit conversion operator to allow for easy C-style API 
      // interoperability
      operator CharT const* () const
      {
        return Get();
      }
  
      // Commit current buffer to target string
      void Commit()
      {
        if (!m_Buffer.empty())
        {
          *m_String = m_Buffer.data();
          m_Buffer.clear();
        }
      }
  
      // Clear current buffer
      void Abort()
      {
        m_Buffer.clear();
      }
      
    protected:
      // Disable copying
      StringBuffer(StringBuffer const&);
      StringBuffer& operator=(StringBuffer const&);
  
    private:
      // Target string
      std::basic_string<CharT>* m_String;
  
      // Temporary buffer
      std::vector<CharT> m_Buffer;
    };
  
    // Make string buffer. Simple helper function to deduce character type.
    template <class CharT>
    StringBuffer<CharT> MakeStringBuffer(std::basic_string<CharT>& String, 
      std::size_t Size)
    {
      return StringBuffer<CharT>(String, Size);
    }
  }
}
