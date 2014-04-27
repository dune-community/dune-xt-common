// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH
#define DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH

#include <vector>
#include <map>
#include <iostream>

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/color.hh>

namespace Dune {
namespace Stuff {
namespace Common {


class ConvergenceStudy
{
public:
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

  std::map<std::string, std::vector<double>> run(const bool relative = true, std::ostream& out = std::cout)
  {
    if (provided_norms().size() == 0)
      DUNE_THROW(Dune::InvalidStateException, "You have to provide at least one norm!");
    std::map<std::string, std::vector<double>> ret;
    for (const auto& norm : provided_norms())
      ret[norm] = std::vector<double>();

    // print table header
    out << identifier() << std::endl;
    if (identifier().size() > 22 * (provided_norms().size() + 1))
      out << Stuff::Common::whitespaceify(identifier(), '=') << std::endl;
    else {
      out << "=====================";
      for (size_t nn = 0; nn < provided_norms().size(); ++nn)
        out << "======================";
    }
    out << "\n";
    // * print line of grid and norms
    out << "        grid         ";
    for (const auto& norm : provided_norms()) {
      std::string relative_norm_str = "";
      if (norm.empty()) {
        relative_norm_str = "???";
        if (relative)
          relative_norm_str += " (relative)";
        else
          relative_norm_str += " (absolute)";
      } else if (norm.size() <= 8) {
        relative_norm_str = norm;
        if (relative)
          relative_norm_str += " (relative)";
        else
          relative_norm_str += " (absolute)";
      } else if (norm.size() <= 12) {
        relative_norm_str = norm;
        if (relative)
          relative_norm_str += " (rel.)";
        else
          relative_norm_str += " (abs.)";
      } else {
        if (relative)
          relative_norm_str = norm.substr(0, 11) + ". (rel.)";
        else
          relative_norm_str = norm.substr(0, 19);
      }
      const double missing = (19.0 - relative_norm_str.size()) / 2.0;
      for (size_t ii = 0; ii < missing; ++ii)
        relative_norm_str += " ";
      assert(relative_norm_str.size() <= 19);
      out << "| " << std::setw(19) << relative_norm_str << " ";
    }
    out << "\n";
    // * print thin delimiter
    out << "---------------------";
    for (size_t nn = 0; nn < provided_norms().size(); ++nn)
      out << "+---------------------";
    out << "\n";
    // * print size/width/error/EOC markers
    out << "     size |    width ";
    for (size_t nn = 0; nn < provided_norms().size(); ++nn)
      out << "|    error |      EOC ";
    out << "\n";
    // * print thick delimiter
    out << "==========+==========";
    for (size_t nn = 0; nn < provided_norms().size(); ++nn)
      out << "+==========+==========";
    out << std::endl;

    // prepare data structures
    std::map<std::string, double> reference_norm;
    std::map<std::string, double> last_relative_error;
    for (const auto& norm : provided_norms()) {
      if (relative)
        reference_norm[norm] = norm_reference_solution(norm);
      else
        reference_norm[norm]    = 0.0;
      last_relative_error[norm] = 0.0;
    }
    double last_grid_width = current_grid_width();

    // iterate
    for (size_t ii = 0; ii <= num_refinements(); ++ii) {
      // print delimiter
      if (ii > 0) {
        out << "----------+----------";
        for (size_t nn = 0; nn < provided_norms().size(); ++nn)
          out << "+----------+----------";
        out << "\n";
      }

      // print grid size
      out << " " << std::setw(8) << current_grid_size() << std::flush;
      // print grid with
      out << " | " << std::setw(8) << std::setprecision(2) << std::scientific << current_grid_width() << std::flush;

      // do the computation
      const double elapsed = compute_on_current_refinement();

      // loop over all norms/columns
      for (const auto& norm : provided_norms()) {
        // compute and print relative error
        double relative_error = current_error_norm(norm);
        if (relative)
          relative_error /= reference_norm[norm];
        ret[norm].push_back(relative_error);
        out << " | " << std::setw(8) << std::setprecision(2) << std::scientific << relative_error << std::flush;
        // print eoc
        out << " | ";
        if (ii == 0)
          out << std::setw(8) << "----" << std::flush;
        else {
          const double eoc_value =
              std::log(relative_error / last_relative_error[norm]) / std::log(current_grid_width() / last_grid_width);
          std::stringstream eoc_string;
          eoc_string << std::setw(8) << std::setprecision(2) << std::fixed << eoc_value;
          if (eoc_value > (0.9 * expected_rate(norm)))
            out << Stuff::Common::colorString(eoc_string.str(), Stuff::Common::Colors::green) << std::flush;
          else if (eoc_value > 0.0)
            out << Stuff::Common::colorString(eoc_string.str(), Stuff::Common::Colors::brown) << std::flush;
          else
            out << Stuff::Common::colorString(eoc_string.str(), Stuff::Common::Colors::red) << std::flush;
        }
        // update
        last_relative_error[norm] = relative_error;
      } // loop over all norms/columns
      // print time
      if (elapsed < 1.0)
        out << "  (solve took " << int(1000 * elapsed) << "ms)" << std::endl;
      else
        out << "  (solve took " << std::setprecision(2) << std::fixed << elapsed << "s)" << std::endl;
      // update
      last_grid_width = current_grid_width();
      if (ii < num_refinements())
        refine();
    } // iterate

    return ret;
  } // ... run(...)
}; // class ConvergenceStudy


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_CONVERGENCE_STUDY_HH
