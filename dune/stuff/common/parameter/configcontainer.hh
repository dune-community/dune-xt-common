#ifndef DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
#define DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED

#include <dune/common/deprecated.hh> // ensure DUNE_DEPRECATED is defined properly

#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/filesystem.hh>
#include <dune/stuff/common/misc.hh>
#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/tree.hh>
#include <dune/stuff/common/type_utils.hh>

#include <boost/format.hpp>

#include <dune/common/parametertree.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/exceptions.hh>

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
namespace Parameter {

//! use this to record defaults, placements and so forth
struct Request
{
  const int line;
  const std::string file;
  const std::string key;
  const std::string def;
  const std::string validator;
  Request(const int _line, const std::string _file, const std::string _key, const std::string _def,
          const std::string _validator)
    : line(_line)
    , file(_file)
    , key(_key)
    , def(_def)
    , validator(_validator)
  {
  }

  //! requests are considered
  bool operator<(const Request& other) const
  {
    DSC_ORDER_REL(key)
    DSC_ORDER_REL(def)
    DSC_ORDER_REL(file)
    DSC_ORDER_REL(line)
    return validator < other.validator;
  }
};

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
    DUNE_THROW(Dune::ParameterInvalid, ss.str());
  }

  std::set<Request> getMismatchedDefaults(RequestMapType::value_type pair) const
  {
    typedef bool (*func)(const Request&, const Request&);
    std::set<Request, func> mismatched(&strictRequestCompare);
    mismatched.insert(pair.second.begin(), pair.second.end());
    return std::set<Request>(std::begin(mismatched), std::end(mismatched));
  }

  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator,
        bool UNUSED_UNLESS_DEBUG(useDbgStream), const Request& request)
  {
    requests_map_[name].insert(request);
#ifndef NDEBUG
    if (warning_output_ && !tree_.hasKey(name)) {
      if (useDbgStream)
        Logger().dbg() << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
      else
        std::cerr << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
    }
#endif // ifndef NDEBUG
    return getValidValue(name, def, validator);
  } // getParam

public:
  ConfigContainer(const Dune::ParameterTree& tree)
    : warning_output_(false)
    , tree_(tree)
  {
  }

  ConfigContainer()
    : warning_output_(true)
  {
  }

  void readCommandLine(int argc, char* argv[])
  {
    if (argc < 2) {
      boost::format usage("usage: %s parameter.file *[-section.key override-value]");
      DUNE_THROW(Dune::Exception, (usage % argv[0]).str());
    }
    Dune::ParameterTreeParser::readINITree(argv[1], tree_);
    Dune::ParameterTreeParser::readOptions(argc, argv, tree_);
  } // ReadCommandLine

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

  //! hack around the "CHARS" is no string issue
  std::string get(std::string name, const char* def, bool useDbgStream = true)
  {
    return get(name, std::string(def), ValidateAny<std::string>(), useDbgStream);
  }

  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator>& validator, bool useDbgStream = true)
  {
    Request req(-1,
                std::string(),
                name,
                Dune::Stuff::Common::String::convertTo(def),
                Dune::Stuff::Common::getTypename(validator));
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

  template <class T>
  void set(const std::string key, const T value)
  {
    tree_[key] = Dune::Stuff::Common::String::convertTo(value);
  }

  void printRequests(std::ostream& out) const
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

  RequestMapType getMismatchedDefaultsMap() const
  {
    RequestMapType ret;
    for (const auto& pair : requests_map_) {
      auto mismatches = getMismatchedDefaults(pair);
      if (mismatches.size())
        ret[pair.first] = mismatches;
    }
    return ret;
  }

  void printMismatchedDefaults(std::ostream& out) const
  {
    for (const auto& pair : requests_map_) {
      out << "Mismatched uses for key " << pair.first << ":";
      for (const auto& req : getMismatchedDefaults(pair)) {
        out << "\n\t" << req;
      }
      out << "\n";
    }
  }

private:
  bool warning_output_;
  Tree::Extended tree_;
  //! config key -> requests map
  RequestMapType requests_map_;
};

//! global ConfigContainer instance
ConfigContainer& Config()
{
  static ConfigContainer parameters;
  return parameters;
}

} // namespace Parameter
} // namespace Common
} // namespace Stuff
} // namespace Dune

#define DSC_CONFIG Dune::Stuff::Common::Parameter::Config()

#define DSC_CONFIG_GET(key, def)                                                                                       \
  DSC_CONFIG.get(key,                                                                                                  \
                 def,                                                                                                  \
                 Dune::Stuff::Common::Parameter::Request(                                                              \
                     __LINE__, __FILE__, key, Dune::Stuff::Common::String::convertTo(def), "none"))

#define DSC_CONFIG_GETV(key, def, validator)                                                                           \
  DSC_CONFIG.get(key,                                                                                                  \
                 def,                                                                                                  \
                 Dune::Stuff::Common::Parameter::Request(                                                              \
                     __LINE__, __FILE__, key, Dune::Stuff::Common::String::convertTo(def), #validator))

#define DSC_CONFIG_GETB(key, def, use_logger)                                                                          \
  DSC_CONFIG.get(key,                                                                                                  \
                 def,                                                                                                  \
                 Dune::Stuff::Common::Parameter::Request(                                                              \
                     __LINE__, __FILE__, key, Dune::Stuff::Common::String::convertTo(def), "none"),                    \
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
