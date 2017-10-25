// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Tobias Leibner  (2017)

#include "config.h"

#include <algorithm>
#include <sstream>
#include <type_traits>

#include "exceptions.hh"
#include "parameter.hh"
#include "numeric_cast.hh"
#include "string.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace internal {


// ========================
// ===== SimpleDict =======
// ========================
template <class ValueType>
SimpleDict<ValueType>::SimpleDict()
{
}

template <class ValueType>
SimpleDict<ValueType>::SimpleDict(const std::string& key, const ValueType& value)
  : dict_({std::make_pair(key, value)})
{
  update_keys();
}

template <class ValueType>
SimpleDict<ValueType>::SimpleDict(const std::vector<std::pair<std::string, ValueType>>& key_value_pairs)
{
  for (const auto& key_value_pair : key_value_pairs)
    dict_.emplace(key_value_pair);
  update_keys();
}

template <class ValueType>
bool SimpleDict<ValueType>::empty() const
{
  return dict_.size() == 0;
}

template <class ValueType>
const std::vector<std::string>& SimpleDict<ValueType>::keys() const
{
  return keys_;
}

template <class ValueType>
bool SimpleDict<ValueType>::has_key(const std::string& key) const
{
  return dict_.find(key) != dict_.end();
}

template <class ValueType>
void SimpleDict<ValueType>::set(const std::string& key, const ValueType& value, const bool overwrite)
{
  if (key.empty())
    DUNE_THROW(Exceptions::parameter_error, "Given key must not be empty!");
  const bool key_was_present = has_key(key);
  if (!overwrite && key_was_present)
    DUNE_THROW(Exceptions::parameter_error,
               "You are trying to overwrite the key '"
                   << key
                   << "' (although a value is already set) and overwrite is false!");
  dict_[key] = value;
  if (!key_was_present)
    update_keys();
}

template <class ValueType>
const ValueType& SimpleDict<ValueType>::get(const std::string& key) const
{
  const auto result = dict_.find(key);
  if (result == dict_.end())
    DUNE_THROW(Exceptions::parameter_error, "Key '" << key << "' does not exist!");
  return result->second;
}

template <class ValueType>
size_t SimpleDict<ValueType>::size() const
{
  return dict_.size();
}

template <class ValueType>
std::string SimpleDict<ValueType>::report(const std::string& prefix) const
{
  if (dict_.empty())
    return "{}";

  assert(keys_.size() > 0);
  const auto whitespaced_prefix = whitespaceify(prefix);
  std::stringstream ss;

  ss << "{" << keys_[0] << ": " << dict_.at(keys_[0]);
  for (size_t ii = 1; ii < keys_.size(); ++ii) {
    ss << ",\n" << whitespaced_prefix << " " << keys_[ii] << ": " << dict_.at(keys_[ii]);
  }
  ss << "}";
  return ss.str();
}

template <class ValueType>
void SimpleDict<ValueType>::update_keys()
{
  keys_ = std::vector<std::string>(dict_.size());
  size_t ii = 0;
  for (const auto& key_value_pair : dict_) {
    keys_[ii] = key_value_pair.first;
    ++ii;
  }
  std::sort(keys_.begin(), keys_.end());
}


template class SimpleDict<size_t>;
template class SimpleDict<std::vector<double>>;


} // namespace internal


// =========================
// ===== ParameterType =====
// =========================
ParameterType::ParameterType()
  : BaseType()
{
}

ParameterType::ParameterType(const std::string& key)
  : BaseType(key, 1)
{
}

ParameterType::ParameterType(const std::string& key, const size_t& sz)
  : BaseType(key, sz)
{
}

ParameterType::ParameterType(const std::pair<std::string, size_t>& key_size_pair)
  : BaseType(key_size_pair.first, key_size_pair.second)
{
}

ParameterType::ParameterType(const std::pair<const char*, int>& key_size_pair)
  : BaseType(std::string(key_size_pair.first), numeric_cast<size_t>(key_size_pair.second))
{
}

ParameterType::ParameterType(const std::vector<std::pair<std::string, size_t>>& key_size_pairs)
  : BaseType(key_size_pairs)
{
}

bool ParameterType::operator==(const ParameterType& other) const
{
  if (this->size() == 1 && other.size() == 1) {
    const auto& this_single_element = *this->dict_.begin();
    const auto& other_single_element = *other.dict_.begin();
    if (this_single_element.first == other_single_element.first || this_single_element.first == "__unspecified__"
        || other_single_element.first == "__unspecified__") {
      return this_single_element.second == other_single_element.second;
    } else {
      return false;
    }
  } else {
    return this->dict_ == other.dict_;
  }
}

bool ParameterType::operator!=(const ParameterType& other) const
{
  return !(*this == other);
}

bool ParameterType::operator<(const ParameterType& other) const
{
  return this->dict_ < other.dict_;
}

bool ParameterType::operator<=(const ParameterType& other) const
{
  return *this < other || *this == other;
}

std::string ParameterType::report() const
{
  return "ParameterType(" + BaseType::report("ParameterType(") + ")";
}


std::ostream& operator<<(std::ostream& out, const ParameterType& param_type)
{
  out << param_type.report();
  return out;
}


// =====================
// ===== Parameter =====
// =====================
Parameter::Parameter(const double& value)
  : BaseType("__unspecified__", {value})
{
}

Parameter::Parameter(const std::vector<double>& value)
  : BaseType("__unspecified__", value)
{
}

Parameter::Parameter(const std::string& key, const double& value)
  : BaseType(key, {value})
{
}

Parameter::Parameter(const std::string& key, const ValueType& value)
  : BaseType(key, value)
{
}

Parameter::Parameter(const std::vector<std::pair<std::string, ValueType>>& key_value_pairs)
  : BaseType(key_value_pairs)
{
}

bool Parameter::operator<(const Parameter& other) const
{
  return this->dict_ < other.dict_;
}

ParameterType Parameter::type() const
{
  ParameterType ret;
  for (const auto key_value_pairs : dict_)
    ret.set(key_value_pairs.first, key_value_pairs.second.size());
  return ret;
}

std::string Parameter::report() const
{
  return "Parameter(" + BaseType::report("Parameter(") + ")";
}


std::ostream& operator<<(std::ostream& out, const Parameter& mu)
{
  out << mu.report();
  return out;
}


// ===============================
// ===== ParametricInterface =====
// ===============================
ParametricInterface::ParametricInterface(const ParameterType& param_type)
  : parameter_type_(param_type)
{
}

bool ParametricInterface::is_parametric() const
{
  return !parameter_type().empty();
}

const ParameterType& ParametricInterface::parameter_type() const
{
  return parameter_type_;
}

Parameter ParametricInterface::parse_parameter(const Parameter& mu) const
{
  const auto& this_type = this->parameter_type();
  const auto& mus_type = mu.type();
  if (this_type.size() == 1 && mus_type.size() == 1) {
    // both have only one key, so either key might be '__unspecified__'
    const auto this_single_key = this_type.keys().at(0);
    const auto mus_single_key = mus_type.keys().at(0);
    if (this_single_key == mus_single_key)
      return mu;
    if (this_single_key == "__unspecified__") {
      return mu;
    } else if (mus_single_key == "__unspecified__") {
      return Parameter(this_single_key, mu.get("__unspecified__"));
    }
    // both have only one key, but the keys don't match and neither is '__unspecified__'
    DUNE_THROW(Exceptions::parameter_error,
               "this->parameter_type() = " << this_type << "\n   "
                                           << "mu.type() = "
                                           << mus_type);
  }
  // one of them has more than one key, so '__unspecified__' does not play a role here
  if (this_type <= mus_type)
    return mu;
  DUNE_THROW(Exceptions::parameter_error,
             "this->parameter_type() = " << this_type << "\n   "
                                         << "mu.type() = "
                                         << mus_type);
  return Parameter();
} // ... parse_parameter(...)

Parameter ParametricInterface::parse_and_check(const Parameter& mu) const
{
  return parse_parameter(mu);
}

} // namespace Common
} // namespace XT
} // namespace Dune
