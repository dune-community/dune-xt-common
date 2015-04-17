// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNESTUFF_PRINTING_HH_INCLUDED
#define DUNESTUFF_PRINTING_HH_INCLUDED

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


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // ifndef DUNESTUFF_PRINTING_HH_INCLUDED
