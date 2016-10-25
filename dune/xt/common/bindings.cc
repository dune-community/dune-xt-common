#include "config.h"

#if HAVE_DUNE_PYBINDXI

#include <string>
#include <vector>

#include <dune/common/parallel/mpihelper.hh>

#include <dune/pybindxi/pybind11.h>
//#include <dune/pybindxi/stl_bind.h> // <- leads to a segfault when using list <-> std::vector conversion in
//                                          in Configuration
// PYBIND11_MAKE_OPAQUE(std::vector<ssize_t>);
// PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
// PYBIND11_MAKE_OPAQUE(std::vector<double>);

// PYBIND11_MAKE_OPAQUE(std::vector<std::vector<ssize_t>>);
// PYBIND11_MAKE_OPAQUE(std::vector<std::vector<std::string>>);
// PYBIND11_MAKE_OPAQUE(std::vector<std::vector<double>>);

#include <dune/xt/common/string.hh>

#include "exceptions.pbh"
#include "fvector.pbh"
#include "configuration.pbh"

namespace py = pybind11;
using namespace pybind11::literals;


PYBIND11_PLUGIN(common)
{
  py::module m("common", "dune-xt-common");

  Dune::XT::Common::bind_Exception(m);

  m.def("init_mpi",
        [](const std::vector<std::string>& args) {
          int argc = args.size();
          char** argv = Dune::XT::Common::vector_to_main_args(args);
          Dune::MPIHelper::instance(argc, argv);
        },
        "args"_a = std::vector<std::string>());

#define BIND_FIELD_VECTOR(V, T, S)                                                                                     \
  V<T, 0>(m, S);                                                                                                       \
  V<T, 1>(m, S);                                                                                                       \
  V<T, 2>(m, S);                                                                                                       \
  V<T, 3>(m, S);                                                                                                       \
  V<T, 4>(m, S)

  BIND_FIELD_VECTOR(Dune::XT::Common::bind_FieldVector, double, "double");
  BIND_FIELD_VECTOR(Dune::XT::Common::bind_FieldVector, ssize_t, "int");
  BIND_FIELD_VECTOR(Dune::XT::Common::bind_XtCommonFieldVector, double, "double");
  BIND_FIELD_VECTOR(Dune::XT::Common::bind_XtCommonFieldVector, ssize_t, "int");

#undef BIND_FIELD_VECTOR

  Dune::XT::Common::bind_Configuration(m);

  return m.ptr();
}

#endif // HAVE_DUNE_PYBINDXI
