add_subdirectory(common)

dune_pybindxi_add_module(_common EXCLUDE_FROM_ALL bindings.cc)
target_link_dune_default_libraries(_common)
if(DUNE_XT_WITH_PYTHON_BINDINGS)
  add_custom_target(bindings ALL DEPENDS _common)
else()
  add_custom_target(bindings DEPENDS _common)
endif()
