#!/usr/bin/env python
#
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2016)
#   Rene Milk       (2012, 2015)

import pygraphviz as gv
import networkx as nx
import matplotlib.pyplot as plt
from itertools import takewhile

is_dot = '.'.__eq__

class Node(object):
    def __init__(self, title, graph, attrs={}):
        self.title = title
        self.parent = None
        self.children = []
        self.graph = graph
        graph.add_node(title,attrs)

    def add(self, child):
        self.children.append(child)
        child.parent = self
        self.graph.add_edge(self.title, child.title)
        
# *** Node insertion logic ***
class Inserter(object):
    def __init__(self, node, depth = 0):
        self.node = node
        self.depth = depth

    def __call__(self, title, depth, graph):
        newNode = Node(title, graph)
        if (depth > self.depth):
            self.node.add(newNode)
            self.depth = depth
        elif (depth == self.depth):
            self.node.parent.add(newNode)
        else:
            parent = self.node.parent
            for i in xrange(0, self.depth - depth):
                parent = parent.parent
            parent.add(newNode)
            self.depth = depth
        self.node = newNode
        
def parse_file(strip_base, outfile, maxdepth=3):
    root = outfile[:-len('.dep')].replace(strip_base, '').strip()
    gr = nx.DiGraph()
    tree = Node(root, gr, {'color':'red'})
    inserter = Inserter(tree,0)
    with open(outfile, 'rb') as gcc_out:
        for line in gcc_out:
            dots = len(list(takewhile(is_dot, line)))
            if dots < 1 or dots > maxdepth:
                continue
            fn = line[dots+1:].replace(strip_base, '').strip()
            inserter(fn, dots, gr)
    return (root, gr)

def render_graph((root, gr)):
    fig = plt.gcf()
    fig.set_size_inches(28.5,28.5)
    nx.draw_graphviz(gr)
    plt.savefig('include_graph_%s.png'%root)
    return gr
