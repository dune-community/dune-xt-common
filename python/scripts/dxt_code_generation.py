#!/usr/bin/env python3
#
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017)

import os
import sys
from runpy import run_path
from jinja2 import Template

from dune.xt.cmake import parse_cache

config_fn = sys.argv[1]
tpl_fn = sys.argv[2]
cmake_binary_dir = sys.argv[3]
out_fn = sys.argv[4]
cache, _ = parse_cache(os.path.join(cmake_binary_dir, 'CMakeCache.txt'))
sys.path.append(os.path.dirname(config_fn))
config = run_path(config_fn,init_globals=locals(), run_name='__dxt_codegen__')

template = Template(open(tpl_fn).read())
with open(out_fn, 'w') as out:
    out.write(template.render(config=config, cache=cache))

