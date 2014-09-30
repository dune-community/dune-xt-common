# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# Copyright holders: Rene Milk, Felix Schindler
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

SET(GRIDTYPE
    "SGRID"
    CACHE STRING "GRIDTYPE")

SET(GRIDDIM
    "2"
    CACHE STRING "GRIDDIM")

add_definitions("-DGRIDDIM=${GRIDDIM}")
add_definitions("-DGRIDTYPE=${GRIDTYPE}")
add_definitions("-D${GRIDTYPE}")

if(ALUGRID_FOUND)
  add_definitions("-DENABLE_ALUGRID=1")
endif(ALUGRID_FOUND)
