# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   René Fritze     (2016, 2018)
# ~~~

# Note: This import makes sure that metis is imported before python bindings from dune-xt. Importing dune-xt python
# bindings at any point before metis will cause a segfault due to misscommunication between libmetis5 and
# libscotchmetis.

try:
	import metis
except ImportError:
	pass


__import__('pkg_resources').declare_namespace(__name__)


from importlib import import_module
import os
import logging


_init_mpi_calls = set()
_init_logger_calls = set()
_test_logger_calls = set()


def guarded_import(globs, base_name, mod_name):
    # see https://stackoverflow.com/questions/43059267/how-to-do-from-module-import-using-importlib
    try:
        mod = import_module('.{}'.format(mod_name), base_name)
        if "__all__" in mod.__dict__:
            names = mod.__dict__["__all__"]
        else:
            names = [x for x in mod.__dict__ if not x.startswith("_")]
        # import special init functions which should be present in every module
        if '_init_mpi' in mod.__dict__:
            _init_mpi_calls.add(mod.__dict__['_init_mpi'])
        if '_init_logger' in mod.__dict__:
            _init_logger_calls.add(mod.__dict__['_init_logger'])
        if '_test_logger' in mod.__dict__:
            _test_logger_calls.add(mod.__dict__['_test_logger'])
        # check the rest for duplicity
        for nm in names:
            if nm in globs:
                logging.error(
                        '{}: overwriting existing name \'{}\' when importing from \'{}\' (continuing anyway)!'.format(
                            base_name, nm, mod_name))
        # and finally import
        globs.update({k: getattr(mod, k) for k in names})
    except ImportError as e:
        logging.error('{}: could not import module \'{}\' (continuing anyway)!'.format(base_name, mod_name))
        if os.environ.get('DXT_PYTHON_DEBUG', False):
            raise e


def init_mpi(args=list()):
    for call in _init_mpi_calls:
        call(args)


def init_logger(max_info_level=999,
                max_debug_level=999,
                enable_warnings=True,
                enable_colors=True,
                info_color='blue',
                debug_color='darkgray',
                warning_color='red'):
    for call in _init_logger_calls:
        call(max_info_level, max_debug_level, enable_warnings, enable_colors, info_color, debug_color, warning_color)


def test_logger(info=True, debug=True, warning=True):
    for call in _test_logger_calls:
        call(info, debug, warning)

