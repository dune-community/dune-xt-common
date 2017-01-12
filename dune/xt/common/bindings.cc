#include "config.h"

#if HAVE_DUNE_PYBINDXI

#include <string>
#include <vector>

#include <dune/common/parallel/mpihelper.hh>

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>

#include <dune/xt/common/string.hh>

#include "exceptions.pbh"
#include "fvector.pbh"
#include "fmatrix.pbh"
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

  return m.ptr();
}

#endif // HAVE_DUNE_PYBINDXI
