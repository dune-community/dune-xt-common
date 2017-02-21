# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2016)
#
# Use this module by invoking find_package with the form::
#
#   find_package(ClangFormat
#     [version] # Minimum version e.g. 3.7
#     )
#
#   optionally pass the minimal version you require like so find_package(ClangFormat 3.7)
#   this module set ClangFormat_EXECUTABLE, ClangFormat_VERSION
#   and ClangFormat_FOUND accordingly

find_program(ClangFormat_EXECUTABLE NAMES clang-format clang-format-3.6 clang-format-3.7 clang-format-3.8 clang-format-3.9 clang-format-4.0)
if(EXISTS ${ClangFormat_EXECUTABLE})
  execute_process(COMMAND ${ClangFormat_EXECUTABLE} -version OUTPUT_VARIABLE clang_out )
  string(REGEX REPLACE ".*clang-format version ([0-9]+\\.[0-9]+).*"  "\\1" ClangFormat_VERSION ${clang_out})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ClangFormat 
    REQUIRED_VARS ClangFormat_EXECUTABLE
    VERSION_VAR ClangFormat_VERSION
    )

