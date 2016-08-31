# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2014, 2016)
#   Rene Milk       (2012 - 2013, 2015)

install(FILES   AddALUGridCompileFlags.cmake
                DuneTBB.cmake
                DuneUtils.cmake
                DuneXtCommonMacros.cmake
                FindClangFormat.cmake
                FindClangTidy.cmake
                FindEigen3.cmake
                FindFASP.cmake
                FindLIKWID.cmake
                FindTBB.cmake
                GridUtils.cmake
                XtCompilerSupport.cmake
                XtTooling.cmake
        DESTINATION ${DUNE_INSTALL_MODULEDIR})
