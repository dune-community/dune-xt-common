// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#include "config.h"
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
  if (mismatched.size() <= std::size_t(1))
    return *(new std::set<Request>);
  else
    return std::set<Request>(std::begin(mismatched), std::end(mismatched));
}

// Constructors and destructors for ConfigContainer
ConfigContainer::ConfigContainer(const Dune::ParameterTree& tree)
  : tree_(tree)
  , record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data")) / get("logging.dir", "log"))
#ifndef NDEBUG
  , warning_output_(false)
#endif
{
  testCreateDirectory(logdir_.string());
}

ConfigContainer::ConfigContainer()
  : record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data")) / get("logging.dir", "log"))
#ifndef NDEBUG
  , warning_output_(true)
#endif
{
  testCreateDirectory(logdir_.string());
}

ConfigContainer::ConfigContainer(const std::string key, const char* value)
  : record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data")) / get("logging.dir", "log"))
#ifndef NDEBUG
  , warning_output_(true)
#endif
{
  set(key, value);
}

ConfigContainer::ConfigContainer(const std::vector<std::string> keys,
                                 const std::initializer_list<std::string> value_list)
  : record_defaults_(false)
  , logdir_(boost::filesystem::path(get("global.datadir", "data", 0, 0)) / get("logging.dir", "log", 0, 0))
#ifndef NDEBUG
  , warning_output_(true)
#endif
{
  std::vector<std::string> tmp_values(value_list);
  if (keys.size() != tmp_values.size())
    DUNE_THROW(Exceptions::shapes_do_not_match,
               "The size of 'keys' (" << keys.size() << ") does not match the size of 'value_list' ("
                                      << tmp_values.size()
                                      << ")!");
  for (size_t ii = 0; ii < keys.size(); ++ii)
    set(keys[ii], tmp_values[ii]);
}

ConfigContainer::ConfigContainer(const std::string filename)
  : ConfigContainer::ConfigContainer(initialize(filename))
{
}

ConfigContainer::ConfigContainer(int argc, char** argv)
  : ConfigContainer::ConfigContainer(initialize(argc, argv))
{
}

ConfigContainer::ConfigContainer(int argc, char** argv, const std::string filename)
  : ConfigContainer::ConfigContainer(initialize(argc, argv, filename))
{
}

ConfigContainer::~ConfigContainer()
{
  std::unique_ptr<boost::filesystem::ofstream> out(DSC::make_ofstream(logdir_ / "dsc_parameter.log"));
  tree_.report(*out);
}

/**
 * \brief Load all key-value pairs from tree into fem parameter database.
 * \param tree ParameterTree to get values from.
 * \param pref Prefix that is added to each key and subkey (like this: pref.key), default is "".
 */
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

// method definitions for ConfigContainer
bool ConfigContainer::has_key(const std::string& key) const
{
  return tree_.hasKey(key);
} // ... has_key(...)

const ParameterTree ConfigContainer::sub(const std::string subTreeName) const
{
  tree_.assertSub(subTreeName);
  return tree_.sub(subTreeName);
} // ... sub(...)

std::string& ConfigContainer::operator[](std::string key)
{
  return tree_[key];
} // ... operator[](...)

bool ConfigContainer::has_sub(const std::string subTreeName) const
{
  return tree_.hasSub(subTreeName);
} // ... has_sub(...)

void ConfigContainer::add(const ConfigContainer& other, const std::string sub_id /*= ""*/,
                          const bool overwrite /* = false*/)
{
  tree_.add(other.tree(), sub_id, overwrite);
  for (auto pair : other.requests_map_)
    for (auto request : pair.second)
      requests_map_[pair.first].insert(request);
} // ... add(...)

void ConfigContainer::add(const ParameterTree& paramtree, const std::string sub_id /* = ""*/,
                          const bool overwrite /* = false*/)
{
  tree_.add(paramtree, sub_id, overwrite);
} // ... add(...)

ConfigContainer& ConfigContainer::operator+=(ConfigContainer& other)
{
  add(other);
  return *this;
} // ... operator+=

ConfigContainer ConfigContainer::operator+(ConfigContainer& other)
{
  ConfigContainer ret(*this);
  ret += other;
  return ret;
} // ... operator+

ConfigContainer& ConfigContainer::operator=(const ConfigContainer& other)
{
  tree_ = other.tree_;
#ifndef NDEBUG
  warning_output_ = other.warning_output_;
#endif
  requests_map_    = other.requests_map_;
  record_defaults_ = other.record_defaults_;
  logdir_          = other.logdir_;
  return *this;
} // ... operator=(...)

ExtendedParameterTree ConfigContainer::tree() const
{
  return tree_;
} // ... tree()

// RequestMapType ConfigContainer::requests_map() const {
//  return requests_map_;
//} // ... requests_map()

bool ConfigContainer::empty() const
{
  return tree_.getValueKeys().empty() && tree_.getSubKeys().empty();
} // ... empty()

void ConfigContainer::report(std::ostream& out /* = std::cout*/, const std::string& prefix /* = ""*/) const
{
  printRequests(out);
  if (!empty()) {
    if (tree_.getSubKeys().size() == 0) {
      tree_.reportAsSub(out, prefix, "");
    } else if (tree_.getValueKeys().size() == 0) {
      const std::string common_prefix = tree_.findCommonPrefix(tree_, "");
      if (!common_prefix.empty()) {
        out << prefix << "[" << common_prefix << "]" << std::endl;
        const ConfigContainer& commonSub(sub(common_prefix));
        tree_.reportFlatly(commonSub.tree(), prefix, out);
      } else
        tree_.reportAsSub(out, prefix, "");
    } else {
      tree_.reportAsSub(out, prefix, "");
    }
  }
} // ... report(...)

std::string ConfigContainer::report_string(const std::string& prefix /* = ""*/) const
{
  std::stringstream stream;
  report(stream, prefix);
  return stream.str();
} // ... report_string(...)

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
  logdir_ = boost::filesystem::path(get("global.datadir", "data")) / get("logging.dir", "log");
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
    auto mismatched = getMismatchedDefaults(pair);
    if (mismatched.size()) {
      out << "Mismatched uses for key " << pair.first << ": ";
      for (const auto& req : mismatched) {
        out << "\n\t" << req;
      }
      out << "\n";
    }
  }
}

void ConfigContainer::setRecordDefaults(bool record)
{
  record_defaults_ = record;
}

ParameterTree ConfigContainer::initialize(const std::string filename)
{
  ParameterTree param_tree;
  Dune::ParameterTreeParser::readINITree(filename, param_tree);
  return param_tree;
} // ... initialize(...)

ParameterTree ConfigContainer::initialize(int argc, char** argv)
{
  ParameterTree param_tree;
  if (argc == 2) {
    Dune::ParameterTreeParser::readINITree(argv[1], param_tree);
  } else if (argc > 2) {
    Dune::ParameterTreeParser::readOptions(argc, argv, param_tree);
  }
  if (param_tree.hasKey("paramfile")) {
    Dune::ParameterTreeParser::readINITree(param_tree.get<std::string>("paramfile"), param_tree, false);
  }
  return param_tree;
} // ... initialize(...)

ParameterTree ConfigContainer::initialize(int argc, char** argv, std::string filename)
{
  ParameterTree param_tree;
  if (argc == 1) {
    Dune::ParameterTreeParser::readINITree(filename, param_tree);
  } else if (argc == 2) {
    Dune::ParameterTreeParser::readINITree(argv[1], param_tree);
  } else {
    Dune::ParameterTreeParser::readOptions(argc, argv, param_tree);
  }
  if (param_tree.hasKey("paramfile")) {
    Dune::ParameterTreeParser::readINITree(param_tree.get<std::string>("paramfile"), param_tree, false);
  }
  return param_tree;
} // ... initialize(...)


} // namespace Common
} // namespace Stuff
} // namespace Dune
