# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

add_subdirectory(stuff)

install(DIRECTORY stuff
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/
        FILES_MATCHING PATTERN "*.hh")
#         EXCLUDE PATTERN "stuff/test/*")
