// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014, 2016)
//   Rene Milk       (2015)

#ifndef DUNE_XT_COMMON_LOCALIZATION_STUDY_HH
#define DUNE_XT_COMMON_LOCALIZATION_STUDY_HH

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <dune/common/dynvector.hh>

#include <dune/xt/common/exceptions.hh>

namespace Dune {
namespace XT {
namespace Common {

class LocalizationStudy
{
public:
  LocalizationStudy(const std::vector<std::string> only_these_indicators = {});

  virtual ~LocalizationStudy();

  virtual std::string identifier() const = 0;

  virtual DynamicVector<double> compute_reference_indicators() const = 0;

  virtual std::vector<std::string> provided_indicators() const = 0;

  virtual DynamicVector<double> compute_indicators(const std::string type) const = 0;

  std::vector<std::string> used_indicators() const;

  /*std::map< std::string, std::vector< double > >*/ void run(std::ostream& out = std::cout);

private:
  const std::vector<std::string> only_these_indicators_;
}; // class LocalizationStudy

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_LOCALIZATION_STUDY_HH
