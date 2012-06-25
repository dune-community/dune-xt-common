#ifndef DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED
#define DUNE_STUFF_CONFIGCONTAINER_HH_INCLUDED

#include <dune/stuff/deprecated.hh> // ensure DUNE_DEPRECATED is defined properly

#include "logging.hh"
#include "filesystem.hh"
#include "misc.hh"
#include "validation.hh"

#include <boost/format.hpp>

#include <dune/common/parametertree.hh>
#include <dune/common/parametertreeparser.hh>
#include <dune/common/exceptions.hh>

namespace Stuff {

class ConfigContainer
{
private:
  template <typename T, class Validator>
  T getValidValue(std::string name, T def, const ValidatorInterface<T, Validator> validator)
  {
    T val = tree_.get(name, def);
    if (validator(val))
      return val;
    std::stringstream ss;
    validator.print(ss);
    DUNE_THROW(Dune::ParameterInvalid, ss.str());
  }

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

  /** \brief  passthrough to underlying Dune::Parameter
     *  \param  useDbgStream
     *          needs to be set to false when using this function in Logging::Create,
     *              otherwise an assertion will will cause streams aren't available yet
     **/
  template <typename T>
  T get(std::string name, T def, bool useDbgStream = true)
  {
    return get(name, def, Stuff::ValidateAny<T>(), useDbgStream);
  }

  //! hack around the "CHARS" is no string issue
  std::string get(std::string name, const char* def, bool useDbgStream = true)
  {
    return get(name, std::string(def), Stuff::ValidateAny<std::string>(), useDbgStream);
  }

  template <typename T, class Validator>
  T get(std::string name, T def, const ValidatorInterface<T, Validator> validator,
        bool UNUSED_UNLESS_DEBUG(useDbgStream) = true)
  {
#ifndef NDEBUG
    if (warning_output_ && !tree_.hasKey(name)) {
      if (useDbgStream)
        Logger().Dbg() << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
      else
        std::cerr << "WARNING: using default value for parameter \"" << name << "\"" << std::endl;
    }
#endif // ifndef NDEBUG
    try {
      return getValidValue(name, def, validator);
    } catch (Dune::ParameterInvalid& p) {
      std::cerr << boost::format("Parameter %s invalid: %s\n") % name % p.what();
    }
    return def;
  } // getParam

  //! hack around the "CHARS" is no string issue again
  template <class Validator>
  std::string get(std::string name, const char* def, const ValidatorInterface<std::string, Validator> validator,
                  bool useDbgStream = true)
  {
    return get<std::string, Validator>(name, def, validator, useDbgStream);
  }

private:
  bool warning_output_;
  Dune::ParameterTree tree_;
};

// ! global ConfigContainer instance
ConfigContainer& Config()
{
  static ConfigContainer parameters;
  return parameters;
}

} // end namespace Stuff
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
