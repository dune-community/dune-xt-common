#ifndef DUNE_XT_COMMON_PARAMETER_HH
#define DUNE_XT_COMMON_PARAMETER_HH

#include <string>
#include <map>
#include <vector>
#include <initializer_list>
#include <sstream>
#include <ostream>

namespace Dune {
namespace XT {
namespace Common {
namespace internal {


template <class ValueType>
class SimpleDict
{
public:
  SimpleDict();

  SimpleDict(const std::string& key, const ValueType& value);

  SimpleDict(const std::vector<std::pair<std::string, ValueType>>& key_value_pairs);

  bool empty() const;

  bool has_key(const std::string& key) const;

  void set(const std::string& key, const ValueType& value, const bool overwrite = false);

  const ValueType& get(const std::string& key) const;

  bool operator<(const SimpleDict<ValueType>& other) const;

  bool operator==(const SimpleDict<ValueType>& other) const;

  bool operator!=(const SimpleDict<ValueType>& other) const;

  size_t size() const;

  //  std::string report() const;

protected:
  std::map<std::string, ValueType> dict_;
}; // class SimpleDict


} // namespace internal


class ParameterType : public internal::SimpleDict<size_t>
{
  typedef internal::SimpleDict<size_t> BaseType;

public:
  ParameterType();

  ParameterType(const std::string& key, const size_t& sz);

  ParameterType(const std::vector<std::pair<std::string, size_t>>& key_size_pairs);
}; // class ParameterType


// std::ostream& operator<<(std::ostream& out, const ParameterType& param_type);


class Parameter : public internal::SimpleDict<std::vector<double>>
{
  typedef SimpleDict<std::vector<double>> BaseType;
  typedef std::vector<double> ValueType;

public:
  Parameter(const std::vector<std::pair<std::string, ValueType>>& key_value_pairs = {});

  Parameter(const double& value);

  Parameter(const std::string& key, const double& value);

  Parameter(const std::string& key, const ValueType& value);

  //  Parameter(const ParameterType& param_type, const std::vector<ValueType>& values);

  ParameterType type() const;
}; // class Parameter


// std::ostream& operator<<(std::ostream& out, const Parameter& mu);


} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PARAMETER_HH
