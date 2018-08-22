// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   Rene Milk       (2015 - 2016, 2018)

#include "config.h"
#include "convergence-study.hh"

#include <algorithm>
#include <boost/io/ios_state.hpp>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/string.hh>
#include <dune/xt/common/color.hh>

namespace Dune {
namespace XT {
namespace Common {

ConvergenceStudy::ConvergenceStudy(const std::vector<std::string> only_these_norms)
  : only_these_norms_(only_these_norms)
{
}

std::vector<std::string> ConvergenceStudy::used_norms() const
{
  if (only_these_norms_.empty())
    return provided_norms();
  else {
    std::vector<std::string> ret;
    for (auto norm : provided_norms())
      if (std::find(only_these_norms_.begin(), only_these_norms_.end(), norm) != only_these_norms_.end())
        ret.push_back(norm);
    return ret;
  }
} // ... used_norms(...)

std::map<std::string, std::vector<double>>
ConvergenceStudy::run(const bool relative, std::ostream& out, const bool print_timings)
{
  boost::io::ios_all_saver guard(out);
  if (provided_norms().size() == 0)
    DUNE_THROW(Dune::InvalidStateException, "You have to provide at least one norm!");
  const auto actually_used_norms = used_norms();
  if (actually_used_norms.size() == 0)
    DUNE_THROW(Dune::InvalidStateException, "There are no common norms in 'provided_norms()' and 'only_these_norms'!");

  std::map<std::string, std::vector<double>> ret;
  for (const auto& norm : actually_used_norms)
    ret[norm] = std::vector<double>();

  // print table header
  out << identifier() << std::endl;
  if (identifier().size() > 22 * (actually_used_norms.size() + 1))
    out << whitespaceify(identifier(), '=') << std::endl;
  else {
    out << "=====================";
    for (size_t nn = 0; nn < actually_used_norms.size(); ++nn)
      out << "======================";
  }
  out << "\n";
  // * print line of grid and norms
  out << "        grid         ";
  for (const auto& norm : actually_used_norms) {
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
    DXT_ASSERT(relative_norm_str.size() <= 19);
    out << "| " << std::setw(19) << relative_norm_str << " ";
  }
  out << "\n";
  // * print thin delimiter
  out << "---------------------";
  for (size_t nn = 0; nn < actually_used_norms.size(); ++nn)
    out << "+---------------------";
  out << "\n";
  // * print size/width/error/EOC markers
  out << "     size |    width ";
  for (size_t nn = 0; nn < actually_used_norms.size(); ++nn)
    out << "|    error |      EOC ";
  out << "\n";
  // * print thick delimiter
  out << "==========+==========";
  for (size_t nn = 0; nn < actually_used_norms.size(); ++nn)
    out << "+==========+==========";
  out << std::endl;

  // prepare data structures
  std::map<std::string, double> reference_norm;
  std::map<std::string, double> last_relative_error;
  for (const auto& norm : actually_used_norms) {
    if (relative)
      reference_norm[norm] = norm_reference_solution(norm);
    else
      reference_norm[norm] = 0.0;
    last_relative_error[norm] = 0.0;
  }
  double last_grid_width = current_grid_width();

  // iterate
  for (size_t ii = 0; ii <= num_refinements(); ++ii) {
    // print delimiter
    if (ii > 0) {
      out << "----------+----------";
      for (size_t nn = 0; nn < actually_used_norms.size(); ++nn)
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
    for (const auto& norm : actually_used_norms) {
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
          out << color_string(eoc_string.str(), Colors::green) << std::flush;
        else if (eoc_value > 0.0)
          out << color_string(eoc_string.str(), Colors::brown) << std::flush;
        else
          out << color_string(eoc_string.str(), Colors::red) << std::flush;
      }
      // update
      last_relative_error[norm] = relative_error;
    } // loop over all norms/columns
    // print time
    if (print_timings) {
      if (elapsed < 1.0)
        out << "  (solve took " << ssize_t(1000 * elapsed) << "ms)";
      else
        out << "  (solve took " << std::setprecision(2) << std::fixed << elapsed << "s)";
    }
    out << std::endl;
    // update
    last_grid_width = current_grid_width();
    if (ii < num_refinements())
      refine();
  } // iterate

  return ret;
} // ... run(...)

std::vector<double> ConvergenceStudy::expected_results(const std::string /*type*/) const
{
  DUNE_THROW(Exceptions::you_have_to_implement_this, "If you want to use this within the test suite!");
}

} // namespace Common
} // namespace XT
} // namespace Dune
