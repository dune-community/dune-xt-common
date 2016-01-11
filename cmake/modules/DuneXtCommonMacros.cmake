# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2013 - 2014)
#   Rene Milk       (2013 - 2015)
#   Sven Kaulmann   (2014)

################ macros and functions #################################################################################

Macro(ADD_IF_SUPPORTED dest)
  FOREACH(flag ${ARGN})
    CHECK_CXX_ACCEPTS_FLAG("${flag}" has_${flag})
    if(has_${flag})
      Set(${dest} "${${dest}} ${flag}")
    else(has_${flag})
      Message("compiler doesn't support: ${flag}")
    endif(has_${flag})
  ENDFOREACH(flag ${ARGN})
EndMacro(ADD_IF_SUPPORTED)

MACRO(INCLUDE_SYS_DIR)
    FOREACH( ARG ${ARGN} )
        IF(IS_DIRECTORY ${ARG} )
            INCLUDE_DIRECTORIES(${ARG})
            ADD_DEFINITIONS("-isystem${ARG}")
        ELSE(IS_DIRECTORY ${ARG} )
            MESSAGE( STATUS "Include directory ${ARG} does not exist" )
        ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_SYS_DIR)

macro(add_analyze)
    find_program(ANALYZER clang-check)
    if(EXISTS ${ANALYZER})
        message(STATUS "adding analyze target")
        add_custom_target( analyze SOURCES ${ARGN} )
        foreach(_file ${ARGN})
            string(REPLACE "/" "_" fn ${_file})
            add_custom_target("analyze_${fn}" ${ANALYZER} -fixit -p=${CMAKE_CURRENT_BINARY_DIR} -analyze ${CMAKE_CURRENT_SOURCE_DIR}/${_file} )
            add_dependencies( analyze "analyze_${fn}" )
        endforeach(_file )
    else()
        message(WARNING "not adding analyze target because clang-check is missing")
    endif(EXISTS ${ANALYZER})
endmacro(add_analyze)

macro(add_format)
    find_program(FORMAT NAMES clang-format clang-format-3.4 clang-format-3.5 clang-format-3.6 clang-format-3.7 clang-format-3.8 clang-format-3.9)
    if(EXISTS ${FORMAT})
        message(STATUS "adding format target")
        add_custom_target( format SOURCES ${ARGN} )
        foreach(_file ${ARGN})
            string(REPLACE "/" "_" fn ${_file})
            if(NOT EXISTS ${_file})
                # 'fix' relative source defs
                set(_file ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
            endif(NOT EXISTS ${_file})
            add_custom_target("format_${fn}" ${FORMAT} -i -style=file ${_file} )
            add_dependencies( format "format_${fn}" )
        endforeach(_file )
    else()
        message(WARNING "not adding format target because clang-format is missing")
    endif(EXISTS ${FORMAT})
endmacro(add_format)

macro(add_forced_doxygen_target)
  add_doxygen_target()
  if(TARGET doxygen_${ProjectName})
    add_custom_target(doxygen_${ProjectName}_pre_build COMMAND rm -rf ${CMAKE_CURRENT_BINARY_DIR}/html )
    add_dependencies(doxygen_${ProjectName} doxygen_${ProjectName}_pre_build)
  endif()
endmacro(add_forced_doxygen_target)

################ end macros and functions #############################################################################

################ library and compiler checks  #########################################################################
INCLUDE (CheckIncludeFileCXX)
include(TestCXXAcceptsFlag)
CHECK_INCLUDE_FILE_CXX("tr1/array" HAVE_TR1_ARRAY)
CHECK_INCLUDE_FILE_CXX("malloc.h" HAVE_MALLOC_H)


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

CHECK_CXX_SOURCE_COMPILES("
    int main(void)
    {
      int a __attribute__((unused)) = 0;
    };
"  HAS_WORKING_UNUSED_ATTRIBUTE
)

CHECK_CXX_SOURCE_COMPILES("
    #include <map>
    int main(void)
    {
      std::map<int, int> a;
      a.emplace(2, 2);
      return 0;
    };
"  HAVE_MAP_EMPLACE
)

set(DS_REQUIRED_BOOST_LIBS system thread filesystem date_time timer chrono)
FIND_PACKAGE(PkgConfig)
FIND_PACKAGE(Boost 1.48.0 COMPONENTS ${DS_REQUIRED_BOOST_LIBS} REQUIRED)
foreach(_boost_lib ${DS_REQUIRED_BOOST_LIBS})
    dune_register_package_flags(LIBRARIES boost_${_boost_lib})
endforeach(_boost_lib ${DS_REQUIRED_BOOST_LIBS})

find_package(Eigen3 3.2.0)
if(EIGEN3_FOUND)
  dune_register_package_flags(INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
  set(HAVE_EIGEN 1)
else(EIGEN3_FOUND)
  set(HAVE_EIGEN 0)
endif(EIGEN3_FOUND)

if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "k1om")
# intel mic and likwid don't mix
    include(FindLIKWID)
    find_package(LIKWID)
    if(LIKWID_FOUND)
        dune_register_package_flags(INCLUDE_DIRS ${LIKWID_INCLUDE_DIR}
            LIBRARIES ${LIKWID_LIBRARY})
    endif(LIKWID_FOUND)
else()
    set(HAVE_LIKWID 0)
endif()

include(DuneTBB)
################ end library and compiler checks  #####################################################################

################ misc vars  #########################################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(DS_MAX_MIC_THREADS CACHE INTEGER 120)
set(DUNE_XT_COMMON_TEST_DIR ${dune-xt-common_SOURCE_DIR}/dune/xt/test/)
set(DUNE_TEST_TIMEOUT 180 CACHE STRING "per-test timeout in seconds")
set(ENABLE_PERFMON 0 CACHE STRING "enable likwid performance monitoring API usage")
if(NOT DS_HEADERCHECK_DISABLE)
    set( ENABLE_HEADERCHECK 1 )
endif(NOT DS_HEADERCHECK_DISABLE)
