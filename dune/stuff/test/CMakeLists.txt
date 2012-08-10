enable_testing()

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )
add_subdirectory(gtest)
include_sys_dir(${CMAKE_CURRENT_SOURCE_DIR}/gtest/include )

file( GLOB test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" )
foreach( source ${test_sources} )
	get_filename_component(testname ${source} NAME_WE)
	add_executable( ${testname} ${source} ${DUNE_HEADERS} )
	add_test( ${testname} ${CMAKE_CURRENT_BINARY_DIR}/${testname} )
	target_link_libraries( ${testname} boost_filesystem boost_system boost_timer boost_thread ${DUNE_LIBS} gtest )
endforeach( source )
