// This file is part of the dune-xt project:
//   https://github.com/dune-community/dune-xt
// Copyright 2009-2018 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   René Fritze (2018)

#include "config.h"

#include <string>
#include <vector>

#include <dune/common/parallel/mpihelper.hh>
#include <python/pybindxi/pybind11.h>
#include <python/pybindxi/stl.h>

#include <python/dune/xt/common/bindings.hh>
#include <python/dune/xt/common/exceptions.bindings.hh>

#include <python/pybindxi/pybind11.h>


struct Pet
{
  Pet(const std::string& name)
    : name(name)
  {}
  void setName(const std::string& name_)
  {
    name = name_;
  }
  const std::string& getName() const
  {
    return name;
  }

  std::string name;
};
struct Dog : Pet
{
  Dog(const std::string& name)
    : Pet(name)
  {}
  std::string bark() const
  {
    return "woof!";
  }
};

struct Terrier : Dog
{
  Terrier()
    : Dog("Berti")
  {}
};

PYBIND11_MODULE(_empty, m)
{
  namespace py = pybind11;
  using namespace pybind11::literals;

  py::class_<Pet>(m, "Pet")
      .def(py::init<const std::string&>())
      .def("setName", &Pet::setName)
      .def("getName", &Pet::getName);
  py::class_<Dog, Pet /* <- specify C++ parent type */>(m, "Dog")
      .def(py::init<const std::string&>())
      .def("bark", &Dog::bark);
  py::class_<Terrier, Dog /* <- specify C++ parent type */>(m, "Terrier").def(py::init<>());
}
