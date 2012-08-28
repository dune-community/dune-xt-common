#!/usr/bin/env python

import sys
import subprocess
import sourcetree
import pprint
from pygraph.algorithms.cycles import find_cycle

fn = sys.argv[-1]
strip_base = sys.argv[-2]
with open(fn,'wb') as out:
    p = subprocess.Popen(sys.argv[1:-3], shell=False, stdout=out, stderr=out)
    errcode = p.wait()
print('STRIP %s'%strip_base)
graph = sourcetree.parse_file(strip_base, fn)
cycle_nodes = find_cycle(graph)
if cycle_nodes:
    with open(fn+'.cycles','wb') as out:
       pp = pprint.PrettyPrinter()
       pp.pprint(cycle_nodes, out)

