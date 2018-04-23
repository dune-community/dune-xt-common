// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017 - 2018)
//   Rene Milk       (2018)
//   Tobias Leibner  (2018)

#include "config.h"

#if HAVE_LPSOLVE
#include <dune/xt/common/disable_warnings.hh>
#include <lpsolve/lp_lib.h>
#include <dune/xt/common/reenable_warnings.hh>
#endif

#include <dune/common/unused.hh>

#include <dune/xt/common/exceptions.hh>

#include "lpsolve.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace lp_solve {


#if HAVE_LPSOLVE
struct LinearProgram
{
  LinearProgram(int rows, int cols)
    : lp_(::make_lp(rows, cols))
  {
    if (!lp_)
      DUNE_THROW(Dune::MathError, "Couldn't construct linear program");
  }

  ~LinearProgram()
  {
    ::delete_lp(lp_);
  }

  lprec* data()
  {
    return lp_;
  }

private:
  lprec* lp_;
};
#else // HAVE_LPSOLVE
struct LinearProgram
{
  LinearProgram(int /*rows*/, int /*cols*/)
  {
    DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  }
};
#endif // HAVE_LPSOLVE

bool available()
{
#if HAVE_LPSOLVE
  return true;
#else
  return false;
#endif
}


int eq()
{
#if HAVE_LPSOLVE
  return EQ;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


int important()
{
#if HAVE_LPSOLVE
  return IMPORTANT;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


int optimal()
{
#if HAVE_LPSOLVE
  return OPTIMAL;
#else
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


double get_objective(LinearProgram& lp)
{
#if HAVE_LPSOLVE
  return ::get_objective(lp.data());
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


std::unique_ptr<LinearProgram> make_lp(int rows, int cols)
{
  return std::make_unique<LinearProgram>(rows, cols);
}


bool set_bounds(LinearProgram& lp, int colnr, double lower, double upper)
{
#if HAVE_LPSOLVE
  auto ret = ::set_bounds(lp.data(), colnr, lower, upper);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(colnr);
  DUNE_UNUSED_PARAMETER(lower);
  DUNE_UNUSED_PARAMETER(upper);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


bool set_col_name(LinearProgram& lp, int colnr, std::string& colname)
{
#if HAVE_LPSOLVE
  auto ret = ::set_col_name(lp.data(), colnr, &colname[0]);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(colnr);
  DUNE_UNUSED_PARAMETER(colname);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


bool set_column(LinearProgram& lp, int colnr, double* column)
{
#if HAVE_LPSOLVE
  auto ret = ::set_column(lp.data(), colnr, column);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(colnr);
  DUNE_UNUSED_PARAMETER(column);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


bool set_constr_type(LinearProgram& lp, int rownr, int con_type)
{
#if HAVE_LPSOLVE
  auto ret = ::set_constr_type(lp.data(), rownr, con_type);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(rownr);
  DUNE_UNUSED_PARAMETER(con_type);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


bool set_rh(LinearProgram& lp, int rownr, double column)
{
#if HAVE_LPSOLVE
  auto ret = ::set_rh(lp.data(), rownr, column);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(rownr);
  DUNE_UNUSED_PARAMETER(column);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


void set_rh_vec(LinearProgram& lp, double* rh)
{
#if HAVE_LPSOLVE
  ::set_rh_vec(lp.data(), rh);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(rh);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


void set_verbose(LinearProgram& lp, int verbose)
{
#if HAVE_LPSOLVE
  ::set_verbose(lp.data(), verbose);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(verbose);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


int solve(LinearProgram& lp)
{
#if HAVE_LPSOLVE
  return ::solve(lp.data());
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


bool write_LP(LinearProgram& lp, FILE* output)
{
#if HAVE_LPSOLVE
  auto ret = ::write_LP(lp.data(), output);
  assert(ret == 0 || ret == 1);
  return static_cast<bool>(ret);
#else
  DUNE_UNUSED_PARAMETER(lp);
  DUNE_UNUSED_PARAMETER(output);
  DUNE_THROW(Exceptions::dependency_missing, "You are missing lp_solve, check available() first!");
  return 1;
#endif
}


} // namespace lp_solve
} // namespace Common
} // namespace XT
} // namespace Dune
