#!/usr/bin/env python3
#
# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017 - 2018)
#
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# ~~~

import os
import sys
from runpy import run_path
from jinja2 import Template

from dune.xt.cmake import parse_cache

config_fn = sys.argv[1]
tpl_fn = sys.argv[2]
cmake_binary_dir = sys.argv[3]
out_fn = sys.argv[4]
backup_bindir = sys.argv[5]

try:
    cache, _ = parse_cache(os.path.join(cmake_binary_dir, 'CMakeCache.txt'))
except FileNotFoundError:
    cache, _ = parse_cache(os.path.join(backup_bindir, 'CMakeCache.txt'))
sys.path.append(os.path.dirname(config_fn))
config = run_path(config_fn,init_globals=locals(), run_name='__dxt_codegen__')

dir_base = os.path.dirname(out_fn)
if not os.path.isdir(dir_base):
    os.makedirs(dir_base)
template = Template(open(tpl_fn).read())

try:
  for postfix, cfg in config['multi_out'].items():
    fn = '{}.{}'.format(out_fn, postfix)
    with open(fn, 'w') as out:
        out.write(template.render(config=cfg, cache=cache))
except KeyError:
    with open(out_fn, 'w') as out:
        out.write(template.render(config=config, cache=cache))
