/**
   *  \file   stuff.hh
   *  \brief  contains some stuff
   **/
#ifndef DUNE_STUFF_COMMON_STRING_HH
#define DUNE_STUFF_COMMON_STRING_HH

#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#else
#include "config.h"
#endif // ifdef HAVE_CMAKE_CONFIG

#include <cstring>
#include <ctime>
#include <map>
#include <assert.h>
#include <algorithm>

#include <dune/common/array.hh>
#include <dune/common/deprecated.hh>
#include <dune/common/static_assert.hh>
#include <ostream>
#include <iomanip>
#include <vector>
#include <string>
#include <ctime>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

//! simple and dumb std::string to anything conversion
template <class ReturnType>
inline ReturnType fromString(const std::string& s)
{
  return boost::lexical_cast<ReturnType, std::string>(s);
} // fromString

//! simple and dumb anything to std::string conversion
template <class ReturnType>
inline std::string toString(const ReturnType& s)
{
  return boost::lexical_cast<std::string, ReturnType>(s);
} // toString

/**
  \brief      Returns a string of lengths s' whitespace (or c chars).
  \param[in]  s
              std::string, defines the length of the return string
  \param[in]  c
              char, optional argument, defines entries of return string
  \return     std::string
              Returns a string of lengths s' whitespace (or c chars).
  **/
std::string whitespaceify(const std::string s, const char c = ' ')
{
  std::string ret = "";
  for (unsigned int i = 0; i < s.size(); ++i) {
    ret += c;
  }
  return ret;
} // end function whitespaceify

#if HAS_LAMBDA_FUNCTIONS && HAS_STD_BEGIN_END
/** \brief convenience wrapper around boost::algorithm::split to split one string into a vector of strings
 * \param msg the spring to be split
 * \param seperators a list of seperaors, duh
 * \param mode token_compress_off --> potentially empty strings in return,
              token_compress_on --> empty tokens are discarded
 * \return all tokens in a vector, if msg contains no seperators, this'll contain msg as its only element
 **/
template <class T = std::string>
inline std::vector<T>
tokenize(const std::string& msg, const std::string& seperators,
         const boost::algorithm::token_compress_mode_type mode = boost::algorithm::token_compress_off)
{
  std::vector<std::string> strings;
  boost::algorithm::split(strings, msg, boost::algorithm::is_any_of(seperators), mode);
  std::vector<T> ret(strings.size());
  size_t i = 0;
  // special case for empty strings to avoid non-default init
  std::generate(
      std::begin(ret), std::end(ret), [&]() { return strings[i++].empty() ? T() : fromString<T>(strings[i - 1]); });
  return ret;
}
//! if the compiler isn't recent enough we make tokenize avalilable for string only
template <>
#endif // HAS_LAMBDA_FUNCTIONS && HAS_STD_BEGIN_END
inline std::vector<std::string> tokenize(const std::string& msg, const std::string& seperators,
                                         const boost::algorithm::token_compress_mode_type mode)
{
  std::vector<std::string> strings;
  boost::algorithm::split(strings, msg, boost::algorithm::is_any_of(seperators), mode);
  return strings;
}


//! returns string with local time in current locale's format
inline std::string stringFromTime(time_t cur_time = time(NULL))
{
  return ctime(&cur_time);
}

//! helper struct for lexical cast
template <typename ElemT>
struct HexToString
{
  // see http://stackoverflow.com/a/2079728
  ElemT value;
  operator ElemT() const
  {
    return value;
  }
  friend std::istream& operator>>(std::istream& in, HexToString& out)
  {
    in >> std::hex >> out.value;
    return in;
  }
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_STRING_HH

/** Copyright (c) 2012, Rene Milk    , Sven Kaulmann
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
