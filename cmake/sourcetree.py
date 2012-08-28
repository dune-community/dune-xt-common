#!/usr/bin/env python

import pygraph
from pygraph.classes.graph import graph
from pygraph.readwrite.dot import write
import pygraphviz as gv
from itertools import takewhile

is_dot = '.'.__eq__

        
class Node(object):
    def __init__(self, title, graph, **attrs):
        self.title = title
        self.parent = None
        self.children = []
        self.graph = graph
        try:
            graph.add_node(title,attrs)
        except pygraph.classes.exceptions.AdditionError:
            pass

    def add(self, child):
        self.children.append(child)
        child.parent = self
        try:
            self.graph.add_edge((self.title, child.title))
        except pygraph.classes.exceptions.AdditionError:
            pass
        
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
    root = outfile[:-len('.dep')].replace(strip_base, '')
    gr = graph()
    tree = Node('ROOT__' + root, gr, color='red')
    inserter = Inserter(tree,0)
    with open(outfile, 'rb') as gcc_out:
        for line in gcc_out:
            dots = len(list(takewhile(is_dot, line)))
            if dots < 1 or dots > 3:
                continue
            fn = line[dots+1:].replace(strip_base, '')
            #fn
            print fn
            inserter(fn, dots, gr)
    dot = write(gr)
    G = gv.AGraph(dot, strict=False,directed=True)
    prog = 'dot'
    G.layout(prog=prog)
    G.draw('%s.png'%outfile, prog=prog)
    return gr