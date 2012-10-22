include(CheckCXXSourceCompiles)

function(TO_LIST_SPACES _LIST_NAME OUTPUT_VAR)
  set(NEW_LIST_SPACE)
  foreach(ITEM ${${_LIST_NAME}})
    set(NEW_LIST_SPACE ${NEW_LIST_SPACE} ${ITEM})
  endforeach()
#   string(STRIP ${NEW_LIST_SPACE} NEW_LIST_SPACE)
  set(${OUTPUT_VAR} "${NEW_LIST_SPACE}" PARENT_SCOPE)
endfunction()

MACRO(ADD_CXX_FLAGS)
    string(REPLACE " " ";" FLAGS ${ARGN})
	FOREACH( ARG ${FLAGS} )
# 		ADD_DEFINITIONS(${ARG})
		LIST(APPEND MY_CXX_FLAGS ${ARG} )
	ENDFOREACH( ARG )
ENDMACRO(ADD_CXX_FLAGS)

MACRO(INCLUDE_DIR)
	FOREACH( ARG ${ARGN} )
		IF(IS_DIRECTORY ${ARG} )
			LIST(APPEND MY_CXX_FLAGS "-I${ARG}" )
			LIST(APPEND PLAIN_INCLUDE_DIRS "-I${ARG}" )
			INCLUDE_DIRECTORIES(${ARG})
		ELSE(IS_DIRECTORY ${ARG} )
			MESSAGE( STATUS "Include directory ${ARG} does not exist" )
		ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_DIR)

MACRO(INCLUDE_SYS_DIR)
	FOREACH( ARG ${ARGN} )
		IF(IS_DIRECTORY ${ARG} )
			LIST(APPEND MY_CXX_FLAGS "-isystem${ARG}")
			LIST(APPEND PLAIN_INCLUDE_DIRS "-I${ARG}" )
			INCLUDE_DIRECTORIES(${ARG})
		ELSE(IS_DIRECTORY ${ARG} )
			MESSAGE( STATUS "Include directory ${ARG} does not exist" )
		ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_SYS_DIR)

MACRO( HEADERCHECK )
	ADD_CUSTOM_TARGET( headercheck SOURCES ${ARGN} )
	FOREACH( HEADER ${ARGN} )
		string(REPLACE "/" "_" fn ${HEADER})	
		SET( TEST_NAME "headercheck_${fn}")
		TO_LIST_SPACES( MY_CXX_FLAGS TEST_NAME_FLAGS )
		SET(XARGS ${TEST_NAME_FLAGS} -DHAVE_CONFIG_H -c ${HEADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.o )
		ADD_CUSTOM_TARGET(  ${TEST_NAME} ${CMAKE_CXX_COMPILER} ${XARGS} )
		ADD_TEST( ${TEST_NAME} ${CMAKE_CXX_COMPILER} ${XARGS} )
		add_dependencies( headercheck ${TEST_NAME} )
	ENDFOREACH( HEADER )
ENDMACRO( HEADERCHECK )

MACRO( DEPENDENCYCHECK )
    ADD_CUSTOM_TARGET( dependenycheck SOURCES ${ARGN} )
    FOREACH( HEADER ${ARGN} )
	string(REPLACE "/" "_" fn ${HEADER})	
        SET( TEST_NAME "dependenycheck_${fn}")
        TO_LIST_SPACES( MY_CXX_FLAGS TEST_NAME_FLAGS )
        SET(XARGS ${TEST_NAME_FLAGS} -DHAVE_CONFIG_H -H -c ${HEADER} -w)
        ADD_CUSTOM_TARGET( ${TEST_NAME} + ${CMAKE_CURRENT_SOURCE_DIR}/../dune-stuff/cmake/dependencyinfo.py ${CMAKE_CXX_COMPILER} ${XARGS} ${CMAKE_CURRENT_SOURCE_DIR} ${fn}.dep)
        add_dependencies( dependenycheck ${TEST_NAME} )
    ENDFOREACH( HEADER )
ENDMACRO( DEPENDENCYCHECK )

MACRO( ADD_CPPCHECK )
	find_program( CPPCHECK_BINARY NAMES cppcheck )
	IF( EXISTS ${CPPCHECK_BINARY} )
		SET(CPPINLINST ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.files )
		IF( EXISTS ${CPPINLINST} )
			file(REMOVE  ${CPPINLINST} )
		ENDIF()
		FOREACH( SOURCEFILE ${ARGN} )
			FILE( APPEND  ${CPPINLINST}  "${SOURCEFILE}\n" )
		ENDFOREACH( SOURCEFILE )	
		TO_LIST_SPACES( PLAIN_INCLUDE_DIRS CPPCHECK_FLAGS_SPLIT )
		ADD_CUSTOM_TARGET(  cppcheck cppcheck --xml --enable=all --report-progress --file-list=${CPPINLINST}
				${CPPCHECK_FLAGS_SPLIT}  2>cppcheck.xml )
	ELSE( EXISTS ${CPPCHECK_BINARY} )
		MESSAGE( STATUS "Not adding cppcheck target because cppcheck executable not found" )
	ENDIF( EXISTS ${CPPCHECK_BINARY} )
ENDMACRO( ADD_CPPCHECK )

MACRO( ADD_DUNE_MODULES )
	FOREACH( MODULE ${ARGN} )
		INCLUDE_SYS_DIR( ${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE} )
		LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/lib/.libs )
		string(TOUPPER ${MODULE} UPMODULE )
		ADD_CXX_FLAGS( -DHAVE_DUNE_${UPMODULE} )
		FILE( GLOB_RECURSE tmp_header "${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/dune/${MODULE}/*.hh" )
		LIST( APPEND DUNE_HEADERS ${tmp_header} )
		SET( tmp_lib ${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/lib/.libs/libdune${MODULE}.a  )
		IF( EXISTS ${tmp_lib} )
			LIST( APPEND DUNE_LIBS ${tmp_lib} )
		ENDIF( EXISTS ${tmp_lib} )
	ENDFOREACH(MODULE)
ENDMACRO( ADD_DUNE_MODULES )

MACRO( ADD_MY_MODULES )
	FOREACH( MODULE ${ARGN} )
		INCLUDE_DIR( ${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE} )
		LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/${MODULE}/.libs )
		FILE( GLOB_RECURSE tmp_header "${CMAKE_CURRENT_SOURCE_DIR}/../dune-${MODULE}/${MODULE}/*.hh" )
		LIST( APPEND DUNE_HEADERS ${tmp_header} )
	ENDFOREACH(MODULE)
ENDMACRO( ADD_MY_MODULES )

MACRO( ADD_DUNE_EXECUTABLE target sources headers libs )
	ADD_EXECUTABLE(${target} ${sources} ${headers} ${DUNE_HEADERS} )
	#for some $@#&& reason these targets DO NOT inherit flags added via TARGET_LINK_LIBRARIES nor INCLUDE_DIRECTORIES
	#so we need some magic to readd them
# 	get_target_property(tmp_flags ${target} COMPILE_FLAGS  )
# 	if( NOT ${tmp_flags} )
# 		set( tmp_flags "" )
# 	endif( NOT ${tmp_flags} )
# 	LIST(APPEND MY_CXX_FLAGS ${tmp_flags} )
# 	foreach(arg ${MY_CXX_FLAGS} )
# 		set(bar "${bar} ${arg}")
# 	endforeach(arg ${MY_CXX_FLAGS} )
# 	set_target_properties( ${target} PROPERTIES COMPILE_FLAGS  ${bar} )
# 	get_target_property(tmp_flags ${target} LINK_FLAGS  )
# 	if( NOT ${tmp_flags} )
# 		set( tmp_flags "" )
# 	endif( NOT ${tmp_flags} )
# # 	message(STATUS ${tmp_flags} )
# 	set( tmp_flags "${bar} ${tmp_flags}" )
# 	set_target_properties( ${target} PROPERTIES LINK_FLAGS  ${tmp_flags} )
	TARGET_LINK_LIBRARIES(${target} ${libs} )
	
	add_dependencies( ${target} config_refresh )
ENDMACRO( ADD_DUNE_EXECUTABLE )

MACRO( SET_CONFIGHEADER_VARS )
	IF( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
		SET( ALUGRID_FOUND "1" )
	ELSE( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
		SET( ALUGRID_FOUND "0" )
	ENDIF( IS_DIRECTORY ${ALUGRID_BASE_PATH} )
ENDMACRO( SET_CONFIGHEADER_VARS )

add_custom_target( config_refresh
				${CMAKE_CURRENT_SOURCE_DIR}/cmake/regen_config_header.sh ${CMAKE_CURRENT_BINARY_DIR}/cmake_config.h
				)

execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

INCLUDE (CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("tr1/array" HAVE_TR1_ARRAY)
CHECK_INCLUDE_FILE_CXX("malloc.h" HAVE_MALLOC_H)

# try to use compiler flag -std=c++11
include(TestCXXAcceptsFlag)
CHECK_CXX_ACCEPTS_FLAG("-std=c++11" CXX_FLAG_CXX11)
if(CXX_FLAG_CXX11)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11 ")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=c++11 ")
  set(CXX_STD0X_FLAGS "-std=c++11")
else()
  # try to use compiler flag -std=c++0x for older compilers
  CHECK_CXX_ACCEPTS_FLAG("-std=c++0x" CXX_FLAG_CXX0X)
  if(CXX_FLAG_CXX0X)
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++0x" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++0x ")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -std=c++0x ")
  set(CXX_STD0X_FLAGS "-std=c++0x")
  endif(CXX_FLAG_CXX0X)
endif(CXX_FLAG_CXX11)

if(NOT CXX_STD0X_FLAGS)
    message(FATAL "you need a c++11 compatible compiler")
endif()

# __attribute__((deprecated))
CHECK_CXX_SOURCE_COMPILES("
   int main(void)
   {
     auto f = [&] (){ return 0; };
     return 0;
   };
"  HAS_LAMBDA_FUNCTIONS
)

CHECK_CXX_SOURCE_COMPILES("
		#include <vector>
		#include <iterator>
		int main(void)
		{
			std::vector<int> a;
			std::vector<int>::const_iterator b = std::begin(a);
			std::vector<int>::const_iterator e = std::end(a);
			return 0;
		};
"  HAS_STD_BEGIN_END
)


SET( CUSTOM_FLAGS
	"-Wall -Wextra -Wlogical-op -Wc++0x-compat -Wparentheses -pedantic -Wredundant-decls -Wshadow -Winline -fno-strict-aliasing" CACHE STRING
	"CUSTOM FLAGS")
SET( BOOST_ROOT
	"/opt/boost-1.48" CACHE STRING
	"" )	

SET( CMAKE_CXX_FLAGS_RELEASE
	"-DDNDEBUG -funroll-loops -m64 -mfpmath=sse -falign-loops -mtune=native -march=native -O3 -pipe -fomit-frame-pointer " )

SET( CMAKE_CXX_FLAGS_DEBUG
	"-O0 -DDNDEBUG -g3 -ggdb" )
	
FIND_PACKAGE( PkgConfig )
FIND_PACKAGE(Boost 1.48.0 REQUIRED)
ADD_CXX_FLAGS( "-DHAVE_CMAKE_CONFIG ${CXX_STD0X_FLAGS}" )
INCLUDE_SYS_DIR(${Boost_INCLUDE_DIRS})
INCLUDE_SYS_DIR(${CMAKE_CURRENT_SOURCE_DIR}/dune/stuff/test/)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

#try to enable link-time-optimization
if (CMAKE_COMPILER_IS_GNUCC)
	if (GCC_VERSION VERSION_GREATER 4.5 OR GCC_VERSION VERSION_EQUAL 4.5)
		set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -flto")
		set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS} -flto")
		set (CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS} -flto")
	endif()
endif()

ENABLE_TESTING()
