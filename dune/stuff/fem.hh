// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012 - 2015)
//   Rene Milk       (2011 - 2012, 2015)

#ifndef DUNE_STUFF_FEM_HH
#define DUNE_STUFF_FEM_HH

#if HAVE_DUNE_FEM
#define WITH_DUNE_FEM(expression) expression
#else
#define WITH_DUNE_FEM(expression)
#endif

#endif // DUNE_STUFF_FEM_HH
