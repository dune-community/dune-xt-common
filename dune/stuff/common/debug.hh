// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

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

inline char* charcopy(const char* s)
{
  size_t l = strlen(s) + 1;
  char* t = new char[l];
  for (size_t i = 0; i < l; i++) {
    t[i] = s[i];
  }
  return t;
} // copy

#define __CLASS__ strtok(charcopy(__PRETTY_FUNCTION__), "<(")


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


#define UNUSED(identifier) /* identifier */

/*  some arguments are only used in debug mode, but unused in release one */
#ifndef NDEBUG
#define UNUSED_UNLESS_DEBUG(param) param
#else
#define UNUSED_UNLESS_DEBUG(param) UNUSED(param)
#endif // ifndef NDEBUG

#ifndef ASSERT_LT
#define ASSERT_LT(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt < actual),                                                                                                 \
      (boost::format("assertion %1% < %2% failed: %3% >= %4%") % #expt % #actual % expt % actual).str().c_str())
#endif

#ifndef ASSERT_EQ
#define ASSERT_EQ(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt == actual),                                                                                                \
      (boost::format("assertion %1% == %2% failed: %3% != %4%") % #expt % #actual % expt % actual).str().c_str())
#endif

#endif // DUNE_STUFF_DEBUG_HH
