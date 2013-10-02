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

SET( CMAKE_EXPORT_COMPILE_COMMANDS "ON" )

SET( CMAKE_CXX_FLAGS_RELEASE
  "-DDNDEBUG -DNDEBUG -O2 -fPIC " )

SET( CMAKE_CXX_FLAGS_DEBUG
  "-O0 -DDNDEBUG -g3 -ggdb  -Wunused-variable -Winline -fno-strict-aliasing -fPIC -Wall -Wextra -Wc++0x-compat -Wparentheses -pedantic -Wredundant-decls -Wshadow  -Wundef -Wnon-virtual-dtor" )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set( CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wnon-literal-null-conversion -Wused-but-marked-unused -Wno-tautological-compare -Wfloat-equal -Wdisabled-macro-expansion -Wcovered-switch-default  -Wswitch-enum -Wunreachable-code  -Wshorten-64-to-32"
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set( CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wlogical-op"
        )
endif()

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
if(EIGEN_FOUND)
  include_directories(${EIGEN_INCLUDE_DIRS})
endif(EIGEN_FOUND)

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
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi -pthread -DMPIPP_H -DENABLE_MPI=1" )
      include_directories(${MPI_CXX_INCLUDE_PATH})
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS} -pthread -L/usr/lib/openmpi/lib -lmpi -lopen-rte -lopen-pal -ldl -Wl,--export-dynamic -lnsl -lutil -lm -ldl")
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