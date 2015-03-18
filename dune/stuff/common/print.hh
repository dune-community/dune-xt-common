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

namespace Dune {
namespace Stuff {
namespace Common {

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
} // dimToAxisName

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // ifndef DUNESTUFF_PRINTING_HH_INCLUDED
