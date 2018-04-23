// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012 - 2014, 2016 - 2018)
//   Rene Milk       (2011 - 2012, 2015 - 2016, 2018)
//   Tobias Leibner  (2017 - 2018)

#ifndef DUNE_XT_COMMON_LPSOLVE_HH
#define DUNE_XT_COMMON_LPSOLVE_HH

#include <stdio.h>

#include <complex>
#include <memory>

namespace Dune {
namespace XT {
namespace Common {
namespace lp_solve {


// forward declaration
struct LinearProgram;


/**
 * \brief If true, calling any of the other methods makes sense.
 */
bool available();


/**
 * \brief Wrapper around lp_solve's EQ
 */
int eq();


/**
 * \brief Wrapper around lp_solve's IMPORTANT
 */
int important();


/**
 * \brief Wrapper around lp_solve's OPTIMAL
 */
int optimal();


/**
 * \brief Wrapper around lp_solve's get_objective
 */
double get_objective(LinearProgram& lp);


/**
 * \brief Create a LinearProgram object (wrapper around lpsolve's lprec)
 */
std::unique_ptr<LinearProgram> make_lp(int rows, int cols);


/**
 * \brief Wrapper around lp_solve's set_bounds
 */
bool set_bounds(LinearProgram& lp, int colnr, double lower, double upper);


/**
 * \brief Wrapper around lp_solve's set_col_name
 */
bool set_col_name(LinearProgram& lp, int colnr, std::string& colname);


/**
 * \brief Wrapper around lp_solve's set_column
 */
bool set_column(LinearProgram& lp, int colnr, double* column);


/**
 * \brief Wrapper around lp_solve's set_constr_type
 */
bool set_constr_type(LinearProgram& lp, int rownr, int con_type);


/**
 * \brief Wrapper around lp_solve's set_rh
 */
bool set_rh(LinearProgram& lp, int rownr, double column);


/**
 * \brief Wrapper around lp_solve's set_rh_vec
 */
void set_rh_vec(LinearProgram& lp, double* rh);


/**
 * \brief Wrapper around lp_solve's set_verbose
 */
void set_verbose(LinearProgram& lp, int verbose);


/**
 * \brief Wrapper around lp_solve's solve
 */
int solve(LinearProgram& lp);


/**
 * \brief Wrapper around lp_solve's write_LP
 */
bool write_LP(LinearProgram& lp, FILE* output);


} // namespace lp_solve
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_LPSOLVE_HH
