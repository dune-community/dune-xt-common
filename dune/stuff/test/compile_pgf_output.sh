#!/bin/sh

make grid_output_pgf
./grid_output_pgf
find . -name "*.tex" | xargs echo pdflatex