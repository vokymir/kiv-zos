#!/bin/bash

# Always operate from the script's own directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# it must be run at least two times to resolve references
pdflatex doc.tex

# placeholder until bibtex is used
echo "No bibtex used."

pdflatex doc.tex
pdflatex doc.tex

# move final pdf from src directory
mv doc.pdf ../doc.pdf
