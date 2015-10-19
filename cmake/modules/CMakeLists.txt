# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
install(FILES   AddALUGridCompileFlags.cmake
                DuneStuffMacros.cmake
                DuneTBB.cmake
                DuneUtils.cmake
                FindEigen3.cmake
                FindFASP.cmake
                FindLIKWID.cmake
                FindTBB.cmake
                GridUtils.cmake
        DESTINATION ${DUNE_INSTALL_MODULEDIR})
