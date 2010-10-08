MACRO(ADD_CXX_FLAGS)
	ADD_DEFINITIONS(${ARGN})
	LIST(APPEND MY_CXX_FLAGS ${ARGN} )
ENDMACRO(ADD_CXX_FLAGS)

MACRO(INCLUDE_DIR)
	FOREACH( ARG ${ARGN} )
		INCLUDE_DIRECTORIES(${ARG})
		LIST(APPEND MY_CXX_FLAGS "-I${ARGN}" )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_DIR)

MACRO( HEADERCHECK )
	ADD_CUSTOM_TARGET( headercheck )
	FOREACH( HEADER ${ARGN} )
		GET_FILENAME_COMPONENT( fn ${HEADER} NAME )
		SET( TEST_NAME "headercheck_${fn}")
		ADD_CUSTOM_TARGET(  ${TEST_NAME} ${CMAKE_CXX_COMPILER} ${MY_CXX_FLAGS} -c ${HEADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.o )
		ADD_TEST( ${TEST_NAME} ${CMAKE_CXX_COMPILER} ${MY_CXX_FLAGS} -c ${HEADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.o )
		add_dependencies( headercheck ${TEST_NAME} )
	ENDFOREACH( HEADER )
ENDMACRO( HEADERCHECK )

MACRO( ADD_DUNE_MODULES )
	FOREACH( MODULE ${ARGN} )
		INCLUDE_DIR( ${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE} )
		LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/${MODULE}/.libs )
		FILE( GLOB_RECURSE tmp_header "${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/${MODULE}/*.hh" )
		LIST( APPEND DUNE_HEADERS ${tmp_header} )
	ENDFOREACH(MODULE)
ENDMACRO( ADD_DUNE_MODULES )

MACRO( ADD_DUNE_EXECUTABLE target sources headers libs )
	ADD_EXECUTABLE(${target} ${sources} ${headers} ${DUNE_HEADERS} )
	#for some $@#&& reason these targets DO NOT inherit flags added via TARGET_LINK_LIBRARIES nor INCLUDE_DIRECTORIES
	#so we need some magic to readd them
	foreach(arg ${MY_CXX_FLAGS} )
		set(bar "${bar} ${arg}")
	endforeach(arg ${MY_CXX_FLAGS} )
	set_target_properties( ${target} PROPERTIES COMPILE_FLAGS  ${bar} )
# 	get_target_property(tmp_flags ${target} LINK_FLAGS  )
# 	if( NOT ${tmp_flags} )
# 		set( tmp_flags "" )
# 	endif( NOT ${tmp_flags} )
# 	message(STATUS ${tmp_flags} )
# 	set( tmp_flags "${bar} ${tmp_flags}" )
# 	set_target_properties( ${target} PROPERTIES LINK_FLAGS  ${tmp_flags} )
	
	add_dependencies( ${target} config_refresh )
ENDMACRO( ADD_DUNE_EXECUTABLE )

add_custom_target( config_refresh
				${CMAKE_CURRENT_SOURCE_DIR}/cmake/regen_config_header.sh ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h
				)