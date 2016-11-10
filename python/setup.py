#!/usr/bin/env python

import sys
from setuptools import setup

setup(name='dune.xt',
      version='2.4',
      namespace_packages=['dune'],
      description='Python for Dune-Xt',
      author='The dune-xt devs',
      author_email='dune-xt-dev@listserv.uni-muenster.de',
      url='https://github.com/dune-community/dune-xt-common',
      packages=['dune.xt'],
      install_requires=['binpacking==1.3, jinja2'],
      scripts=['./scripts/generate_compare_functions.py',
               './scripts/distribute_testing.py',
               './wrapper/dune_xt_execute.py'])
