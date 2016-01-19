# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2014 - 2016)
#   Rene Milk       (2012 - 2015)
#   Tobias Leibner  (2015)

enable_testing()

DEPENDENCYCHECK( ${xtcommon} )

set_source_files_properties( ${DUNE_HEADERS} PROPERTIES HEADER_FILE_ONLY 1 )



install(DIRECTORY gtest
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dune/
        FILES_MATCHING PATTERN "*.hh")

BEGIN_TESTCASES(dunextcommon)
 
END_TESTCASES()
