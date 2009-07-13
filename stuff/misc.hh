/**
 *  \file   stuff.hh
 *  \brief  contains some stuff
 **/
#ifndef STUFF_MISC_HH_INCLUDED
#define STUFF_MISC_HH_INCLUDED

#define SEGFAULT                                                                                                       \
  {                                                                                                                    \
    int* J = 0;                                                                                                        \
    *J     = 9;                                                                                                        \
  }

template <typename T>
bool isnan(T x)
{
  return !(x == x);
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

#include <fstream>
#include <ostream>
#include <sstream>
#include <vector>
#include <assert.h>
#include <cmath>

namespace Stuff {

/**
 *  \todo doc me
 **/
template <class ReturnType>
ReturnType fromString(const std::string& s)
{
  std::stringstream ss;
  ss << s;
  ReturnType r;
  ss >> r;
  return r;
}

/**
 *  \todo doc
 **/
template <class ReturnType>
std::string toString(const ReturnType& s)
{
  std::stringstream ss;
  ss << s;
  std::string r;
  ss >> r;
  return r;
}

template <class Info>
class TexOutput
{
  typedef std::vector<std::string> Strings;

  Info info_;
  double current_h_;
  Strings headers_;


public:
  TexOutput(const Info& info, Strings& headers)
    : info_(info)
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  TexOutput(Strings& headers)
    : info_(Info())
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  void setInfo(const Info& info)
  {
    info_ = info;
  }

  void putLineEnd(std::ofstream& outputFile_)
  {
    outputFile_ << "\n"
                << "\\tabularnewline\n"
                << "\\hline \n";
    outputFile_.flush();
  }

  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool /*initial*/)
  {
    current_h_    = info_.grid_width;
    double factor = current_h_ / prevh_;
    double eoc    = std::log(error_ / prevError_) / std::log(factor);
    outputFile_ << " & " << error_ << " & " << eoc;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";
    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << "|c|";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\hline \n";
    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << headers_[i];
      if (i < statColSize - 1)
        outputFile_ << " & ";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << " & " << headers_[i + statColSize] << " & EOC ";
    }
    outputFile_ << "\n \\endhead\n"
                << "\\hline\n"
                << "\\hline\n";
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << info_.run_time << " & "
                << info_.c11 << " & " << info_.d11 << " & " << info_.c12 << " & " << info_.d12;
  }

  void endTable(std::ofstream& outputFile_)
  {
    outputFile_ << "\\end{longtable}";
    outputFile_.flush();
  }

  double get_h()
  {
    return current_h_;
  }
};

/**
 *  \brief Only free mem pointed to by valid pointer, log warning otherwise
 *
 **/

template <class T>
void safe_delete(T t)
{
  if (t) {
    delete t;
    t = 0;
  }
  // else log warning
}

template <class Container, class Element>
int getIdx(const Container& ct, Element e)
{
  int idx = 0;
  for (typename Container::const_iterator it = ct.begin(); it != ct.end(); ++it, ++idx) {
    if (*it == e)
      return idx;
  }
  return -1;
}

} // end namepspace stuff


#endif // end of stuff.hh
