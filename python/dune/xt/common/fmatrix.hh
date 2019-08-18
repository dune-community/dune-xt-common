// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2016 - 2017)
//   René Fritze     (2018)
//   TiKeil          (2018)

#ifndef DUNE_XT_COMMON_FMATRIX_PBH
#define DUNE_XT_COMMON_FMATRIX_PBH

#include <python/pybindxi/complex.h>
#include <python/pybindxi/pybind11.h>
#include <python/pybindxi/cast.h>

#include <dune/xt/common/fmatrix.hh>

#include "fvector.hh"

NAMESPACE_BEGIN(pybind11)
NAMESPACE_BEGIN(detail)


template <class FieldMatrixImp>
struct FieldMatrix_type_caster
{
  using type = FieldMatrixImp;
  typedef typename type::value_type K;
  typedef typename type::row_type row_type;
  static const int ROWS = type::rows;
  static const int COLS = type::cols;
  using value_conv = make_caster<K>;
  using row_conv = make_caster<row_type>;

  bool load(handle src, bool convert)
  {
    if (!isinstance<sequence>(src))
      return false;
    auto s = reinterpret_borrow<sequence>(src);
    if (s.size() != ROWS)
      return false;
    row_conv conv;
    value *= K(0.0);
    size_t ii = 0;
    for (auto it : s) {
      if (ii >= ROWS)
        return false;
      if (!conv.load(it, convert))
        return false;
      value[ii++] = cast_op<row_type>(conv);
    }
    return true;
  } // ... load(...)

  static handle cast(const type& src, return_value_policy policy, handle parent)
  {
    list l(ROWS);
    for (size_t ii = 0; ii < src.size(); ++ii) {
      object val = reinterpret_steal<object>(row_conv::cast(src[ii], policy, parent));
      if (!val)
        return handle();
      PyList_SET_ITEM(l.ptr(), ii, val.release().ptr()); // steals a reference
    }
    return l.release();
    return list(0).release();
  } // ... cast(...)

  PYBIND11_TYPE_CASTER(type,
                       _("List[List[") + value_conv::name + _("[") + _<COLS>() + _("]]") + _("[") + _<ROWS>()
                           + _("]]"));
}; // struct FieldMatrix_type_caster


template <class K, int N, int M>
struct type_caster<Dune::FieldMatrix<K, N, M>> : public FieldMatrix_type_caster<Dune::FieldMatrix<K, N, M>>
{};

/**
 * Dune::XT::Common::FieldMatrix is already able to handle std::string as we already provide the correct default value
 * (see type_traits::suitable_default). Thus, we do not need to initalize with value *= K(0.0)
 *
 * Attention: If this is not enough you need to specialize for std::string. (see fvector.hh)
 */
template <class K, int N, int M>
struct type_caster<Dune::XT::Common::FieldMatrix<K, N, M>>
  : public FieldMatrix_type_caster<Dune::XT::Common::FieldMatrix<K, N, M>>
{
  using type = Dune::XT::Common::FieldMatrix<K, N, M>;
  typedef typename type::row_type row_type;
  static const int ROWS = type::rows;
  static const int COLS = type::cols;
  using value_conv = make_caster<K>;
  using row_conv = make_caster<row_type>;

  bool load(handle src, bool convert)
  {
    if (!isinstance<sequence>(src))
      return false;
    auto s = reinterpret_borrow<sequence>(src);
    if (s.size() != ROWS)
      return false;
    row_conv conv;
    size_t ii = 0;
    for (auto it : s) {
      if (ii >= ROWS)
        return false;
      if (!conv.load(it, convert))
        return false;
      value[ii++] = cast_op<row_type>(conv);
    }
    return true;
  } // ... load(...)

  PYBIND11_TYPE_CASTER(type,
                       _("List[List[") + value_conv::name + _("[") + _<COLS>() + _("]]") + _("[") + _<ROWS>()
                           + _("]]"));
};


NAMESPACE_END(detail)
NAMESPACE_END(pybind11)

#endif // DUNE_XT_COMMON_FMATRIX_PBH
