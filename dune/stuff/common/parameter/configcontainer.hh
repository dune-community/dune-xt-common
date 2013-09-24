#ifndef DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
#define DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED

#include <config.h>

#include <dune/common/deprecated.hh>
#include <dune/common/parametertree.hh>
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

  template <typename T, class Validator>
  T getValidValue(std::string name, T def, const ValidatorInterface<T, Validator>& validator)
  {
    T val = tree_.get(name, def);
    if (validator(val))
      return val;
    std::stringstream ss;
    validator.print(ss);
    DUNE_THROW(InvalidParameter, ss.str());
  }

  //! return a set of Request objects for keys that have been queried with non-matching default values
  std::set<Request> getMismatchedDefaults(RequestMapType::value_type pair) const;

  //! all public get signatures call this one
  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator,
        bool UNUSED_UNLESS_DEBUG(useDbgStream), const Request& request)
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
    if (record_defaults_ && !tree_.hasKey(name))
      set(name, def);
    return getValidValue(name, def, validator);
  } // getParam

public:
  ConfigContainer(const Dune::ParameterTree& tree);
  ConfigContainer();
  ~ConfigContainer();

  void readCommandLine(int argc, char* argv[]);
  void readOptions(int argc, char* argv[]);

  /** \brief  passthrough to underlying Dune::ParameterTree
     *  \param  useDbgStream
     *          needs to be set to false when using this function in Logging::Create,
     *              otherwise an assertion will fire cause streams aren't available yet
     **/
  template <typename T>
  T get(std::string name, T def, bool useDbgStream = true)
  {
    return get(name, def, ValidateAny<T>(), useDbgStream);
  }

  //! get variation with request recording
  template <typename T>
  T get(std::string name, T def, Request req, bool useDbgStream = true)
  {
    return get(name, def, ValidateAny<T>(), useDbgStream, req);
  }
  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator, Request req,
        bool useDbgStream = true)
  {
    return get(name, def, validator, useDbgStream, req);
  }

  //! hack around the "CHARS" is no string issue
  std::string get(std::string name, const char* def, bool useDbgStream = true)
  {
    return get(name, std::string(def), ValidateAny<std::string>(), useDbgStream);
  }

  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator, bool useDbgStream = true)
  {
    Request req(
        -1, std::string(), name, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    return get(name, def, validator, useDbgStream, req);
  }

  //! hack around the "CHARS" is no string issue again
  template <class Validator>
  std::string get(std::string name, const char* def, const ValidatorInterface<std::string, Validator>& validator,
                  bool useDbgStream = true)
  {
    return get<std::string, Validator>(name, def, validator, useDbgStream);
  }

  //! get variation with request recording
  std::string get(std::string name, const char* def, Request req, bool useDbgStream = true)
  {
    return get(name, std::string(def), ValidateAny<std::string>(), useDbgStream, req);
  }

  template <typename T, class Validator = ValidateAny<T>>
  std::vector<T> getList(const std::string name, const T def = T(), const std::string seperators = ";",
                         const ValidatorInterface<T, Validator>& validator = ValidateAny<T>(), bool useDbgStream = true)
  {
    Request req(
        -1, std::string(), name, Dune::Stuff::Common::toString(def), Dune::Stuff::Common::getTypename(validator));
    const auto value  = get<std::string>(name, toString(def), ValidateAny<std::string>(), useDbgStream, req);
    const auto tokens = tokenize<T>(value, seperators);
    for (auto token : tokens) {
      if (!validator(token)) {
        std::stringstream ss;
        validator.print(ss);
        DUNE_THROW(InvalidParameter, ss.str());
      }
    }
    return tokens;
  }

  template <class T>
  void set(const std::string key, const T value)
  {
    tree_[key] = toString(value);
  }

  void printRequests(std::ostream& out) const;

  RequestMapType getMismatchedDefaultsMap() const;

  void printMismatchedDefaults(std::ostream& out) const;

  /**
   *  Control if the value map is filled with default values for missing entries
   *  Initially false
   **/
  void setRecordDefaults(bool record);

private:
  bool warning_output_;
  ExtendedParameterTree tree_;
  //! config key -> requests map
  RequestMapType requests_map_;
  bool record_defaults_;
  boost::filesystem::path logdir_;
};

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

/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
