#!/usr/bin/env python3
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
config = run_path(config_fn,init_globals=locals())

template = Template(open(tpl_fn).read())
with open(out_fn, 'w') as out:
    out.write(template.render(config=config, cache=cache))

