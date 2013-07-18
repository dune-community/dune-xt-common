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


SET( CMAKE_CXX_FLAGS_RELEASE
  "-DDNDEBUG -O2 -fPIC " )

SET( CMAKE_CXX_FLAGS_DEBUG
  "-O0 -DDNDEBUG -g3 -ggdb -Wall -Wextra -Wc++0x-compat -Wparentheses -pedantic -Wredundant-decls -Wshadow -Wunused-variable -Winline -fno-strict-aliasing -Wundef -Wnon-virtual-dtor -fPIC " )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set( CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wdocumentation -Wshorten-64-to-32 -Wused-but-marked-unused -Wdisabled-macro-expansion -Wcovered-switch-default  -Wfloat-equal -Wswitch-enum -Wunreachable-code -Wnon-literal-null-conversion "
        CACHE STRING
        "Flags used by the compiler during debug builds.")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set( CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wlogical-op"
        CACHE STRING
        "Flags used by the compiler during debug builds.")
endif()

FIND_PACKAGE( PkgConfig )
FIND_PACKAGE(Boost 1.48.0 COMPONENTS system thread filesystem date_time timer REQUIRED)
foreach(_lib ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} 
                ${Boost_THREAD_LIBRARY} ${Boost_TIMER_LIBRARY} 
                ${Boost_DATE_TIME_LIBRARY} ${Boost_CHRONO_LIBRARY})
list(APPEND DUNE_DEFAULT_LIBS "${_lib}")
endforeach(_lib)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#find_package(SuperLU)
#if(SUPERLU_FOUND)
  #include_directories(${SUPERLU_INCLUDES})
  #link_directories(${SUPERLU_LIBRARY_DIRS})
#endif(SUPERLU_FOUND)

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
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -w -pthread -DMPIPP_H -DENABLE_MPI=1" )
      include_directories(${MPI_CXX_INCLUDE_PATH})
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_CXX_LINK_FLAGS} -pthread")
      LIST( APPEND PARALIBS ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
    else(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      message(STATUS "Enabling mpi features")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS} -w -I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi -pthread -DMPIPP_H -DENABLE_MPI=1" )
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
