# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   René Fritze     (2018)
# ~~~

from importlib import import_module

# This is here on purpose, needs to be imported as early as possible!
try:
    from mpi4py import MPI
except ImportError:
    pass

# empty is missing here on purpose!
_modules = (
    '_common',
    '_logging',
    '_timings',
    )

# see https://stackoverflow.com/questions/43059267/how-to-do-from-module-import-using-importlib
for mod_name in _modules:
    try:
        mod = import_module('.{}'.format(mod_name), 'dune.xt.common')
        if "__all__" in mod.__dict__:
            names = mod.__dict__["__all__"]
        else:
            # otherwise we import all names that don't begin with _
            names = [x for x in mod.__dict__ if not x.startswith("_")]
        globals().update({k: getattr(mod, k) for k in names})
    except ImportError as e:
        import os
        import logging
        if os.environ.get('DXT_PYTHON_DEBUG', False):
            raise e
        logging.error('dune-xt-common: could not import {} module'.format(mod_name))

