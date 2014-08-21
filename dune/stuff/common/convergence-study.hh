// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH
#define DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH

#include <vector>
#include <map>
#include <string>
#include <ostream>

namespace Dune {
namespace Stuff {
namespace Common {


class ConvergenceStudy
{
public:
  ConvergenceStudy(const std::vector<std::string> only_use_this_norms = {});

  virtual ~ConvergenceStudy()
  {
  }

  virtual std::string identifier() const = 0;

  virtual size_t num_refinements() const = 0;

  virtual std::vector<std::string> provided_norms() const = 0;

  virtual size_t expected_rate(const std::string type) const = 0;

  virtual double norm_reference_solution(const std::string type) = 0;

  virtual size_t current_grid_size() const = 0;

  virtual double current_grid_width() const = 0;

  /**
   * \return The time it took to solve on this refinement.
   */
  virtual double compute_on_current_refinement() = 0;

  virtual double current_error_norm(const std::string type) = 0;

  virtual void refine() = 0;

  std::map<std::string, std::vector<double>> run(const bool relative = true, std::ostream& out = std::cout);

private:
  std::vector<std::string> only_use_this_norms_;
}; // class ConvergenceStudy


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH
