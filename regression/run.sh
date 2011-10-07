#!/bin/sh
# Runs test GTP files using gogui-regress (http://gogui.sf.net)

PENTOBI=../build/opt/src/pentobi_gtp/pentobi-gtp
TESTS=\
symmetry.tst

mkdir -p html
gogui-regress -output html "$PENTOBI" $TESTS
