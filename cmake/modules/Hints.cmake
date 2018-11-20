# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze    (2018)
#   Tobias Leibner (2018)
# ~~~

macro(append_to_each INPUTLIST POSTFIX OUTPUTLIST)
  foreach(ENTRY ${INPUTLIST})
    list(APPEND ${OUTPUTLIST} ${ENTRY}${POSTFIX})
  endforeach(ENTRY ${INPUTLIST})
endmacro()

set(hint_prefixes
    "${CMAKE_SOURCE_DIR}/../local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-minimal/local/"
    "${CMAKE_SOURCE_DIR}/../environments/debian-full/local/"
    "$ENV{HOME}/"
    "$ENV{HOME}/Software/")

set(bin_hints "")
append_to_each("${hint_prefixes}" "bin/" bin_hints)

set(lib_hint "")
append_to_each("${hint_prefixes}" "lib/" lib_hints)

set(include_hints "")
append_to_each("${hint_prefixes}" "include/" include_hints)
