# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2014)
#   Rene Milk       (2012 - 2013, 2015)

install(FILES   AddALUGridCompileFlags.cmake
                DuneXtCommonMacros.cmake
                DuneTBB.cmake
                DuneUtils.cmake
                FindEigen3.cmake
                FindFASP.cmake
                FindLIKWID.cmake
                FindTBB.cmake
                GridUtils.cmake
        DESTINATION ${DUNE_INSTALL_MODULEDIR})
