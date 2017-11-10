// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Tobias Leibner (2017)

#ifndef DUNE_XT_COMMON_LAPACKE_HH
#define DUNE_XT_COMMON_LAPACKE_HH

#if HAVE_MKL
#include <mkl.h>
#elif HAVE_LAPACKE
#include <lapacke.h>
#endif

#endif // DUNE_XT_COMMON_LAPACKE_HH
