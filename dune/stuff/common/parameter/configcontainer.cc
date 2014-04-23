// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#include <config.h>
#include "configcontainer.hh"

#if HAVE_DUNE_FEM
#include <dune/stuff/common/disable_warnings.hh>
#include <dune/fem/io/parameter.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#endif

#ifndef HAVE_DUNE_FEM_PARAMETER_REPLACE
#define HAVE_DUNE_FEM_PARAMETER_REPLACE 0
#endif

#define DSC_ORDER_REL_GENERIC(var, a, b)                                                                               \
  if (a.var < b.var) {                                                                                                 \
    return true;                                                                                                       \
  }                                                                                                                    \
  if (a.var > b.var) {                                                                                                 \
    return false;                                                                                                      \
  }

#define DSC_ORDER_REL(var) DSC_ORDER_REL_GENERIC(var, (*this), other)

namespace Dune {
namespace Stuff {
namespace Common {

Request::Request(const int _line, const std::string _file, const std::string _key, const std::string _def,
                 const std::string _validator)
  : line(_line)
  , file(_file)
  , key(_key)
  , def(_def)
  , validator(_validator)
{
}

bool Request::operator<(const Request& other) const
{
  DSC_ORDER_REL(key)
  DSC_ORDER_REL(def)
  DSC_ORDER_REL(file)
  DSC_ORDER_REL(line)
  return validator < other.validator;
}

bool strictRequestCompare(const Request& a, const Request& b)
{
  DSC_ORDER_REL_GENERIC(key, a, b);
  return a.def < b.def;
}

std::ostream& operator<<(std::ostream& out, const Request& r)
{
  boost::format out_f("Request for %s with default %s in %s:%d (validation: %s)");
  out_f % r.key % r.def % r.file % r.line % r.validator;
  out << out_f.str();
  return out;
}

std::set<Request> ConfigContainer::getMismatchedDefaults(ConfigContainer::RequestMapType::value_type pair) const
{
  typedef bool (*func)(const Request&, const Request&);
  std::set<Request, func> mismatched(&strictRequestCompare);
  mismatched.insert(pair.second.begin(), pair.second.end());
  return std::set<Request>(std::begin(mismatched), std::end(mismatched));
}

ConfigContainer::ConfigContainer(const Dune::ParameterTree& tree)
  : warning_output_(false)
  , tree_(tree)
  , record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data", false)) / get("logging.dir", "log", false))
{
  testCreateDirectory(logdir_.string());
}

ConfigContainer::ConfigContainer()
  : warning_output_(true)
  , record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data", false)) / get("logging.dir", "log", false))
{
  testCreateDirectory(logdir_.string());
}

ConfigContainer::~ConfigContainer()
{
  std::unique_ptr<boost::filesystem::ofstream> out(DSC::make_ofstream(logdir_ / "dsc_parameter.log"));
  tree_.report(*out);
}


void loadIntoFemParameter(const Dune::ParameterTree& tree, const std::string pref = "")
{
#if HAVE_DUNE_FEM_PARAMETER_REPLACE
  for (auto key : tree.getValueKeys()) {
    const auto val = tree.get(key, std::string());
    key = pref + "." + key;
    Dune::Fem::Parameter::replaceKey(key, val);
  }
  for (auto subkey : tree.getSubKeys()) {
    const auto subpref = pref.empty() ? subkey : pref + "." + subkey;
    loadIntoFemParameter(tree.sub(subkey), subpref);
  }
#else
  (void)(tree);
  (void)(pref);
#endif
}


void ConfigContainer::readCommandLine(int argc, char* argv[])
{
  if (argc < 2) {
    boost::format usage("usage: %s parameter.file *[-section.key override-value]");
    DUNE_THROW(Dune::Exception, (usage % argv[0]).str());
  }
  Dune::ParameterTreeParser::readINITree(argv[1], tree_);
  Dune::ParameterTreeParser::readOptions(argc, argv, tree_);
  loadIntoFemParameter(tree_);

  // datadir and logdir may be given from the command line...
  logdir_ = boost::filesystem::path(get("global.datadir", "data", false)) / get("logging.dir", "log", false);
} // ReadCommandLine

void ConfigContainer::readOptions(int argc, char* argv[])
{
  Dune::ParameterTreeParser::readOptions(argc, argv, tree_);
}

void ConfigContainer::printRequests(std::ostream& out) const
{
  out << "Config requests:";
  for (const auto& pair : requests_map_) {
    out << "Key: " << pair.first;
    for (const auto& req : pair.second) {
      out << "\n\t" << req;
    }
    out << std::endl;
  }
}

ConfigContainer::RequestMapType ConfigContainer::getMismatchedDefaultsMap() const
{
  RequestMapType ret;
  for (const auto& pair : requests_map_) {
    auto mismatches = getMismatchedDefaults(pair);
    if (mismatches.size())
      ret[pair.first] = mismatches;
  }
  return ret;
}

void ConfigContainer::printMismatchedDefaults(std::ostream& out) const
{
  for (const auto& pair : requests_map_) {
    out << "Mismatched uses for key " << pair.first << ": ";
    for (const auto& req : getMismatchedDefaults(pair)) {
      out << "\n\t" << req;
    }
    out << "\n";
  }
}

void ConfigContainer::setRecordDefaults(bool record)
{
  record_defaults_ = record;
}

} // namespace Common
} // namespace Stuff
} // namespace Dune
