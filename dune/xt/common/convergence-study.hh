// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2013 - 2014, 2016)
//   Rene Milk       (2014 - 2015)

#ifndef DUNE_XT_COMMON_CONVERGENCE_STUDY_HH
#define DUNE_XT_COMMON_CONVERGENCE_STUDY_HH

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <dune/xt/common/logging.hh>

namespace Dune {
namespace XT {
namespace Common {

class ConvergenceStudy
{
public:
  ConvergenceStudy(const std::vector<std::string> only_these_norms = {});

  virtual ~ConvergenceStudy()
  {
  }

  virtual std::string identifier() const = 0;

  virtual size_t num_refinements() const = 0;

  virtual std::vector<std::string> provided_norms() const = 0;

  virtual size_t expected_rate(const std::string type) const = 0;

  virtual double norm_reference_solution(const std::string type) = 0;

  virtual size_t current_num_DoFs() = 0;

  virtual size_t current_grid_size() const = 0;

  virtual double current_grid_width() = 0;

  /**
   * \return The time it took to solve on this refinement.
   */
  virtual double compute_on_current_refinement() = 0;

  virtual double current_error_norm(const std::string type) = 0;

  virtual void refine() = 0;

  std::vector<std::string> used_norms() const;

  std::map<std::string, std::vector<double>> run(const bool relative = false, std::ostream& out = DXTC_LOG_INFO_0,
                                                 const bool print_timings = true);

  virtual std::vector<double> expected_results(const std::string /*type*/) const;

private:
  std::vector<std::string> only_these_norms_;
}; // class ConvergenceStudy

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_CONVERGENCE_STUDY_HH
