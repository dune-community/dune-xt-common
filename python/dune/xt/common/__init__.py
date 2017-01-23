# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2016 - 2017)
#   Rene Milk       (2015)

from importlib import import_module

try:
    from mpi4py import MPI
except ImportError:
    pass


def init_logger(max_info_level=-1,
                max_debug_level=-1,
                enable_warnings=True,
                enable_colors=True,
                info_color='blue',
                debug_color='darkgray',
                warning_color='red'):
    from ._common import init_logger as _init_logger
    initializers = [_init_logger]
    for module_name in ('xt.la', 'xt.grid', 'xt.functions', 'gdt'):
        try:
            mm = import_module('dune.{}'.format(module_name))
            initializers.append(mm.init_logger)
        except ModuleNotFoundError:
            pass
    for initializer in initializers:
        initializer(max_info_level, max_debug_level, enable_warnings, enable_colors, info_color, debug_color,
                        warning_color)


def init_mpi(args=list()):
    try:
        from dune.gdt import init_mpi as _init_mpi
    except ModuleNotFoundError:
        from dune.xt.common import init_mpi as _init_mpi
    _init_mpi(args)


from ._common import *

