# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze (2018)
# ~~~

dune_pybindxi_add_module(logging EXCLUDE_FROM_ALL logging.cc)
dune_pybindxi_add_module(timedlogging EXCLUDE_FROM_ALL timedlogging.cc)

dune_pybindxi_add_module(_empty EXCLUDE_FROM_ALL empty.cc)
dune_pybindxi_add_module(_exceptions EXCLUDE_FROM_ALL exceptions.cc)
dune_pybindxi_add_module(_mpi EXCLUDE_FROM_ALL mpi.cc)
dune_pybindxi_add_module(_timings EXCLUDE_FROM_ALL timings.cc)
