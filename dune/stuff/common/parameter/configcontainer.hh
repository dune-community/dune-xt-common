// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Sven Kaulmann

#ifndef DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
#define DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED

#include <set>

#include <boost/format.hpp>

#include <dune/common/deprecated.hh>
#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/parametertree.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/parametertreeparser.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/misc.hh>
#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/fem/namespace.hh>


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

  /**
   * \brief less-than operator for Requests
   * \details Compare this' member variables key, def, file, line and validator (in that order) with other's. Returns
   *          true if the first comparison returns true (i.e. this.key < other.key) and returns false if this.key >
   *          other.key. If neither true nor false is returned in the first comparison (i.e. this.key "==" other.key),
   * the
   *          next comparison is evaluated similarly. If none of the first four comparisons returns a value, the value
   * of
   *          this.validator < other.validator is returned.
   * \param other Request to compare with.
   * \return bool this < other (see detailed description)
   */
  bool operator<(const Request& other) const;

  /**
   * \brief Less-than comparison of member variables key and def of a and b
   * \details Return true if a.key < b.key and false if a.key > b.key. If nothing is returned in this first step, the
   *  value of a.def < b.def is returned.
   * \param a, b Requests to compare
   * \return bool a < b (see detailed description)
   */
  friend bool strictRequestCompare(const Request& a, const Request& b);
  friend std::ostream& operator<<(std::ostream& out, const Request& r);
};

bool strictRequestCompare(const Request& a, const Request& b);

std::ostream& operator<<(std::ostream& out, const Request& r);

class DUNE_DEPRECATED_MSG("Use configuration_error instead!") InvalidParameter : public Exceptions::configuration_error
{
};


namespace internal {


static const bool config_container_record_defaults        = false;
static const bool config_container_log_on_exit            = false;
static const bool config_container_warn_on_default_access = false;
static const std::string config_container_logfile         = "data/log/dsc_parameter.log";


} // namespace internal


class ConfigContainer
{
  typedef std::map<std::string, std::set<Request>> RequestMapType;

public:
  //! warning_output = true, record_defaults = false
  ConfigContainer(const bool record_defaults = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  //! copy tree to tree_, set warning_output and record_defaults to false
  ConfigContainer(const ParameterTree& tree, const bool record_defaults = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  ConfigContainer(const ConfigContainer& other);

  //! read ParameterTree from file and call ConfigContainer(const ParameterTree& tree)
  explicit ConfigContainer(const std::string filename, const bool record_defaults, const bool warn_on_default_access,
                           const bool log_on_exit, const std::string logfile);

  //! read ParameterTree from given arguments and call ConfigContainer(const ParameterTree& tree)
  ConfigContainer(int argc, char** argv, const bool record_defaults = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  //! read ParameterTree from given arguments and file and call ConfigContainer(const ParameterTree& tree)
  ConfigContainer(int argc, char** argv, const std::string filename,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  //! warning output = true, record_defaults = false, tree_[key] = value
  template <class T>
  ConfigContainer(const std::string key, const T& value,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile)
    : tree_()
    , requests_map_()
    , record_defaults_(record_defaults)
    , warn_on_default_access_(warn_on_default_access)
    , log_on_exit_(log_on_exit)
    , logfile_(logfile)
  {
    set(key, value);
    setup_();
  }

  //! warning output = true, record_defaults = false, tree_[key] = value
  ConfigContainer(const std::string key, const char* value,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  ConfigContainer(const char* key, const char* value,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  //! warning output = true, record_defaults = false, tree_[keys[ii]] = values[ii] for 0 <= ii <= keys.size()
  template <class T>
  ConfigContainer(const std::vector<std::string> keys, const std::vector<T> values_in,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile)
    : tree_()
    , requests_map_()
    , record_defaults_(record_defaults)
    , warn_on_default_access_(warn_on_default_access)
    , log_on_exit_(log_on_exit)
    , logfile_(logfile)
  {
    if (keys.size() != values_in.size())
      DUNE_THROW(Exceptions::shapes_do_not_match,
                 "The size of 'keys' (" << keys.size() << ") does not match the size of 'values' (" << values_in.size()
                                        << ")!");
    for (size_t ii = 0; ii < keys.size(); ++ii)
      set(keys[ii], values_in[ii]);
    setup_();
  }

  /** creates std::vector< T > from value_list and then behaves exactly like
   * ConfigContainer(const std::vector< std::string > keys, const std::vector< T > values_in) */
  template <class T>
  ConfigContainer(const std::vector<std::string> keys, const std::initializer_list<T> value_list,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile)
    : ConfigContainer(keys, std::vector<T>(value_list), record_defaults, warn_on_default_access, log_on_exit, logfile)
  {
  }

  // explicit specialization of the constructor above
  ConfigContainer(const std::vector<std::string> keys, const std::initializer_list<std::string> value_list,
                  const bool record_defaults        = internal::config_container_record_defaults,
                  const bool warn_on_default_access = internal::config_container_warn_on_default_access,
                  const bool log_on_exit            = internal::config_container_log_on_exit,
                  const std::string logfile = internal::config_container_logfile);

  //! destructor printing tree_.report() to logdir_
  ~ConfigContainer();

  void set_record_defaults(const bool value = internal::config_container_record_defaults);

  void set_warn_on_default_access(const bool value = internal::config_container_warn_on_default_access);

  void set_log_on_exit(const bool value = internal::config_container_log_on_exit);

  void set_logfile(const std::string logfile = internal::config_container_logfile);

  //! get variation with default value, without validation, request needs to be provided
  template <typename T>
  T get(const std::string key, T def, Request req, const size_t size = 0, const size_t cols = 0)
  {
    return get(key, def, ValidateAny<T>(), req, size, cols, true);
  }

  //! get variation with default value and validation, request needs to be provided
  template <typename T, class Validator>
  T get(const std::string key, T def, const ValidatorInterface<T, Validator>& validator, Request req,
        const size_t size = 0, const size_t cols = 0)
  {
    return get(key, def, validator, req, size, cols, true);
  }

  //! get variation with default value, validation
  template <typename T, class Validator>
  T get(const std::string key, T def, const size_t size = 0, const size_t cols = 0,
        const ValidatorInterface<T, Validator>& validator = ValidateAny<T>())
  {
    Request req(
        -1, std::string(), key, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    return get(key, def, validator, req, size, cols, true);
  }

  //! get variation with default value, without validation.
  template <typename T>
  T get(const std::string key, T def, const size_t size = 0, const size_t cols = 0)
  {
    Request req(
        -1, std::string(), key, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(ValidateAny<T>()));
    return get(key, def, ValidateAny<T>(), req, size, cols, true);
  }

  //! get variation with default value, validation
  template <typename T, class Validator>
  T get(const std::string key, T def, const ValidatorInterface<T, Validator>& validator, const size_t size = 0,
        const size_t cols = 0)
  {
    Request req(
        -1, std::string(), key, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    return get(key, def, validator, req, size, cols, true);
  }

  //! get without default value, without validation
  template <class T>
  T get(const std::string key, size_t size = 0, size_t cols = 0)
  {
    if (!has_key(key))
      DUNE_THROW(Exceptions::configuration_error, "");
    Request req(-1, std::string(), key, std::string(), Dune::Stuff::Common::getTypename(ValidateAny<T>()));
    return get<T, ValidateAny<T>>(key, T(), ValidateAny<T>(), req, size, cols, false);
  }

  //! const get without default value, without validation
  template <class T>
  T get(const std::string key, size_t size = 0, size_t cols = 0) const
  {
    if (!has_key(key))
      DUNE_THROW(Exceptions::configuration_error,
                 "ConfigContainer does not have this key and there was no default value provided");
    return get_valid_value<T, ValidateAny<T>>(key, T(), ValidateAny<T>(), size, cols);
  }

  //! const get with default value, without validation
  template <class T>
  T get(const std::string key, T def, size_t size = 0, size_t cols = 0) const
  {
    return get_valid_value<T, ValidateAny<T>>(key, def, ValidateAny<T>(), size, cols);
  }

  //! const get without default value, with validation
  template <class T, class Validator>
  T get(const std::string key, const ValidatorInterface<T, Validator>& validator, size_t size = 0,
        size_t cols = 0) const
  {
    if (!has_key(key))
      DUNE_THROW(Exceptions::configuration_error,
                 "ConfigContainer does not have this key and there was no default value provided");
    return get_valid_value(key, T(), validator, size, cols);
  }

  //! const get with default value, with validation
  template <class T, class Validator>
  T get(const std::string key, T def, const ValidatorInterface<T, Validator>& validator, size_t size = 0,
        size_t cols = 0) const
  {
    return get_valid_value(key, def, validator, size, cols);
  }

  //! get std::vector< T > from tree_
  template <typename T, class Validator = ValidateAny<T>>
  std::vector<T> getList(const std::string key, const T def = T(), const std::string separators = ";",
                         const ValidatorInterface<T, Validator>& validator = ValidateAny<T>())
  {
    Request req(
        -1, std::string(), key, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    const auto value  = get(key, toString(def), ValidateAny<std::string>(), req);
    const auto tokens = tokenize<T>(value, separators);
    for (auto token : tokens) {
      if (!validator(token)) {
        std::stringstream ss;
        validator.print(ss);
        DUNE_THROW(Exceptions::configuration_error, ss.str());
      }
    }
    return tokens;
  }

  //! set value to key in ConfigContainer
  template <class T>
  void set(const std::string key, const T& value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW(Exceptions::configuration_error,
                 "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                                  << "' already exists and you requested no overwrite!"
                                  << "\n======================\n"
                                  << report_string());
    tree_[key] = toString(value);
  } // ... set(..., T, ...)

  void set(const std::string& key, const char* value, const bool overwrite = false)
  {
    if (has_key(key) && !overwrite)
      DUNE_THROW(Exceptions::configuration_error,
                 "While adding '" << key << "' = '" << value << "' to this (see below), the key '" << key
                                  << "' already exists and you requested no overwrite!"
                                  << "\n======================\n"
                                  << report_string());
    tree_[key] = toString(value);
  } // ... set(..., const char *, ...)

  //! check if key is existing in tree_
  bool has_key(const std::string& key) const;

  //! check if sub is existing in tree_
  bool has_sub(const std::string subTreeName) const;

  //! get subtree of tree_
  const ConfigContainer sub(const std::string sub_id) const;

  //! get reference to value assigned to key in tree_
  std::string& operator[](std::string key);

  /** \brief add another ConfigContainer to this (merge tree_s and requests_map_s)
   *  \param other ConfigContainer to add
   *  \param sub_id if not empty, other.tree_ is merged in as a sub "sub_id" of tree_
   *  \param overwrite if true, existing values are overwritten by other's values to the same key
   */
  void add(const ConfigContainer& other, const std::string sub_id = "", const bool overwrite = false);

  /** \brief add a Dune::ParameterTree paramtree to this (merge tree_ and paramtree)
   *  \param paramtree ParameterTree to add
   *  \param sub_id if not empty, paramtree is merged in as a sub "sub_id" of tree_
   *  \param overwrite if true, existing values are overwritten by paramtree's values to the same key
   */
  void add(const ParameterTree& paramtree, const std::string sub_id = "", const bool overwrite = false);

  //! add another ConfigContainer to this (merge tree_s and requests_map_s)
  ConfigContainer& operator+=(ConfigContainer& other);

  //! add this and another ConfigContainer (merge tree_s and requests_map_s)
  ConfigContainer operator+(ConfigContainer& other);

  //! assignment operator
  ConfigContainer& operator=(const ConfigContainer& other);

  //! return tree_
  ExtendedParameterTree tree() const;

  //! return requests_map_
  RequestMapType requests_map() const;

  //! check if tree_ is empty (if tree_ is empty return true even if requests_map_ is not empty)
  bool empty() const;

  /** \brief print requests_map_ and tree_
   *  \param out output stream
   *  \param prefix is printed before each key from tree_
   */
  void report(std::ostream& out = std::cout, const std::string& prefix = "") const;

  //! store output of report(..., prefix) in std::string
  std::string report_string(const std::string& prefix = "") const;

  /** get parameters from parameter file or key-value pairs given on the command line and store in ConfigContainer (and
  load into fem parameter, if available) */
  void readCommandLine(int argc, char* argv[]);

  //! search command line options for key-value pairs and add them to ConfigContainer
  void readOptions(int argc, char* argv[]);

  //! print all Requests in requests_map_
  void printRequests(std::ostream& out) const;

  /** return a map mapping keys which where requested with at least two different default values to a set containing
  the corresponding Requests */
  RequestMapType getMismatchedDefaultsMap() const;

  /**
     * \brief Checks if there are Request with differing default values to the same key
     *  (i.e. if the key was queried with non-matching default values)
     * \details Extracts the std::set<Request> from pair and removes all duplicates with respect to key and def. If
   * there is
     *  only one Request left after this step (or if the extracted set was empty originally), an empty set is returned,
     *  otherwise the set of differing requests is returned.
     * \param pair RequestMapType::value_type (i.e. std::pair< std::string, std::set<Request> >)
     * \return std::set filled with Requests that differ either in key or in def (or both), empty if there are no such
     *  differing Requests
     */
  std::set<Request> getMismatchedDefaults(RequestMapType::value_type pair) const;

  //! print all keys that were requested with at least two different default values and their respective Requests
  void printMismatchedDefaults(std::ostream& out) const;

  /**
     *  Control if the value map is filled with default values for missing entries
     *  Initially false
     **/
  void setRecordDefaults(bool record);

private:
  void setup_();

  //! get value from tree and validate with validator
  template <typename T, class Validator>
  T get_valid_value(std::string key, T def, const ValidatorInterface<T, Validator>& validator, const size_t size,
                    const size_t cols) const
  {
    std::string valstring = tree_.get(key, toString(def));
    T val = fromString<T>(valstring, size, cols);
    if (validator(val))
      return val;
    else {
      std::stringstream ss;
      validator.print(ss);
      DUNE_THROW(Exceptions::configuration_error, ss.str());
    }
  }

  /** \brief all public get signatures call this one
   *  \param key requested key
   *  \param def default value
   *  \param validator validator that is used to validate the value before it is returned
   *  \param request Request that is stored in requests_map_
   *  \param size Determines the size of the returning container (size if T is a vector type,
   *  rows if T is a matrix type, 0 means automatic).
   *  \param cols Determines the number of columns of the returning matrix if T is a matrix type
   *  (0 means automatic, ignored, if T is a vector or scalar type).
   *  \param def_provided bool to indicate if the calling get method provides a default value
   *  \return value associated to key in ConfigContainer (interpreted as type T),
   *  def if key does not exist in ConfigContainer
   */
  template <typename T, class Validator>
  T get(std::string key, T def, const ValidatorInterface<T, Validator>& validator, const Request& request,
        const size_t size, const size_t cols, const bool def_provided)
  {
    requests_map_[key].insert(request);
#ifndef NDEBUG
    if (warn_on_default_access_ && !tree_.hasKey(key)) {
      std::cerr << DSC::colorString("WARNING", DSC::Colors::brown) << ": using default value for parameter \"" << key
                << "\"" << std::endl;
    }
#endif // ifndef NDEBUG
    if (record_defaults_ && !tree_.hasKey(key) && def_provided)
      set(key, def);
    return get_valid_value(key, def, validator, size, cols);
  } // get

  //! read Dune::ParameterTree from file
  static ParameterTree initialize(const std::string filename);

  //! read Dune::ParameterTree from arguments
  static ParameterTree initialize(int argc, char** argv);

  //! read Dune::ParameterTree from arguments and file
  static ParameterTree initialize(int argc, char** argv, std::string filename);

  ExtendedParameterTree tree_;
  //! config key -> requests map
  RequestMapType requests_map_;
  bool record_defaults_;
  bool warn_on_default_access_;
  bool log_on_exit_;
  std::string logfile_;
}; // class ConfigContainer


inline std::ostream& operator<<(std::ostream& out, const ConfigContainer& config)
{
  config.report(out);
  return out;
}


//! global ConfigContainer instance
inline ConfigContainer& Config()
{
  static ConfigContainer parameters(false, true, true);
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
