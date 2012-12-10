enable_testing()

HEADERCHECK( ${stuff} )
DEPENDENCYCHECK( ${stuff} )

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )
include_dir(${CMAKE_CURRENT_SOURCE_DIR} )

file( GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )

foreach( source ${test_sources} )
	get_filename_component(testname ${source} NAME_WE)
	add_executable( test_${testname} ${source} ${DUNE_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/gtest/gtest-all.cc)
	add_test( test_${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname} )
	target_link_libraries( test_${testname} boost_filesystem boost_system boost_timer boost_thread ${DUNE_LIBS} pthread )
	list(APPEND testnames test_${testname} )
endforeach( source )

#grid pgf output
include(UseLATEX)
set(texfiles pgfoutput_stacked.tex pgfoutput_leaf.tex pgfoutput_refineseries.tex )
SET_SOURCE_FILES_PROPERTIES(${texfiles} GENERATED)
add_custom_command(OUTPUT ${texfiles} COMMAND "./grid_output_pgf" DEPENDS grid_output_pgf)
add_custom_target(tex_compile)
foreach( tex ${texfiles} )
    ADD_LATEX_DOCUMENT( ${tex} DEPENDS ${tex} DEFAULT_PDF MANGLE_TARGET_NAMES )
    string(REPLACE ".tex" "_pdf" pdf ${tex})
    add_dependencies(tex_compile ${pdf} )
endforeach( tex )
add_test(NAME test_tex_compile COMMAND ${CMAKE_MAKE_PROGRAM} tex_compile)
LIST(APPEND testnames tex_compile )

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${testnames} )
