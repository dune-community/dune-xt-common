# ~~~
# This file is part of the dune-xt project:
#   https://github.com/dune-community/dune-xt
# Copyright 2009-2018 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2016 - 2017)
#   René Fritze     (2014 - 2015, 2017 - 2018)
#
# Searches for Intel's Thread Building Blocks library
#
# The following function is defined:
#
# add_dune_mpi_flags(targets)
#
# Adds the above flags and libraries to the specified targets.
# tries to link _debug libraries if cmake build mode matches DEBUG
# ~~~

find_package(TBB)

# add all TBB related flags to ALL_PKG_FLAGS, this must happen regardless of a target using add_dune_tbb_flags
if(TBB_FOUND)
  set_property(GLOBAL APPEND PROPERTY ALL_PKG_FLAGS "-DENABLE_TBB=1")
  foreach(dir ${TBB_INCLUDE_DIRS})
    set_property(GLOBAL APPEND PROPERTY ALL_PKG_FLAGS "-I${dir}")
  endforeach()
  set(HAVE_TBB 1)
else(TBB_FOUND)
  set(HAVE_TBB 0)
endif(TBB_FOUND)

function(add_dune_tbb_flags _targets)
  include_sys_dir("${TBB_INCLUDE_DIRS}")
  foreach(_target ${_targets})
    if(CMAKE_BUILD_TYPE MATCHES DEBUG)
      if(TBB_LIBRARY_DEBUG)
        target_link_libraries(${_target} ${TBB_LIBRARY_DEBUG})
      endif(TBB_LIBRARY_DEBUG)
      if(TBB_MALLOC_LIBRARY_DEBUG)
        target_link_libraries(${_target} ${TBB_MALLOC_LIBRARY_DEBUG})
      endif(TBB_MALLOC_LIBRARY_DEBUG)
    endif(CMAKE_BUILD_TYPE MATCHES DEBUG)
    target_link_libraries(${_target} "${TBB_LIBRARIES}")
  endforeach(_target)
endfunction(add_dune_tbb_flags)
