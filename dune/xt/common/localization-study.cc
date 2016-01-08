// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014)
//   Rene Milk       (2015)

#include "config.h"
#include "localization-study.hh"

#include <boost/io/ios_state.hpp>

#include <dune/xt/common/string.hh>

namespace Dune {
namespace XT {
namespace Common {

LocalizationStudy::LocalizationStudy(const std::vector<std::string> only_these_indicators)
  : only_these_indicators_(only_these_indicators)
{
}

LocalizationStudy::~LocalizationStudy()
{
}

std::vector<std::string> LocalizationStudy::used_indicators() const
{
  if (only_these_indicators_.empty())
    return provided_indicators();
  else {
    std::vector<std::string> ret;
    for (auto indicator : provided_indicators())
      if (std::find(only_these_indicators_.begin(), only_these_indicators_.end(), indicator)
          != only_these_indicators_.end())
        ret.push_back(indicator);
    return ret;
  }
} // ... used_indicators(...)

void LocalizationStudy::run(std::ostream& out)
{
  boost::io::ios_all_saver guard(out);
  if (provided_indicators().size() == 0)
    DUNE_THROW(Dune::InvalidStateException, "You have to provide at least one indicator!");
  const auto actually_used_indicators = used_indicators();
  if (actually_used_indicators.size() == 0)
    DUNE_THROW(Dune::InvalidStateException,
               "There are no common indicators in 'provided_indicators()' and 'only_these_indicators'!");

  // build table header
  out << identifier() << std::endl;
  const size_t total_width = 80;
  std::string header_line =
      std::string(" ||") + "   L^2 difference   " + "|" + "   L^oo difference  " + "|" + " standard deviation ";
  size_t first_column_size = 0;
  for (auto id : actually_used_indicators)
    first_column_size          = std::max(first_column_size, id.size());
  first_column_size            = std::max(first_column_size, total_width - header_line.size() - 1);
  std::string first_column_str = "";
  for (size_t ii = 0; ii < first_column_size; ++ii)
    first_column_str += " ";
  header_line = std::string(" ") + first_column_str + header_line;
  // print table header
  if (identifier().size() > header_line.size())
    out << whitespaceify(identifier(), '=') << "\n";
  else
    out << whitespaceify(header_line, '=') << "\n";
  out << header_line << "\n";
  const std::string thin_delimiter =
      whitespaceify(" " + first_column_str + " ", '-') + "++" + whitespaceify("   L^2 difference   ", '-') + "+"
      + whitespaceify("   L^oo difference  ", '-') + "+" + whitespaceify(" standard deviation ", '-');
  const std::string thick_delimiter =
      whitespaceify(" " + first_column_str + " ", '=') + "++" + whitespaceify("   L^2 difference   ", '=') + "+"
      + whitespaceify("   L^oo difference  ", '=') + "+" + whitespaceify(" standard deviation ", '=');
  out << thick_delimiter << std::endl;
  // comput reference indicators
  const auto reference_indicators = compute_reference_indicators();
  if (reference_indicators.size() == 0)
    DUNE_THROW(Exceptions::requirements_not_met, "Given reference indicators must not be empty!");
  // loop over all indicators
  for (size_t ind = 0; ind < actually_used_indicators.size(); ++ind) {
    const std::string indicator_id = actually_used_indicators[ind];
    // enlarge/cap id to first_column_size chars
    std::string id_str = indicator_id.empty() ? "???" : indicator_id;
    if (id_str.size() > first_column_size)
      id_str = id_str.substr(0, first_column_size);
    else if (id_str.size() < first_column_size) {
      const double missing = (double(first_column_size) - id_str.size()) / 2.0;
      for (size_t ii = 0; ii < missing; ++ii)
        id_str = " " + id_str + " ";
      if (id_str.size() == (first_column_size - 1))
        id_str = " " + id_str;
    }
    assert(id_str.size() == first_column_size);
    // print first column
    out << " " << id_str << " || " << std::flush;
    // compute indicators
    const auto indicators = compute_indicators(indicator_id);
    if (indicators.size() != reference_indicators.size())
      DUNE_THROW(Exceptions::requirements_not_met,
                 "Given indicators of type '" << indicator_id << "' are of wrong length (is " << indicators.size()
                                              << ", should be "
                                              << reference_indicators.size()
                                              << ")!");
    const auto difference = reference_indicators - indicators;
    // compute L^2 difference
    out << std::setw(18) << std::setprecision(2) << std::scientific << difference.l2_norm() << std::flush;
    // compute L^oo difference
    out << " | " << std::setw(18) << std::setprecision(2) << std::scientific << difference.sup_norm() << std::flush;
    // compute standard deviation
    out << " | " << std::setw(18) << std::setprecision(2) << std::scientific << difference.standard_deviation()
        << std::flush;
    if (ind < (actually_used_indicators.size() - 1))
      out << "\n" << thin_delimiter;
    out << std::endl;
  } // loop over all indicators
} // ... run(...)

} // namespace Common
} // namespace XT
} // namespace Dune
