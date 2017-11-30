// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2017)
//   Tobias Leibner  (2017)

#ifndef DUNE_XT_COMMON_BINDINGS_HH
#define DUNE_XT_COMMON_BINDINGS_HH

#include <vector>

#include <dune/common/parallel/mpihelper.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/misc/mpimanager.hh>
#endif

#include <dune/pybindxi/pybind11.h>
#include <dune/pybindxi/stl.h>

#include "exceptions.bindings.hh"
#include "fvector.pbh"
#include "fmatrix.pbh"
#include "configuration.pbh"
#include "numeric_cast.hh"
#include "string.hh"
#include "timedlogging.hh"


#if HAVE_DUNE_FEM
#define _DUNE_XT_COMMON_BINDINGS_INIT_FEM_MPI(_argc, _argv) Dune::Fem::MPIManager::initialize(_argc, _argv);
#else
#define _DUNE_XT_COMMON_BINDINGS_INIT_FEM_MPI(_argc, _argv)
#endif


/**
  * A macro to initialize the _init_mpi and _init_logger methods, which have to be present in all generated modules.
  * \note This cannot be acchieved by free functions.
  */
#define DUNE_XT_COMMON_BINDINGS_INITIALIZE(_module, _logger_prefix)                                                    \
  Dune::XT::Common::bindings::addbind_exceptions(_module);                                                             \
  _module.def("_init_mpi",                                                                                             \
              [](const std::vector<std::string>& args) {                                                               \
                int argc = Dune::XT::Common::numeric_cast<int>(args.size());                                           \
                char** argv = Dune::XT::Common::vector_to_main_args(args);                                             \
                Dune::MPIHelper::instance(argc, argv);                                                                 \
                _DUNE_XT_COMMON_BINDINGS_INIT_FEM_MPI(argc, argv)                                                      \
              },                                                                                                       \
              pybind11::arg("args") = std::vector<std::string>());                                                     \
  _module.def(                                                                                                         \
      "_init_logger",                                                                                                  \
      [](const ssize_t max_info_level,                                                                                 \
         const ssize_t max_debug_level,                                                                                \
         const bool enable_warnings,                                                                                   \
         const bool enable_colors,                                                                                     \
         const std::string& info_color,                                                                                \
         const std::string& debug_color,                                                                               \
         const std::string& warning_color) {                                                                           \
        Dune::XT::Common::TimedLogger().create(                                                                        \
            max_info_level, max_debug_level, enable_warnings, enable_colors, info_color, debug_color, warning_color);  \
      },                                                                                                               \
      pybind11::arg("max_info_level") = std::numeric_limits<ssize_t>::max(),                                           \
      pybind11::arg("max_debug_level") = std::numeric_limits<ssize_t>::max(),                                          \
      pybind11::arg("enable_warnings") = true,                                                                         \
      pybind11::arg("enable_colors") = true,                                                                           \
      pybind11::arg("info_color") = "blue",                                                                            \
      pybind11::arg("debug_color") = "darkgray",                                                                       \
      pybind11::arg("warning_color") = "red");                                                                         \
  _module.def("_test_logger",                                                                                          \
              [](const bool info, const bool debug, const bool warning) {                                              \
                auto logger = Dune::XT::Common::TimedLogger().get(_logger_prefix);                                     \
                if (info)                                                                                              \
                  logger.info() << "info logging works!" << std::endl;                                                 \
                if (debug)                                                                                             \
                  logger.debug() << "debug logging works!" << std::endl;                                               \
                if (warning)                                                                                           \
                  logger.warn() << "warning logging works!" << std::endl;                                              \
              },                                                                                                       \
              pybind11::arg("info") = true,                                                                            \
              pybind11::arg("debug") = true,                                                                           \
              pybind11::arg("warning") = true)


#endif // DUNE_XT_COMMON_BINDINGS_HH
