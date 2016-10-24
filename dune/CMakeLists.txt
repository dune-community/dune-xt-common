# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2014, 2016)
#   Rene Milk       (2012, 2015)

add_subdirectory(xt)

install(DIRECTORY xt
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/
        FILES_MATCHING PATTERN "*.hh")

install(FILES
        __init__.py
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/dune/)
