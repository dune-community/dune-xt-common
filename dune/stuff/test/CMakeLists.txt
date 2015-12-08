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

BEGIN_TESTCASES(dunestuff)
 
find_program(LATEX_FOUND latex)
if (LATEX_FOUND)
  include(UseLATEX)
  if(dune_grid-FOUND)
    # these inputs can only exist if tests have run that require dune-grid
    set( grid_texfiles pgfoutput_stacked.tex pgfoutput_leaf.tex pgfoutput_refineseries.tex)
    add_custom_command(OUTPUT ${grid_texfiles} COMMAND "./test_grid_output_pgf" DEPENDS test_grid_output_pgf)
  endif()
#
  set(texfiles ${grid_texfiles})

  add_custom_target(tex_compile)
  foreach( tex ${texfiles} )
      add_latex_document( ${tex} DEPENDS ${tex} DEFAULT_PDF MANGLE_TARGET_NAMES )
      string(REPLACE ".tex" "_pdf" pdf ${tex})
      add_dependencies(tex_compile ${pdf} )
      set_source_files_properties(${tex} GENERATED)
  endforeach( tex )
  add_test(NAME test_tex_compile COMMAND ${CMAKE_MAKE_PROGRAM} tex_compile)
  list(APPEND testnames tex_compile )
endif()

END_TESTCASES()

dune_add_system_test(SOURCE functions_checkerboard.cc
                     INIFILE functions_checkerboard.mini
                     BASENAME test_functions_checkerboard
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_constant.cc
                     INIFILE functions_constant.mini
                     BASENAME test_functions_constant
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_ESV2007.cc
                     INIFILE functions_ESV2007.mini
                     BASENAME test_functions_ESV2007
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_expression.cc
                     INIFILE functions_expression.mini
                     BASENAME test_functions_expression
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_functions.cc
                     INIFILE functions_functions.mini
                     BASENAME test_functions_functions
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_global.cc
                     INIFILE functions_global.mini
                     BASENAME test_functions_global
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_indicator.cc
                     INIFILE functions_indicator.mini
                     BASENAME test_functions_indicator
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE functions_spe10.cc
                     INIFILE functions_spe10.mini
                     BASENAME test_functions_spe10
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})

dune_add_system_test(SOURCE grid_periodicview.cc
                     INIFILE grid_periodicview.mini
                     BASENAME test_grid_periodicview
                     CREATED_TARGETS targetlist
                     ${DEBUG_MACRO_TESTS})


foreach(_exe ${targetlist})
  target_link_libraries(${_exe} ${COMMON_LIBS} ${GRID_LIBS} gtest_dune_stuff dunestuff)
  add_dune_mpi_flags(${_exe})
endforeach()
