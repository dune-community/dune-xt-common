# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#
# Contributors: Sven Kaulmann

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

include(CheckOverrideFinal)

SET( CMAKE_EXPORT_COMPILE_COMMANDS "ON" )

SET( CMAKE_CXX_FLAGS_RELEASE CACHE STRING
  "-DDNDEBUG -DNDEBUG -O2 -fPIC ${CMAKE_CXX_FLAGS_RELEASE}")

# exp grid extensions needed for boundaryId stuff
SET( CMAKE_CXX_FLAGS "-DDUNE_GRID_EXPERIMENTAL_GRID_EXTENSIONS=1 ${CMAKE_CXX_FLAGS}" )

SET( CMAKE_CXX_FLAGS_DEBUG
  "-DDNDEBUG -g3 -ggdb  -Wunused-variable -Winline -fno-strict-aliasing -fPIC -Wall -Wextra -Wc++0x-compat -Wparentheses -pedantic -Wredundant-decls -Wshadow  -Wundef -Wnon-virtual-dtor -ftemplate-backtrace-limit=0 ${CMAKE_CXX_FLAGS_DEBUG}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set( CMAKE_CXX_FLAGS_DEBUG CACHE STRING
        "-Wnon-literal-null-conversion -Wused-but-marked-unused -Wno-tautological-compare -Wfloat-equal -Wdisabled-macro-expansion -Wcovered-switch-default  -Wswitch-enum -Wunreachable-code  -Wshorten-64-to-32 -O0 ${CMAKE_CXX_FLAGS_DEBUG}"
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set( CMAKE_CXX_FLAGS_DEBUG CACHE STRING
        "-Wlogical-op -Og ${CMAKE_CXX_FLAGS_DEBUG} "
        )
endif()

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

FIND_PACKAGE( PkgConfig )
FIND_PACKAGE(Boost 1.48.0 COMPONENTS system thread filesystem date_time timer chrono REQUIRED)
foreach(_lib ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} 
                ${Boost_THREAD_LIBRARY} ${Boost_TIMER_LIBRARY} 
                ${Boost_DATE_TIME_LIBRARY} ${Boost_CHRONO_LIBRARY})
list(APPEND DUNE_DEFAULT_LIBS "${_lib}")
endforeach(_lib)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#dune-common doesn't add it's deps correctly atm
if(HAVE_LAPACK)
	list(APPEND DUNE_DEFAULT_LIBS ${LAPACK_LIBRARIES})
endif(HAVE_LAPACK)
if(HAVE_BLAS)
	list(APPEND DUNE_DEFAULT_LIBS ${BLAS_LIBRARIES})
endif(HAVE_BLAS)

pkg_check_modules(EIGEN eigen3)
set(CMAKE_REQUIRED_INCLUDES ${EIGEN_INCLUDE_DIRS})
check_include_file_cxx("Eigen/SparseLU" HAVE_EIGEN_SPARSE)
if(EIGEN_FOUND AND HAVE_EIGEN_SPARSE)  
  include_directories(${EIGEN_INCLUDE_DIRS})
  set(HAVE_EIGEN 1)
else(EIGEN_FOUND AND HAVE_EIGEN_SPARSE)
  set(HAVE_EIGEN 0)
endif(EIGEN_FOUND AND HAVE_EIGEN_SPARSE)

pkg_check_modules(LIBAMA libama)
if(LIBAMA_FOUND)
        include_directories(${LIBAMA_INCLUDE_DIRS})
        find_library(LIBAMA_LIBRARY NAMES ama
                HINTS ${LIBAMA_LIBDIR} ${LIBAMA_LIBRARY_DIRS})
        list(APPEND DUNE_DEFAULT_LIBS ${LIBAMA_LIBRARY})
endif(LIBAMA_FOUND)

include(FindLIKWID)
find_package(LIKWID)
if(LIKWID_FOUND)
  list(APPEND DUNE_DEFAULT_LIBS "${LIKWID_LIBRARY}")
  include_directories(${LIKWID_INCLUDE_DIR})
endif(LIKWID_FOUND)
set(ENABLE_PERFMON 0 CACHE STRING "enable likwid performance monitoring API usage")


# include(FindFASP)

if(ENABLE_MPI)
  find_package(MPI REQUIRED)
  if(MPI_FOUND)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      message(STATUS "Enabling mpi features, mac style!")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -pthread -DMPIPP_H -DENABLE_MPI=1" )
      include_directories(${MPI_CXX_INCLUDE_PATH})
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS} -pthread")
      LIST( APPEND PARALIBS ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
    else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      message(STATUS "Enabling mpi features")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -pthread -DMPIPP_H -DENABLE_MPI=1" )
      include_directories(${MPI_CXX_INCLUDE_PATH})
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS} -pthread -lopen-rte -lopen-pal -ldl -Wl,--export-dynamic -lnsl -lutil -lm -ldl")
      LIST( APPEND PARALIBS ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY} parmetis metis)
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  else(MPI_FOUND)
    message(FATAL "mpi requested but not found")
  endif(MPI_FOUND)
endif(ENABLE_MPI)

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
    find_program(FORMAT NAMES clang-format clang-format-3.4)
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
