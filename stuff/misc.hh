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
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>

#define HAS_RUN_INFO

struct RunInfo // define this beforepass is included so it's known in pass, i know it's ugly
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

  RunInfo()
  {
    grid_width = refine_level = run_time = codim0 = polorder_velocity = polorder_pressure = polorder_sigma = bfg =
        solver_accuracy = inner_solver_accuracy = bfg_tau = iterations_inner_avg = iterations_inner_min =
            iterations_inner_max = iterations_outer_total = max_inner_accuracy = -1;
    gridname = problemIdentifier = "UNSET";
    extra_info = "";
  }
};

namespace Stuff {

/**
 *  \todo   doc
 **/
template <class FieldMatrixImp>
double colonProduct(const FieldMatrixImp& arg1, const FieldMatrixImp& arg2)
{
  assert(arg1.rowdim() == arg2.coldim());
  double ret = 0.0;
  // iterators
  typedef typename FieldMatrixImp::ConstRowIterator ConstRowIteratorType;
  typedef typename FieldMatrixImp::row_type::ConstIterator ConstIteratorType;
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
                << " Loeser Genauigkeit: " << info_.solver_accuracy << "}\\\\  \n"
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


/**
 *  \brief Only free mem pointed to by valid pointer, log warning otherwise
 *
 **/

template <class T>
void safe_delete(T t) // this is actually bullshit :P
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

} // end namepspace stuff

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

#endif // end of stuff.hh
