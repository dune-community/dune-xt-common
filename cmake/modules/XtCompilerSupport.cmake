# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2016 - 2018)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# ~~~

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
            # due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129
            # we have to filter what to sys-include includes
            if(${ARG} MATCHES "/usr/include")
                message(AUTHOR_WARNING "-isystem not supported for ${ARG}")
            else()
                add_definitions("-isystem ${_idir}")
            endif()
        ELSE(IS_DIRECTORY ${ARG} )
            MESSAGE( STATUS "Include directory ${ARG} does not exist" )
        ENDIF(IS_DIRECTORY ${ARG} )
    ENDFOREACH( ARG )
ENDMACRO(INCLUDE_SYS_DIR)

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

