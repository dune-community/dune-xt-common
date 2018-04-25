//
// Created by r_milk01 on 4/25/18.
//

#ifndef DUNE_XT_COMMON_PYTHON_HH
#define DUNE_XT_COMMON_PYTHON_HH

#include <functional>

namespace pybind11 {
class module;
}

namespace Dune {
namespace XT {
namespace Common {
namespace bindings {

void add_initialization(pybind11::module& m, std::string logger_name);
void try_register(pybind11::module& m, const std::function<void(pybind11::module&)>& registrar);

} // namespace bindings
} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_PYTHON_HH
