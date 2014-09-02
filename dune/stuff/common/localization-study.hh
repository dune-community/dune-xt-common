// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_LOCALIZATION_STUDY_HH
#define DUNE_STUFF_COMMON_LOCALIZATION_STUDY_HH

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/la/container/common.hh>

namespace Dune {
namespace Stuff {
namespace Common {


class LocalizationStudy
{
public:
  LocalizationStudy(const std::vector<std::string> only_these_indicators = {});

  virtual ~LocalizationStudy();

  virtual std::string identifier() const = 0;

  virtual LA::CommonDenseVector<double> compute_reference_indicators() const = 0;

  virtual std::vector<std::string> provided_indicators() const = 0;

  virtual LA::CommonDenseVector<double> compute_indicators(const std::string type) const = 0;

  std::vector<std::string> used_indicators() const;

  /*std::map< std::string, std::vector< double > >*/ void run(std::ostream& out = std::cout);

private:
  const std::vector<std::string> only_these_indicators_;
}; // class LocalizationStudy


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_LOCALIZATION_STUDY_HH
