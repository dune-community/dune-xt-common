# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Rene Milk (2018)
# ~~~

from pkg_resources import resource_filename, resource_stream
import pkgutil
import logging
import pprint


def load_all_submodule(module):
    ignore_playground = True
    fails = []
    for _, module_name, _ in pkgutil.walk_packages(module.__path__, module.__name__ + '.',
                                                   lambda n: fails.append((n, ''))):
        if ignore_playground and 'playground' in module_name:
            continue
        try:
            __import__(module_name, level=0)
        except (TypeError, ImportError) as t:
            fails.append((module_name, t))
    if len(fails) > 0:
        logging.getLogger(module.__name__).fatal('Failed imports: {}'.format(pprint.pformat(fails)))
        raise ImportError(module.__name__)


def runmodule(filename):
    import pytest
    sys.exit(pytest.main(sys.argv[1:] + [filename]))
