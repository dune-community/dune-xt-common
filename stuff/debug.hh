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
}

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
#endif
#else
#define LOGIC_ERROR
#endif

char* copy(const char* s)
{
  int l   = strlen(s) + 1;
  char* t = new char[l];
  for (int i = 0; i < l; i++) {
    t[i] = s[i];
  }
  return t;
}
#define __CLASS__ strtok(copy(__PRETTY_FUNCTION__), "<(")

#ifndef NDEBUG
#define NEEDS_IMPLEMENTATION                                                                                           \
  {                                                                                                                    \
    std::stringstream ss;                                                                                              \
    ss << " implementation missing: " << __CLASS__ << " -- " << rightPathLimiter(__FILE__) << ":" << __LINE__;         \
    std::cerr << ss.str() << std::endl;                                                                                \
  }
#else
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

namespace Stuff {
class singlerun_abort_exception : public std::runtime_error
{
public:
  singlerun_abort_exception(std::string msg)
    : std::runtime_error(msg)
  {
  }
};
}

#ifndef NDEBUG
#define ASSERT_EXCEPTION(cond, msg)                                                                                    \
  if (!(cond)) {                                                                                                       \
    std::string rmsg(std::string(__FILE__) + std::string(":") + Stuff::toString(__LINE__) + std::string("\n")          \
                     + std::string(msg));                                                                              \
    throw assert_exception(rmsg);                                                                                      \
  }
#else
#define ASSERT_EXCEPTION(cond, msg)
#endif

#if 1 /*  there should be no more any compilers needing the "#else" version */
#define UNUSED(identifier) /* identifier */
#else /*  stupid, broken compiler */
#define UNUSED(identifier) identifier
#endif

/*  some arguments are only used in debug mode, but unused in release one */
#ifndef NDEBUG
#define UNUSED_UNLESS_DEBUG(param) param
#else
#define UNUSED_UNLESS_DEBUG(param) UNUSED(param)
#endif


#define ASSERT_LT(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt < actual),                                                                                                 \
      (boost::format("assertion %1% < %2% failed: %3% >= %4%") % #expt % #actual % expt % actual).str().c_str())
#define ASSERT_EQ(expt, actual)                                                                                        \
  BOOST_ASSERT_MSG(                                                                                                    \
      (expt == actual),                                                                                                \
      (boost::format("assertion %1% == %2% failed: %3% != %4%") % #expt % #actual % expt % actual).str().c_str())

#endif // DUNE_STUFF_DEBUG_HH
