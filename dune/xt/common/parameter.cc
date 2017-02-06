#include "config.h"

#include <type_traits>

#include "exceptions.hh"
#include "parameter.hh"

namespace Dune {
namespace XT {
namespace Common {
namespace internal {


// ========================
// ===== SimpleDict =====
// ========================
template <class ValueType>
SimpleDict<ValueType>::SimpleDict()
{
}

template <class ValueType>
SimpleDict<ValueType>::SimpleDict(const std::string& key, const ValueType& value)
  : dict_({std::make_pair(key, value)})
{
}

template <class ValueType>
SimpleDict<ValueType>::SimpleDict(const std::vector<std::pair<std::string, ValueType>>& key_value_pairs)
{
  for (const auto& key_value_pair : key_value_pairs)
    dict_.emplace(key_value_pair);
}

template <class ValueType>
bool SimpleDict<ValueType>::empty() const
{
  return dict_.size() == 0;
}

template <class ValueType>
bool SimpleDict<ValueType>::has_key(const std::string& key) const
{
  return dict_.find(key) != dict_.end();
}

template <class ValueType>
void SimpleDict<ValueType>::set(const std::string& key, const ValueType& value, const bool overwrite)
{
  if (!overwrite && has_key(key))
    DUNE_THROW(Exceptions::parameter_error,
               "You are trying to set a value (" << /*value <<*/ ") for the key '" << key
                                                 << "', although a value is already set ("
                                                 //                                                 << dict_[key]
                                                 << ") and overwrite is false!");
  dict_[key] = value;
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
bool SimpleDict<ValueType>::operator<(const SimpleDict<ValueType>& other) const
{
  return dict_ < other.dict_;
}

template <class ValueType>
bool SimpleDict<ValueType>::operator==(const SimpleDict<ValueType>& other) const
{
  return dict_ == other.dict_;
}

template <class ValueType>
bool SimpleDict<ValueType>::operator!=(const SimpleDict<ValueType>& other) const
{
  return dict_ != other.dict_;
}

template <class ValueType>
size_t SimpleDict<ValueType>::size() const
{
  return dict_.size();
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

ParameterType::ParameterType(const std::string& key, const size_t& sz)
  : BaseType(key, sz)
{
}

ParameterType::ParameterType(const std::vector<std::pair<std::string, size_t>>& key_size_pairs)
  : BaseType(key_size_pairs)
{
}

// std::string ParameterType::report() const
//{
//  std::ostringstream ret;
//  ret << "(";
//  if (dict_.size() == 1) {
//    const auto element = dict_.begin();
//    ret << "\"" << element->first << "\", " << element->second;
//  } else if (dict_.size() > 1) {
//    const auto& kk = keys();
//    const auto& vv = values();
//    ret << "{\"";
//    for (size_t ii = 0; ii < (kk.size() - 1); ++ii)
//      ret << kk[ii] << "\", \"";
//    ret << kk[kk.size() - 1] << "\"}, {";
//    for (size_t ii = 0; ii < (vv.size() - 1); ++ii)
//      ret << vv[ii] << ", ";
//    ret << vv[vv.size() - 1] << "}";
//  }
//  ret << ")";
//  return ret.str();
//}


// std::ostream& operator<<(std::ostream& oo, const ParameterType& pp)
//{
//  oo << pp.report();
//  return oo;
//}


// =====================
// ===== Parameter =====
// =====================
Parameter::Parameter(const double& value)
  : BaseType("None", {value})
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

ParameterType Parameter::type() const
{
  ParameterType ret;
  for (const auto key_value_pairs : dict_)
    ret.set(key_value_pairs.first, key_value_pairs.second.size());
  return ret;
}

// std::string Parameter::report() const
//{
//  std::ostringstream ret;
//  ret << "(";
//  if (dict_.size() == 1) {
//    const auto element = dict_.begin();
//    ret << "\"" << element->first << "\", ";
//    const auto& second = element->second;
//    if (second.size() == 1) {
//      ret << second[0];
//    } else {
//      ret << "{";
//      for (size_t ii = 0; ii < (second.size() - 1); ++ii)
//        ret << second[ii] << ", ";
//      ret << second[second.size() - 1] << "}";
//    }
//  } else if (dict_.size() > 1) {
//    const auto& kk = keys();
//    const auto& vv = values();
//    ret << "{\"";
//    for (size_t ii = 0; ii < (kk.size() - 1); ++ii)
//      ret << kk[ii] << "\", \"";
//    ret << kk[kk.size() - 1] << "\"}, {";
//    for (size_t ii = 0; ii < (vv.size() - 1); ++ii) {
//      if (vv[ii].size() == 1) {
//        ret << vv[ii][0] << ", ";
//      } else {
//        ret << "{";
//        for (size_t jj = 0; jj < (vv[ii].size() - 1); ++jj)
//          ret << vv[ii][jj] << ", ";
//        ret << vv[ii][vv[ii].size() - 1] << "}, ";
//      }
//    }
//    if (vv[vv.size() - 1].size() == 1) {
//      ret << vv[vv.size() - 1][0] << "}";
//    } else {
//      ret << "{";
//      for (size_t jj = 0; jj < (vv[vv.size() - 1].size() - 1); ++jj)
//        ret << vv[vv.size() - 1][jj] << ", ";
//      ret << vv[vv.size() - 1][vv[vv.size() - 1].size() - 1] << "}";
//    }
//  }
//  ret << ")";
//  return ret.str();
//}


// std::ostream& operator<<(std::ostream& oo, const Parameter& pp)
//{
//  oo << pp.report();
//  return oo;
//}


} // namespace Common
} // namespace XT
} // namespace Dune
