# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   René Fritze     (2016 - 2018)
#   Tobias Leibner  (2018)
# ~~~

macro(ADD_IF_SUPPORTED dest)
  foreach(flag ${ARGN})
    check_cxx_accepts_flag("${flag}" has_${flag})
    if(has_${flag})
      set(${dest} "${${dest}} ${flag}")
    else(has_${flag})
      message("compiler doesn't support: ${flag}")
    endif(has_${flag})
  endforeach(flag ${ARGN})
endmacro(ADD_IF_SUPPORTED)

macro(INCLUDE_SYS_DIR)
  foreach(ARG ${ARGN})
    if(IS_DIRECTORY ${ARG})
      include_directories(${ARG}) # due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129  we have to filter what
                                  # to sys-include includes
      if(${ARG} MATCHES "/usr/include")
        message(AUTHOR_WARNING "-isystem not supported for ${ARG}")
      else()
        add_definitions("-isystem ${ARG}")
      endif()
    else(IS_DIRECTORY ${ARG})
      message(STATUS "Include directory ${ARG} does not exist")
    endif(IS_DIRECTORY ${ARG})
  endforeach(ARG)
endmacro(INCLUDE_SYS_DIR)

include(CheckIncludeFileCXX)
include(TestCXXAcceptsFlag)
check_include_file_cxx("tr1/array" HAVE_TR1_ARRAY)
check_include_file_cxx("malloc.h" HAVE_MALLOC_H)

check_cxx_source_compiles("
   int main(void)
   {
     auto f = [&] (){ return 0; };
     return 0;
   };
" HAS_LAMBDA_FUNCTIONS)

check_cxx_source_compiles("
    #include <vector>
    #include <iterator>
    int main(void)
    {
      std::vector<int> a;
      std::vector<int>::const_iterator b = std::begin(a);
      std::vector<int>::const_iterator e = std::end(a);
      return 0;
    };
" HAS_STD_BEGIN_END)

check_cxx_source_compiles("
    int main(void)
    {
      int a __attribute__((unused)) = 0;
    };
" HAS_WORKING_UNUSED_ATTRIBUTE)

check_cxx_source_compiles("
    #include <map>
    int main(void)
    {
      std::map<int, int> a;
      a.emplace(2, 2);
      return 0;
    };
" HAVE_MAP_EMPLACE)
