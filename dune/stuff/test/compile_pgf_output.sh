#!/bin/sh
#
# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Rene Milk (2012, 2015)

make grid_output_pgf
./grid_output_pgf
find . -name "*.tex" | xargs echo pdflatex