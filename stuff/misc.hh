/**
 *  \file   stuff.hh
 *  \brief  contains some stuff
 **/
#ifndef STUFF_MISC_HH_INCLUDED
#define STUFF_MISC_HH_INCLUDED

#include <cstring>
#include <map>
#include <assert.h>
#include <algorithm>
#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#else
#include "config.h"
#endif
#include <dune/common/version.hh>
#if DUNE_VERSION_NEWER_REV(DUNE_COMMON, 1, 3, 0) // ie >= 2.0
#include <dune/common/array.hh>
#else
#include <dune/common/fixedarray.hh>
#endif

#include "static_assert.hh"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cerrno>
#include <limits>
#include <string.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>

namespace Stuff {

//! simple and dumb std::string to anything conversion
template <class ReturnType>
ReturnType fromString(const std::string& s)
{
  std::stringstream ss;
  ss << s;
  ReturnType r;
  ss >> r;
  return r;
}

//! simple and dumb anything to std::string conversion
template <class ReturnType>
std::string toString(const ReturnType& s)
{
  std::stringstream ss;
  ss << s;
  std::string r;
  ss >> r;
  return r;
}

/** stupid element-index-in-conatiner search
  \todo stl implementation?
  **/
template <class Container, class Element>
int getIdx(const Container& ct, const Element& e)
{
  int idx = 0;
  for (typename Container::const_iterator it = ct.begin(); it != ct.end(); ++it, ++idx) {
    if (*it == e)
      return idx;
  }
  return -1;
}

//! no idea what this was for
template <typename T>
std::string getParameterString(const std::string& prefix, T min, T max, T inc)
{
  std::stringstream ss;
  ss << prefix << " ";
  for (; min < max; min += inc) {
    ss << min << ",  ";
  }
  return ss.str();
}

/** \brief string to AnyType tokenizer
  A given string is split according to a list of delimiters. Each token is coerced into class parameter
TokenType and saved in a vector which is exposed via iterator-like mechanism
  \see StringTokenizer, a shorthand tyoedef for string to string tokenisation
  **/
template <class TokenType>
class Tokenizer
{

protected:
  typedef std::vector<TokenType> Tokens;
  typedef typename Tokens::iterator TokenIterator;

public:
  Tokenizer(const std::string tokenstring, const std::string delimiters)
  {
    // code taken from http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
    // Skip delimiters at beginning.
    std::string::size_type lastPos = tokenstring.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = tokenstring.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
      // Found a token, add it to the vector.
      tokens_.push_back(fromString<TokenType>(tokenstring.substr(lastPos, pos - lastPos)));
      // Skip delimiters.  Note the "not_of"
      lastPos = tokenstring.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = tokenstring.find_first_of(delimiters, lastPos);
    }
    currentToken_ = tokens_.begin();
  }

  Tokens getTokens()
  {
    return tokens_;
  }

  bool hasMoreTokens()
  {
    return currentToken_ != tokens_.end();
  }

  TokenType getNextToken()
  {
    TokenType ret = *currentToken_;
    ++currentToken_;
    return ret;
  }

protected:
  TokenIterator currentToken_;
  Tokens tokens_;
};

typedef Tokenizer<std::string> StringTokenizer;

//! for backward compat only
template <class ContainerType>
void MergeVector(ContainerType& target, const ContainerType& a)
{
  target.insert(target.end(), a.begin(), a.end());
}

//! stupid timing helper
struct TimeGuard
{
  const time_t cur_time;
  TimeGuard()
    : cur_time(time(NULL))
  {
  }
  ~TimeGuard()
  {
    time_t delta = time(NULL) - cur_time;
    std::cout << ctime(&delta) << std::endl;
  }
};

//! \todo seems borked, resutls in gigantic amount of compile errors?!
template <class StlContainer, class T>
void fill_entirely(StlContainer& c, const T& value)
{
  std::fill(c.begin(), c.end(), value);
}

template <class T, int N>
/** this allows subscription indices to wrap around
  \example N=4: wraparound_array[4] == wraparound_array[0] && wraparound_array[-1] == wraparound_array[3]
  **/
struct wraparound_array : public Dune::array<T, N>
{
  typedef Dune::array<T, N> BaseType;
  wraparound_array()
  {
    for (size_t i = 0; i < N; ++i)
      this->operator[](i) = T();
  }
  wraparound_array(const BaseType other)
  {
    for (size_t i = 0; i < N; ++i)
      this->operator[](i) = other[i];
  }
  typename BaseType::reference operator[](std::size_t i)
  {
    return BaseType::operator[](i % N);
  }
  typename BaseType::reference operator[](int i)
  {
    std::size_t real_index = i;
    if (i < 0)
      real_index = static_cast<size_t>(N - (((i * -1) % N) + 1));
    return BaseType::operator[](real_index);
  }
  typename BaseType::const_reference operator[](std::size_t i) const
  {
    return BaseType::operator[](i % N);
  }
  typename BaseType::const_reference operator[](int i) const
  {
    std::size_t real_index = i;
    if (i < 0)
      real_index = static_cast<size_t>(N - (((i * -1) % N) + 1));
    return BaseType::operator[](real_index);
  }
};

//! type safe (this will not compile for degraded-to-pointer arrays) way of getting array length
template <class T, size_t N>
size_t arrayLength(T(&/*array*/)[N])
{
  return N;
}

void binaryInfo()
{
  assert(false);
  //    std::cout << "-O " << __OPTIMIZE__   << "\n";
}

} // end namepspace stuff

#endif // end of stuff.hh
