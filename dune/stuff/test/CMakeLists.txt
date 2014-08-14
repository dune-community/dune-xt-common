# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

enable_testing()

DEPENDENCYCHECK( ${stuff} )

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )

BEGIN_TESTCASES(dunestuff)

#grid pgf output
include(UseLATEX)
set(texfiles pgfoutput_stacked.tex pgfoutput_leaf.tex pgfoutput_refineseries.tex )
SET_SOURCE_FILES_PROPERTIES(${texfiles} GENERATED)
add_custom_command(OUTPUT ${texfiles} COMMAND "./test_grid_output_pgf" DEPENDS test_grid_output_pgf)

add_custom_target(tex_compile)
foreach( tex ${texfiles} )
    ADD_LATEX_DOCUMENT( ${tex} DEPENDS ${tex} DEFAULT_PDF MANGLE_TARGET_NAMES )
    string(REPLACE ".tex" "_pdf" pdf ${tex})
    add_dependencies(tex_compile ${pdf} )
endforeach( tex )
add_test(NAME test_tex_compile COMMAND ${CMAKE_MAKE_PROGRAM} tex_compile)
LIST(APPEND testnames tex_compile )

END_TESTCASES()