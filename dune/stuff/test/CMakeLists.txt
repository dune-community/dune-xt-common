# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

enable_testing()

DEPENDENCYCHECK( ${stuff} )

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )



install(DIRECTORY gtest
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/
        FILES_MATCHING PATTERN "*.hh")
#         EXCLUDE PATTERN "stuff/test/*")

# link grid files
foreach (filename
         "gmsh_2d_simplices.msh"
         "dgf_1d_interval.dgf"
         "dgf_2d_interval.dgf"
         "dgf_3d_interval.dgf"
         "dgf_4d_interval.dgf"
        )
  execute_process(COMMAND ln -s "${CMAKE_CURRENT_SOURCE_DIR}/${filename}" "${CMAKE_CURRENT_BINARY_DIR}/${filename}")
endforeach (filename)

add_dune_system_test(SOURCE grid_periodicview.cc
	             INIFILE grid_periodicview.mini
	             BASENAME test_grid_periodicview
	             CREATED_TARGETS targetlist
		     ${DEBUG_MACRO_TESTS})

add_dune_system_test(SOURCE functions_functions.cc
	             INIFILE functions_functions.mini
	             BASENAME test_functions_functions
	             CREATED_TARGETS targetlist
		     ${DEBUG_MACRO_TESTS})

foreach(_exe ${targetlist})
  target_link_libraries(${_exe} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_stuff dunestuff)
  add_dune_mpi_flags(${_exe})
endforeach()
