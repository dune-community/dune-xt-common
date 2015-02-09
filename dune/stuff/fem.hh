// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FEM_HH
#define DUNE_STUFF_FEM_HH

#if HAVE_DUNE_FEM
#define WITH_DUNE_FEM(expression) expression
#else
#define WITH_DUNE_FEM(expression)
#endif

#endif // DUNE_STUFF_FEM_HH
