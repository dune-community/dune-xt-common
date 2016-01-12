// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2010, 2012 - 2015)
//   Rene Milk       (2009 - 2015)
//   Sven Kaulmann   (2011 - 2012)
//   Tobias Leibner  (2014)

#ifndef DUNE_XT_COMMON_PRINT_HH
#define DUNE_XT_COMMON_PRINT_HH

#include <string>
#include <iostream>

#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/ranges.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/vector.hh>
#include <dune/stuff/common/matrix.hh>

namespace Dune {
namespace Stuff {
namespace Common {

template <class V>
typename std::enable_if<Dune::Stuff::Common::is_vector<V>::value, void>::type
print(const V& vec, const std::string name, std::ostream& out, const std::string prefix = "")
{
  out << prefix << name << " = ";
  if (vec.size() == 0)
    out << "[]";
  else if (vec.size() == 1)
    out << vec[0];
  else {
    out << "[" << vec[0];
    for (decltype(vec.size()) ii = 1; ii < vec.size(); ++ii)
      out << " " << vec[ii];
    out << "]";
  }
} // ... print(...)

template <class M>
typename std::enable_if<Dune::Stuff::Common::is_matrix<M>::value, void>::type
print(const M& mat, const std::string name, std::ostream& out, const std::string prefix = "")
{
  typedef MatrixAbstraction<M> MM;
  const size_t rows = MM::rows(mat);
  const size_t cols = MM::cols(mat);
  out << prefix << name << " = ";
  if (rows == 0 && cols == 0)
    out << "[]";
  else if (rows == 1) {
    out << "[" << MM::get_entry(mat, 0, 0);
    for (size_t jj = 1; jj < cols; ++jj)
      out << " " << MM::get_entry(mat, 0, jj);
    out << "]";
  } else if (cols == 1) {
    out << "[" << MM::get_entry(mat, 0, 0);
    for (size_t ii = 1; ii < rows; ++ii)
      out << " " << MM::get_entry(mat, ii, 0);
    out << "]";
  } else {
    out << "[";
    auto local_prefix = prefix + " ";
    for (size_t ii = 0; ii < rows - 1; ++ii) {
      out << MM::get_entry(mat, ii, 0);
      for (size_t jj = 1; jj < cols; ++jj)
        out << " " << MM::get_entry(mat, ii, jj);
      out << ";\n" << local_prefix;
    }
    out << MM::get_entry(mat, rows - 1, 0);
    for (size_t jj = 1; jj < cols; ++jj)
      out << " " << MM::get_entry(mat, rows - 1, jj);
    out << "]";
  }
} // ... print(...)

//! useful for visualizing sparsity patterns of matrices
template <class Matrix>
void matrixToGnuplotStream(const Matrix& matrix, std::ostream& stream)
{
  unsigned long nz = 0;

  const auto cols = matrix.cols();
  for (auto row : valueRange(matrix.rows())) {
    for (auto col : valueRange(cols)) {
      if (matrix.find(row, col))
        stream << row << "\t" << col << "\t" << matrix(row, col) << "\n";
    }
    nz += matrix.numNonZeros(row);
    stream << "#non zeros in row " << row << " " << matrix.numNonZeros(row) << " (of " << matrix.cols() << " cols)\n";
  }
  stream << "#total non zeros " << nz << " of " << matrix.rows() * matrix.cols() << " entries\n";
} // matrixToGnuplotStream

//! maps 1,2,3 to x,y,z / X,Y,Z
inline std::string dimToAxisName(const size_t dim, const bool capitalize = false)
{
  char c = 'x';

  c += dim;
  if (capitalize)
    c -= 32;
  return std::string() += c;
} // matrixToGnuplotStream

/** Outputiterator to emulate python's str.join(iterable)
 * \see http://codereview.stackexchange.com/questions/30132/comma-formatted-stl-vectors/30181#30181
 * \example std::copy(strings.begin(), strings.end(), PrefixOutputIterator<string>(ostream, ","));
 **/
template <typename T>
class PrefixOutputIterator
{
  std::ostream& ostream;
  std::string prefix;
  bool first;

public:
  typedef std::size_t difference_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T reference;
  typedef std::output_iterator_tag iterator_category;

  PrefixOutputIterator(std::ostream& o, std::string const& p = "")
    : ostream(o)
    , prefix(p)
    , first(true)
  {
  }

  PrefixOutputIterator& operator*()
  {
    return *this;
  }
  PrefixOutputIterator& operator++()
  {
    return *this;
  }
  PrefixOutputIterator& operator++(int)
  {
    return *this;
  }

  void operator=(T const& value)
  {
    if (first) {
      ostream << value;
      first = false;
    } else {
      ostream << prefix << value;
    }
  }
};

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_XT_COMMON_PRINT_HH
