#ifndef DUNE_STUFF_DEBUG_HH
#define DUNE_STUFF_DEBUG_HH

#include <cstring>
#include <boost/assert.hpp>
#include <boost/format.hpp>

#define SEGFAULT                                                                                                       \
  {                                                                                                                    \
    int* J = 0;                                                                                                        \
    *J     = 9;                                                                                                        \
  }

//! from right/bottom limiter for file paths
const char* rightPathLimiter(const char* path, int depth = 2)
{
  char* c = new char[255];

  strcpy(c, path);
  const char* p = strtok(c, "/");
  int i = 0;
  while (p && i < depth) {
    p = strtok(NULL, "/");
  }
  p = strtok(NULL, "\0");
  return p;
} // rightPathLimiter

#ifndef NDEBUG
#ifndef LOGIC_ERROR
#include <stdexcept>
#include <sstream>
#define LOGIC_ERROR                                                                                                    \
  {                                                                                                                    \
    std::stringstream ss;                                                                                              \
    ss << __FILE__ << ":" << __LINE__ << " should never be called";                                                    \
    throw std::logic_error(ss.str());                                                                                  \
  }
#endif // ifndef LOGIC_ERROR
#else // ifndef NDEBUG
#define LOGIC_ERROR
#endif // ifndef NDEBUG

char* copy(const char* s)
{
  int l   = strlen(s) + 1;
  char* t = new char[l];
  for (int i = 0; i < l; i++) {
    t[i] = s[i];
  }
  return t;
} // copy
#define __CLASS__ strtok(copy(__PRETTY_FUNCTION__), "<(")

#ifndef NDEBUG
#define NEEDS_IMPLEMENTATION                                                                                           \
  {                                                                                                                    \
    std::stringstream ss;                                                                                              \
    ss << " implementation missing: " << __CLASS__ << " -- " << rightPathLimiter(__FILE__) << ":" << __LINE__;         \
    std::cerr << ss.str() << std::endl;                                                                                \
  }
#else // ifndef NDEBUG
#define NEEDS_IMPLEMENTATION
#endif // NDEBUG

class assert_exception : public std::runtime_error
{
public:
  assert_exception(std::string msg)
    : std::runtime_error(msg)
  {
  }
};

class singlerun_abort_exception : public std::runtime_error
{
public:
  singlerun_abort_exception(std::string msg)
    : std::runtime_error(msg)
  {
  }
};

#ifndef NDEBUG
#define ASSERT_EXCEPTION(cond, msg)                                                                                    \
  if (!(cond)) {                                                                                                       \
    std::string rmsg(std::string(__FILE__) + std::string(":") + Stuff::Common::String::convertTo(__LINE__)             \
                     + std::string("\n")                                                                               \
                     + std::string(msg));                                                                              \
    throw assert_exception(rmsg);                                                                                      \
  }
#else // ifndef NDEBUG
#define ASSERT_EXCEPTION(cond, msg)
#endif // ifndef NDEBUG

#define UNUSED(identifier) /* identifier */

/*  some arguments are only used in debug mode, but unused in release one */
#ifndef NDEBUG
#define UNUSED_UNLESS_DEBUG(param) param
#else
#define UNUSED_UNLESS_DEBUG(param) UNUSED(param)
#endif // ifndef NDEBUG

#define ASSERT_LT(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt < actual),                                                                                                 \
      (boost::format("assertion %1% < %2% failed: %3% >= %4%") % #expt % #actual % expt % actual).str().c_str())
#define ASSERT_EQ(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt == actual),                                                                                                \
      (boost::format("assertion %1% == %2% failed: %3% != %4%") % #expt % #actual % expt % actual).str().c_str())

#endif // DUNE_STUFF_DEBUG_HH

/** Copyright (c) 2012, Rene Milk
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
