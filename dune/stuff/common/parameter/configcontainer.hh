// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
#define DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED

#include <dune/common/deprecated.hh>
#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/parametertree.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/exceptions.hh>

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/misc.hh>
#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/fem/namespace.hh>

#include <boost/format.hpp>
#include <set>


namespace Dune {
namespace Stuff {
namespace Common {

//! use this to record defaults, placements and so forth
class Request
{
  int line;
  std::string file;
  std::string key;
  std::string def;
  std::string validator;

public:
  Request(const int _line, const std::string _file, const std::string _key, const std::string _def,
          const std::string _validator);

  //! requests are considered
  bool operator<(const Request& other) const;

  friend bool strictRequestCompare(const Request& a, const Request& b);
  friend std::ostream& operator<<(std::ostream& out, const Request& r);
};

bool strictRequestCompare(const Request& a, const Request& b);

std::ostream& operator<<(std::ostream& out, const Request& r);

class InvalidParameter : public Dune::Exception
{
};

class ConfigContainer
{
private:
  typedef std::map<std::string, std::set<Request>> RequestMapType;

  //! get value from tree and validate with validator
  template <typename T, class Validator>
  T get_valid_value(std::string name, T def, const ValidatorInterface<T, Validator>& validator, const size_t size,
                    const size_t cols) const
  {
    std::string valstring = tree_.get(name, toString(def));
    T val = fromString<T>(valstring, size, cols);
    if (validator(val))
      return val;
    std::stringstream ss;
    validator.print(ss);
    DUNE_THROW(InvalidParameter, ss.str());
  }

  //! return a set of Request objects for keys that have been queried with non-matching default values
  std::set<Request> getMismatchedDefaults(RequestMapType::value_type pair) const;

  /** \brief all public get signatures call this one
   *  \param size Determines the size of the returning container (size if T is a vector type, rows if T is a matrix
   * type, 0 means automatic).
   *  \param cols Determines the number of columns of the returning matrix if T is a matrix type (0 means automatic,
   * ignored, if T is a vector type).
   */
  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator,
        bool UNUSED_UNLESS_DEBUG(useDbgStream), const Request& request, const size_t size, const size_t cols,
        const bool def_provided = true)
  {
    requests_map_[name].insert(request);
#ifndef NDEBUG
    if (warning_output_ && !tree_.hasKey(name)) {
      if (useDbgStream)
        Logger().debug() << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
      else
        std::cerr << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
    }
#endif // ifndef NDEBUG
    if (record_defaults_ && !tree_.hasKey(name) && def_provided)
      set(name, def);
    return get_valid_value(name, def, validator, size, cols);
  } // get

public:
  // Constructors and destructors
  //! copy tree to tree_, set warning_output and record_defaults to false, set logdir_ to ???
  ConfigContainer(const ParameterTree& tree);

  //! warning_output = true, record_defaults = false, logdir_ = ???
  ConfigContainer();

  //! warning output = true, record_defaults = false, logdir_ = ???, tree_[key] = value
  template <class T>
  ConfigContainer(const std::string key, const T& value);

  //! warning output = true, record_defaults = false, logdir_ = ???, tree_[key] = value
  template <class T>
  ConfigContainer(const std::string key, const char* value);

  //! warning output = true, record_defaults = false, logdir_ = ???, tree_[keys[ii]] = values[ii] for 0 <= ii <=
  //! keys.size()
  template <class T>
  ConfigContainer(const std::vector<std::string> keys, const std::vector<T> values_in);

  /** creates std::vector< T > from value_list and then behaves exactly like
   * ConfigContainer(const std::vector< std::string > keys, const std::vector< T > values_in) */
  template <class T>
  ConfigContainer(const std::vector<std::string> keys, const std::initializer_list<T> value_list);

  ConfigContainer(const std::vector<std::string> keys, const std::initializer_list<std::string> value_list);

  //! read ParameterTree from file and call ConfigContainer(const ParameterTree& tree)
  explicit ConfigContainer(const std::string filename);

  //! read ParameterTree from given arguments and call ConfigContainer(const ParameterTree& tree)
  ConfigContainer(int argc, char** argv);

  //! read ParameterTree from given arguments and file and call ConfigContainer(const ParameterTree& tree)
  ConfigContainer(int argc, char** argv, const std::string filename);

  //! destructor printing tree_.report() to logdir_
  ~ConfigContainer();

  // get parameters from tree
  //! get variation with default value, without validation, request needs to be provided
  template <typename T>
  T get(const std::string name, T def, Request req, bool useDbgStream = true, const size_t size = 0,
        const size_t cols = 0)
  {
    return get(name, def, ValidateAny<T>(), useDbgStream, req, size, cols);
  }

  //! get variation with default value and validation, request needs to be provided
  template <typename T, class Validator>
  T get(const std::string name, T def, const ValidatorInterface<T, Validator>& validator, Request req,
        bool useDbgStream = true, const size_t size = 0, const size_t cols = 0)
  {
    return get(name, def, validator, useDbgStream, req, size, cols);
  }

  //! get variation with default value, validation
  template <typename T, class Validator>
  T get(const std::string name, T def, const size_t size = 0, const size_t cols = 0, bool useDbgStream = true,
        const ValidatorInterface<T, Validator>& validator = ValidateAny<T>())
  {
    Request req(
        -1, std::string(), name, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    return get(name, def, validator, useDbgStream, req, size, cols);
  }

  //! get variation with default value, without validation.
  template <typename T>
  T get(const std::string name, T def, const size_t size = 0, const size_t cols = 0, bool useDbgStream = true)
  {
    Request req(-1,
                std::string(),
                name,
                Dune::Stuff::Common::toString(def),
                Dune::Stuff::Common::getTypename(ValidateAny<T>()));
    return get(name, def, ValidateAny<T>(), useDbgStream, req, size, cols);
  }

  //! get variation with default value, validation
  template <typename T, class Validator>
  T get(const std::string name, T def, const ValidatorInterface<T, Validator>& validator, bool useDbgStream = true,
        const size_t size = 0, const size_t cols = 0)
  {
    Request req(
        -1, std::string(), name, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    return get(name, def, validator, useDbgStream, req, size, cols);
  }

  //! get without default value, without validation
  template <class T>
  T get(const std::string name, size_t size = 0, size_t cols = 0, bool useDbgStream = true)
  {
    if (!has_key(name))
      DUNE_THROW(InvalidParameter, "");
    Request req(-1, std::string(), name, std::string(), Dune::Stuff::Common::getTypename(ValidateAny<T>()));
    return get<T, ValidateAny<T>>(name, T(), ValidateAny<T>(), useDbgStream, req, size, cols, false);
  }

  //! const get without default value, without validation
  template <class T>
  T get(const std::string name, size_t size = 0, size_t cols = 0, bool useDbgStream = true) const
  {
    if (!has_key(name))
      DUNE_THROW(InvalidParameter, "ConfigContainer does not have this key and there was no default value provided");
    return get_valid_value<T, ValidateAny<T>>(name, T(), ValidateAny<T>(), size, cols);
  }

  //! const get with default value, without validation
  template <class T>
  T get(const std::string name, T def, size_t size = 0, size_t cols = 0, bool useDbgStream = true) const
  {
    return get_valid_value<T, ValidateAny<T>>(name, def, ValidateAny<T>(), size, cols);
  }

  //! const get without default value, with validation
  template <class T, class Validator>
  T get(const std::string name, const ValidatorInterface<T, Validator>& validator, size_t size = 0, size_t cols = 0,
        bool useDbgStream = true) const
  {
    if (!has_key(name))
      DUNE_THROW(InvalidParameter, "ConfigContainer does not have this key and there was no default value provided");
    return get_valid_value(name, T(), validator, size, cols);
  }

  //! const get with default value, with validation
  template <class T, class Validator>
  T get(const std::string name, T def, const ValidatorInterface<T, Validator>& validator, size_t size = 0,
        size_t cols = 0, bool useDbgStream = true) const
  {
    return get_valid_value(name, def, validator, size, cols);
  }

  //! get std::vector< T > from tree_
  template <typename T, class Validator = ValidateAny<T>>
  std::vector<T> getList(const std::string name, const T def = T(), const std::string separators = ";",
                         const ValidatorInterface<T, Validator>& validator = ValidateAny<T>(), bool useDbgStream = true)
  {
    Request req(
        -1, std::string(), name, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    const auto value  = get(name, toString(def), ValidateAny<std::string>(), useDbgStream, req);
    const auto tokens = tokenize<T>(value, separators);
    for (auto token : tokens) {
      if (!validator(token)) {
        std::stringstream ss;
        validator.print(ss);
        DUNE_THROW(InvalidParameter, ss.str());
      }
    }
    return tokens;
  }

  // set values in tree_
  template <class T>
  void set(const std::string key, const T& value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                                             << "' already exists and you requested no overwrite!"
                                             << "\n======================\n"
                                             << report_string());
    tree_[key] = toString(value);
  }

  void set(const std::string& key, const char* value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW_COLORFULLY(Exceptions::configuration_error,
                            "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                                             << "' already exists and you requested no overwrite!"
                                             << "\n======================\n"
                                             << report_string());
    tree_[key] = toString(value);
  } // ... set(...)

  // forwards to ExtendedParameterTree tree_
  bool has_key(const std::string& key) const
  {
    return tree_.hasKey(key);
  }

  bool has_sub(const std::string subTreeName) const
  {
    return tree_.hasSub(subTreeName);
  }

  const ParameterTree sub(const std::string subTreeName) const
  {
    tree_.assertSub(subTreeName);
    return tree_.sub(subTreeName);
  }

  std::string& operator[](std::string name)
  {
    return tree_[name];
  }

  // add ConfigContainer or ParameterTree to ConfigContainer
  void add(const ConfigContainer& other, const std::string sub_id = "", const bool overwrite = false)
  {
    tree_.add(other.tree(), sub_id, overwrite);
    for (auto pair : other.requests_map())
      for (auto request : pair.second)
        requests_map_[pair.first].insert(request);
  } // ... add(...)

  void add(const ParameterTree& paramtree, const std::string sub_id = "", const bool overwrite = false)
  {
    tree_.add(paramtree, sub_id, overwrite);
  } // ... add(...)


  ConfigContainer& operator+=(ConfigContainer& other)
  {
    add(other);
    return *this;
  }

  ConfigContainer operator+(ConfigContainer& other)
  {
    ConfigContainer ret(*this);
    ret += other;
    return ret;
  }

  ConfigContainer& operator=(const ConfigContainer& other)
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


  ExtendedParameterTree tree() const
  {
    return tree_;
  }

  RequestMapType requests_map() const
  {
    return requests_map_;
  }

  bool empty() const
  {
    return tree_.getValueKeys().empty() && tree_.getSubKeys().empty();
  }

  void report(std::ostream& out = std::cout, const std::string& prefix = "") const
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

  std::string report_string(const std::string& prefix = "") const
  {
    return tree_.reportString(prefix);
  } // ... report_string(...)

  // read parameters from command line and/or file
  void readCommandLine(int argc, char* argv[]);
  void readOptions(int argc, char* argv[]);

  void printRequests(std::ostream& out) const;

  RequestMapType getMismatchedDefaultsMap() const;

  void printMismatchedDefaults(std::ostream& out) const;

  /**
   *  Control if the value map is filled with default values for missing entries
   *  Initially false
   **/
  void setRecordDefaults(bool record);

private:
  // methods to read in a parameter tree
  static ParameterTree initialize(const std::string filename)
  {
    ParameterTree param_tree;
    Dune::ParameterTreeParser::readINITree(filename, param_tree);
    return param_tree;
  } // ... initialize(...)

  static ParameterTree initialize(int argc, char** argv)
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

  static ParameterTree initialize(int argc, char** argv, std::string filename)
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

// member variables
#ifndef NDEBUG
  bool warning_output_;
#endif
  ExtendedParameterTree tree_;
  //! config key -> requests map
  RequestMapType requests_map_;
  bool record_defaults_;
  boost::filesystem::path logdir_;
};


inline std::ostream& operator<<(std::ostream& out, const ConfigContainer& config)
{
  config.report(out);
  return out;
}

//! global ConfigContainer instance
inline ConfigContainer& Config()
{
  static ConfigContainer parameters;
  return parameters;
}

} // namespace Common
} // namespace Stuff
} // namespace Dune

#define DSC_CONFIG Dune::Stuff::Common::Config()

#define DSC_CONFIG_GET(key, def)                                                                                       \
  DSC_CONFIG.get(                                                                                                      \
      key, def, Dune::Stuff::Common::Request(__LINE__, __FILE__, key, Dune::Stuff::Common::toString(def), "none"))

#define DSC_CONFIG_GETV(key, def, validator)                                                                           \
  DSC_CONFIG.get(                                                                                                      \
      key,                                                                                                             \
      def,                                                                                                             \
      validator,                                                                                                       \
      Dune::Stuff::Common::Request(__LINE__, __FILE__, key, Dune::Stuff::Common::toString(def), #validator))

#define DSC_CONFIG_GETB(key, def, use_logger)                                                                          \
  DSC_CONFIG.get(key,                                                                                                  \
                 def,                                                                                                  \
                 Dune::Stuff::Common::Request(__LINE__, __FILE__, key, Dune::Stuff::Common::toString(def), "none"),    \
                 use_logger)

#endif // DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
