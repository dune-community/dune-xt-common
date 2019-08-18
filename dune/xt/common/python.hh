// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   René Fritze (2018)
//
// Created by r_milk01 on 4/25/18.

#ifndef DUNE_XT_COMMON_PYTHON_HH
#define DUNE_XT_COMMON_PYTHON_HH

#include <functional>

#include <python/pybindxi/pybind11.h>

#include <dune/xt/common/deprecated.hh>

namespace Dune {
namespace XT {
namespace Common {
namespace bindings {


void guarded_bind(const std::function<void()>& registrar);


DXT_DEPRECATED_MSG("This is not required any more (08.08.2019)!")
void add_initialization(pybind11::module& /*m*/, std::string /*logger_name*/, std::string /*so_name*/ = "");


DXT_DEPRECATED_MSG("use guarded_bind() instead (08.08.2019)!")
void try_register(pybind11::module& m, const std::function<void(pybind11::module&)>& registrar);


} // namespace bindings
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PYTHON_HH
