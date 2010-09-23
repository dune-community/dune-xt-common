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

#include <cstring>
#include <assert.h>

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

const char* pathStuff(const char* path, int depth = 2)
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
#define NEEDS_IMPLEMENTATION                                                                                           \
  {                                                                                                                    \
    std::stringstream ss;                                                                                              \
    ss << " implementation missing: " << __CLASS__ << " -- " << pathStuff(__FILE__) << ":" << __LINE__;                \
    std::cerr << ss.str() << std::endl;                                                                                \
  }
#else
#define NEEDS_IMPLEMENTATION
#endif // NDEBUG

#include <fstream>
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

class assert_exception : public std::runtime_error
{
public:
  assert_exception(std::string msg)
    : std::runtime_error(msg){};
};

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

#define HAS_RUN_INFO

/** \brief wrap any info that might be remotely interesting about a single run
  \note define this beforepass is included so it's known in pass. Yes, I know it's ugly.
  **/
struct RunInfo
{
  std::vector<double> L2Errors;
  double grid_width;
  int refine_level;
  double run_time;
  long codim0;
  int polorder_velocity;
  int polorder_pressure;
  int polorder_sigma;
  std::pair<int, double> c11, d11, c12, d12;
  bool bfg;
  std::string gridname;
  double solver_accuracy;
  double inner_solver_accuracy;
  double bfg_tau;
  std::string extra_info;
  int iterations_inner_avg;
  int iterations_inner_min;
  int iterations_inner_max;
  int iterations_outer_total;
  double max_inner_accuracy;
  std::string problemIdentifier;
  double current_time, delta_t, viscosity, reynolds, alpha;

  RunInfo()
  {
    refine_level = codim0 = polorder_velocity = polorder_sigma = polorder_pressure = iterations_inner_avg =
        iterations_inner_min = iterations_inner_max = iterations_outer_total = -1;
    bfg     = true;
    bfg_tau = max_inner_accuracy = grid_width = solver_accuracy = run_time = alpha = inner_solver_accuracy = -1.0;
    gridname = problemIdentifier = "UNSET";
    extra_info   = "";
    delta_t      = 0.1;
    current_time = 0.0;
    viscosity = reynolds = 1.0;
  }
};

//! used in all runs to store L2 errors across runs
typedef std::vector<RunInfo> RunInfoVector;

typedef unsigned int RunInfoVectorMapKeyType;

typedef std::map<RunInfoVectorMapKeyType, RunInfoVector> RunInfoVectorMap;

#include <dune/common/misc.hh>

namespace Stuff {

/** \todo DOCME **/
template <class SomeRangeType, class OtherRangeType>
static double colonProduct(const SomeRangeType& arg1, const OtherRangeType& arg2)
{
  Dune::CompileTimeChecker<SomeRangeType::cols == SomeRangeType::rows && OtherRangeType::cols == OtherRangeType::rows
                           && OtherRangeType::cols == SomeRangeType::rows> RangeTypes_dont_fit;

  double ret = 0.0;
  // iterators
  typedef typename SomeRangeType::ConstRowIterator ConstRowIteratorType;
  typedef typename SomeRangeType::row_type::ConstIterator ConstIteratorType;
  ConstRowIteratorType arg1RowItEnd = arg1.end();
  ConstRowIteratorType arg2RowItEnd = arg2.end();
  ConstRowIteratorType arg2RowIt = arg2.begin();
  for (ConstRowIteratorType arg1RowIt = arg1.begin(); arg1RowIt != arg1RowItEnd, arg2RowIt != arg2RowItEnd;
       ++arg1RowIt, ++arg2RowIt) {
    ConstIteratorType row1ItEnd = arg1RowIt->end();
    ConstIteratorType row2ItEnd = arg2RowIt->end();
    ConstIteratorType row2It = arg2RowIt->begin();
    for (ConstIteratorType row1It = arg1RowIt->begin(); row1It != row1ItEnd, row2It != row2ItEnd; ++row1It, ++row2It) {
      ret += *row1It * *row2It;
    }
  }
  return ret;
}

/**
 *  \brief  dyadic product
 *
 *          Implements \f$\left(arg_{1} \otimes arg_{2}\right)_{i,j}:={arg_{1}}_{i} {arg_{2}}_{j}\f$
 *			RangeType1 should be fieldmatrix, RangeType2 fieldvector
 **/
template <class RangeType1, class RangeType2>
static RangeType1 dyadicProduct(const RangeType2& arg1, const RangeType2& arg2)
{
  RangeType1 ret(0.0);
  typedef typename RangeType1::RowIterator MatrixRowIteratorType;
  typedef typename RangeType2::ConstIterator ConstVectorIteratorType;
  typedef typename RangeType2::Iterator VectorIteratorType;
  MatrixRowIteratorType rItEnd   = ret.end();
  ConstVectorIteratorType arg1It = arg1.begin();
  for (MatrixRowIteratorType rIt = ret.begin(); rIt != rItEnd; ++rIt) {
    ConstVectorIteratorType arg2It = arg2.begin();
    VectorIteratorType vItEnd = rIt->end();
    for (VectorIteratorType vIt = rIt->begin(); vIt != vItEnd; ++vIt) {
      *vIt = *arg1It * *arg2It;
      ++arg2It;
    }
    ++arg1It;
  }
  return ret;
}

/**
 *  \brief  multiplies rows of arg2 with arg1
 *  \todo   doc
 **/
template <class FieldMatrixImp>
FieldMatrixImp rowWiseMatrixMultiplication(const FieldMatrixImp& arg1, const FieldMatrixImp& arg2)
{
  typedef FieldMatrixImp FieldMatrixType;
  typedef typename FieldMatrixType::row_type RowType;
  typedef typename FieldMatrixType::ConstRowIterator ConstRowIteratorType;
  typedef typename FieldMatrixType::RowIterator RowIteratorType;

  assert(arg2.rowdim() == arg1.coldim());

  FieldMatrixType ret(0.0);

  ConstRowIteratorType arg2RowItEnd = arg2.end();
  RowIteratorType retRowItEnd       = ret.end();
  RowIteratorType retRowIt = ret.begin();
  for (ConstRowIteratorType arg2RowIt = arg2.begin(); arg2RowIt != arg2RowItEnd, retRowIt != retRowItEnd;
       ++arg2RowIt, ++retRowIt) {
    RowType row(0.0);
    arg1.mv(*arg2RowIt, row);
    *retRowIt = row;
  }
  return ret;
}

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

//! interface and base class for all out eoc tex output
template <class Info>
class TexOutputBase
{
protected:
  typedef std::vector<std::string> Strings;

  typedef TexOutputBase<Info> BaseType;

  Info info_;
  double current_h_;
  Strings headers_;


public:
  TexOutputBase(const Info& info, Strings& headers)
    : info_(info)
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  TexOutputBase(Strings& headers)
    : info_(Info())
    , current_h_(1.0)
    , headers_(headers)
  {
  }

  virtual ~TexOutputBase()
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

  virtual void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_,
                           const double prevh_, const bool /*initial*/) = 0;

  virtual void putHeader(std::ofstream& outputFile_) = 0;

  virtual void putStaticCols(std::ofstream& outputFile_) = 0;

  void endTable(std::ofstream& outputFile_)
  {
    outputFile_ << "\\end{longtable}";
    outputFile_ << info_.extra_info;
    outputFile_.flush();
  }

  double get_h()
  {
    return current_h_;
  }
};

//! format RunInfo and error vetor into a latex table, performs actual eoc calculation
class EocOutput : public TexOutputBase<RunInfo>
{
  typedef TexOutputBase<RunInfo> BaseType;

public:
  EocOutput(const RunInfo& info, BaseType::Strings& headers)
    : BaseType(info, headers)
  {
  }

  EocOutput(BaseType::Strings& headers)
    : BaseType(RunInfo(), headers)
  {
  }

  //! eoc calc happens here
  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool /*initial*/)
  {
    current_h_    = info_.grid_width;
    double factor = current_h_ / prevh_;
    double eoc = std::log(error_ / prevError_) / std::log(factor);
    if (isnan(eoc))
      outputFile_ << " & " << std::scientific << error_ << std::fixed << " & "
                  << "--";
    else
      outputFile_ << " & " << std::scientific << error_ << std::fixed << " & " << eoc;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";

    for (unsigned int i = 0; i < statColSize; i++) {
      if (i == 2)
        outputFile_ << "|r|"; // runtime col
      else
        outputFile_ << "|c|";
    }

    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\caption{" << info_.problemIdentifier << ": " << info_.gridname
                << (info_.bfg ? std::string(", BFG ($\\tau = ") + toString(info_.bfg_tau) + std::string("$ ),")
                              : std::string(", kein BFG,"))
                << "\\\\"
                << " Polorder (u,p,$ \\sigma $): (" << info_.polorder_velocity << ", " << info_.polorder_pressure
                << ", " << info_.polorder_sigma << " ) "
                << " Loeser Genauigkeit: " << info_.solver_accuracy << ", $\\alpha = " << info_.alpha
                << "$, $\\mu = " << info_.viscosity << "$}\\\\  \n"
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

  virtual void putStaticCols(std::ofstream& outputFile_)
  {
    std::stringstream runtime;
    if (info_.run_time > 59)
      runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60;
    else
      runtime << long(info_.run_time);

    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << runtime.str() << " & "
                << info_.c11.first << " / " << info_.c11.second << " & " << info_.c12.first << " / " << info_.c12.second
                << " & " << info_.d11.first << " / " << info_.d11.second << " & " << info_.d12.first << " / "
                << info_.d12.second;
  }
};

//! basically the same as EocOutput, but with less columns (no stab coefficients)
class RefineOutput : public EocOutput
{
  typedef EocOutput BaseType;

public:
  RefineOutput(const RunInfo& info, BaseType::Strings& headers)
    : BaseType(info, headers)
  {
  }

  RefineOutput(BaseType::Strings& headers)
    : BaseType(RunInfo(), headers)
  {
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    std::stringstream runtime;
    if (info_.run_time > 59)
      runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60;
    else
      runtime << long(info_.run_time);

    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << runtime.str();
  }
};

//! tex output for bfg runs
class BfgOutput : public TexOutputBase<RunInfo>
{
  typedef TexOutputBase<RunInfo> BaseType;

  RunInfo reference_;

public:
  BfgOutput(const RunInfo& info, BaseType::Strings& headers)
    : BaseType(info, headers)
  {
  }

  BfgOutput(BaseType::Strings& headers, RunInfo reference)
    : BaseType(RunInfo(), headers)
    , reference_(reference)
  {
  }

  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool /*initial*/)
  {
    // some trickery to calc correct diff w/o further work on the fem stuff
    static bool col = true;
    col             = !col;
    current_h_      = info_.grid_width;
    double diff     = error_ - reference_.L2Errors[col];
    outputFile_ << " & " << std::scientific << error_ << std::fixed << " & " << diff;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";

    for (unsigned int i = 0; i < statColSize; i++) {
      if (i == 2)
        outputFile_ << "|r|"; // runtime col
      else
        outputFile_ << "|c|";
    }

    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\caption{" << info_.problemIdentifier << ": " << info_.gridname << " Polorder (u,p,$\\sigma$): ("
                << info_.polorder_velocity << ", " << info_.polorder_pressure << ", " << info_.polorder_sigma << " ) "
                << " Loeser Genauigkeit: " << info_.solver_accuracy << "}\\\\  \n"
                << "\\hline \n";

    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << headers_[i];
      if (i < statColSize - 1)
        outputFile_ << " & ";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << " & " << headers_[i + statColSize] << " & Diff. zur Referenz";
    }
    outputFile_ << "\n \\endhead\n"
                << "\\hline\n"
                << "\\hline\n";
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    std::stringstream runtime;
    if (info_.run_time > 59)
      runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60;
    else
      runtime << long(info_.run_time);

    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << runtime.str() << " & "
                << (info_.bfg ? toString(info_.bfg_tau) : std::string("--"))
                << " & " // don't output a num in reference row
                << info_.iterations_inner_avg << " & " << info_.iterations_inner_min << " & "
                << info_.iterations_inner_max << " & " << info_.iterations_outer_total << " & " << std::scientific
                << info_.max_inner_accuracy << std::fixed;
  }
};

//! tex output for accuracy runs(where both inner and outer accuracy are varied)
class AccurracyOutput : public TexOutputBase<RunInfo>
{
  typedef TexOutputBase<RunInfo> BaseType;

public:
  AccurracyOutput(BaseType::Strings& headers)
    : BaseType(RunInfo(), headers)
  {
  }

  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool /*initial*/)
  {
    // some trickery to calc correct diff w/o further work on the fem stuff
    static bool col = true;
    col             = !col;
    current_h_      = info_.grid_width;
    outputFile_ << " & " << error_;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";

    for (unsigned int i = 0; i < statColSize; i++) {
      if (i == 2)
        outputFile_ << "|r|"; // runtime col
      else
        outputFile_ << "|c|";
    }

    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\caption{" << info_.problemIdentifier << ": " << info_.gridname << " Polorder (u,p,$\\sigma$): ("
                << info_.polorder_velocity << ", " << info_.polorder_pressure << ", " << info_.polorder_sigma << " ) "
                << "}\\\\  \n"
                << "\\hline \n";

    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << headers_[i];
      if (i < statColSize - 1)
        outputFile_ << " & ";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << " & " << headers_[i + statColSize];
    }
    outputFile_ << "\n \\endhead\n"
                << "\\hline\n"
                << "\\hline\n";
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    std::stringstream runtime;
    if (info_.run_time > 59)
      runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60;
    else
      runtime << long(info_.run_time);

    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << runtime.str() << " & "
                << info_.iterations_inner_avg << " & " << std::scientific << info_.inner_solver_accuracy << std::fixed
                << " & " << info_.iterations_outer_total << " & " << std::scientific << info_.solver_accuracy
                << std::fixed;
  }
};

//! tex output for accuracy runs(where only outer accuracy is varied)
class AccurracyOutputOuter : public TexOutputBase<RunInfo>
{
  typedef TexOutputBase<RunInfo> BaseType;

public:
  AccurracyOutputOuter(BaseType::Strings& headers)
    : BaseType(RunInfo(), headers)
  {
  }

  void putErrorCol(std::ofstream& outputFile_, const double prevError_, const double error_, const double prevh_,
                   const bool /*initial*/)
  {
    // some trickery to calc correct diff w/o further work on the fem stuff
    static bool col = true;
    col             = !col;
    current_h_      = info_.grid_width;
    outputFile_ << " & " << error_;
  }

  void putHeader(std::ofstream& outputFile_)
  {
    const unsigned int dynColSize  = 2;
    const unsigned int statColSize = headers_.size() - 2;
    outputFile_ << "\\begin{longtable}{";

    for (unsigned int i = 0; i < statColSize; i++) {
      if (i == 2)
        outputFile_ << "|r|"; // runtime col
      else
        outputFile_ << "|c|";
    }

    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << "|cc|";
    }
    outputFile_ << "}\n"
                << "\\caption{" << info_.problemIdentifier << ": " << info_.gridname << " Polorder (u,p,$\\sigma$): ("
                << info_.polorder_velocity << ", " << info_.polorder_pressure << ", " << info_.polorder_sigma << " ) "
                << "}\\\\  \n"
                << "\\hline \n";

    for (unsigned int i = 0; i < statColSize; i++) {
      outputFile_ << headers_[i];
      if (i < statColSize - 1)
        outputFile_ << " & ";
    }
    for (unsigned int i = 0; i < dynColSize; i++) {
      outputFile_ << " & " << headers_[i + statColSize];
    }
    outputFile_ << "\n \\endhead\n"
                << "\\hline\n"
                << "\\hline\n";
  }

  void putStaticCols(std::ofstream& outputFile_)
  {
    std::stringstream runtime;
    if (info_.run_time > 59)
      runtime << long(info_.run_time) / 60 << ":" << long(info_.run_time) % 60;
    else
      runtime << long(info_.run_time);

    outputFile_ << std::setw(4) << info_.grid_width << " & " << info_.codim0 << " & " << runtime.str() << " & "
                << info_.iterations_inner_avg << " & " << info_.iterations_outer_total << " & " << std::scientific
                << info_.solver_accuracy << std::fixed;
  }
};

/** stupid element-index-in-conatiner search
  \todo stl implementation?
  **/
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

long sign(long x)
{
  return long(x != 0) | (long(x >= 0) - 1);
}

//! strip filename from \path if present, return empty string if only filename present
std::string pathOnly(std::string path)
{
  if (!path.empty()) {
    char buf[1024]; // not _exactly_ sure this is max path length, but it's suggested in wx source

    // Local copy
    strcpy(buf, path.c_str());

    int l = path.length();
    int i = l - 1;

    // Search backward for a backward or forward slash
    while (i > -1) {
      if ((path[i] == '/') || (path[i] == '\\')) {
        // Don't return an empty string
        if (i == 0)
          i++;
        buf[i] = 0;
        return std::string(buf);
      }
      i--;
    }
  }
  return std::string();
}

//! return everything after the last slash
std::string filenameOnly(const std::string& path)
{
  size_t last_slash_position = path.find_last_of('/');
  return path.substr(last_slash_position + 1);
}

//! may include filename, will be stripped
bool testCreateDirectory(std::string path)
{
  std::string pathonly = pathOnly(path);
  if (pathonly.empty())
    return true; // no dir to create

  // maybe test if dir exists??
  bool ok = (mkdir(pathonly.c_str(), 0755) == 0);
  if (!ok) {
    perror(pathonly.c_str());
    return errno == EEXIST;
  }
  return true;
}

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

/** \brief a vector wrapper for continiously updating min,max,avg of some element type vector
  \todo find use? it's only used in minimal as testcase for itself...
  **/
template <class ElementType>
class MinMaxAvg
{
protected:
  typedef MinMaxAvg<ElementType> ThisType;
  typedef std::vector<ElementType> ElementsVec;
  typedef typename ElementsVec::const_iterator ElementsVecConstIterator;

public:
  MinMaxAvg()
    : min_(std::numeric_limits<ElementType>::max())
    , max_(std::numeric_limits<ElementType>::min())
    , avg_((min_ + max_) / 2.0)
  {
  }

  MinMaxAvg(const ElementsVec& elements)
  {
    for (ElementsVecConstIterator it = elements.begin(); it != elements.end(); ++it) {
      push(*it);
    }
  }

  ElementType min() const
  {
    return min_;
  }
  ElementType max() const
  {
    return max_;
  }
  ElementType average() const
  {
    return avg_;
  }

  void push(const ElementType& el)
  {
    size_t num = elements_.size();
    elements_.push_back(el);
    max_ = std::max(el, max_);
    min_ = std::min(el, min_);
    if (num > 0) {
      avg_ *= (num / double(num + 1));
      avg_ += (el / double(num + 1));
    } else {
      avg_ = el;
    }
  }

  template <class Stream>
  void output(Stream& stream)
  {
    stream << "min: " << min_ << " max: " << max_ << " avg: " << avg_ << std::endl;
  }

protected:
  ElementsVec elements_;
  ElementType min_, max_, avg_;

  MinMaxAvg(const ThisType& other);
};

//! read a file and output all lines containing filter string to a stream
template <class Stream>
void fileToStreamFiltered(Stream& stream, std::string filename, std::string filter)
{
  std::ifstream file(filename.c_str(), std::ifstream::in);
  ASSERT_EXCEPTION(file.good(), filename.c_str())
  while (file.good()) {
    std::string line;
    std::getline(file, line);
    if (line.find(filter) != std::string::npos)
      stream << line << "\n";
  }
  file.close();
}

//! output programs mem usage stats by reading from /proc
template <class Stream>
void meminfo(Stream& stream)
{
  stream << "Memory info: \n";
  stream.Resume();
  pid_t pid = getpid();
  std::stringstream filename;
  filename << "/proc/" << pid << "/status";

  fileToStreamFiltered(stream, filename.str(), "Vm");
  fileToStreamFiltered(stream, "/proc/meminfo", "Mem");
  stream << "------------ \n\n" << std::endl;
}

template <class MatrixType>
void addMatrix(MatrixType& dest, const MatrixType& arg, const double eps = 1e-14)
{
  for (int i = 0; i < arg.rows(); ++i)
    for (int j = 0; j < arg.cols(); ++j) {
      const double value = arg(i, j);
      if (std::fabs(value) > eps)
        dest.add(i, j, value);
    }
}

//! useless and/or obsolete stl wrapper?
template <class ContainerType>
void MergeVector(ContainerType& target, const ContainerType& a)
{
  target.insert(target.end(), a.begin(), a.end());
}

template <typename T>
T clamp(const T var, const T min, const T max)
{
  return ((var < min) ? min : (var > max) ? max : var);
}

} // end namepspace stuff

#endif // end of stuff.hh
