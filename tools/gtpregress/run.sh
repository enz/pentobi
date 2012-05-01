#!/bin/bash
# Runs test GTP files using gogui-regress (http://gogui.sf.net)

PENTOBI_DBG="../../build/dbg/src/pentobi_gtp/pentobi-gtp"
PENTOBI_OPT="../../build/opt/src/pentobi_gtp/pentobi-gtp"
TESTS=\
symmetry.tst

usage() {
    cat >&2 <<EOF
Usage: $0 [options]

Options:
  -d Use debug version
  -h Print help and exit
  -l Long output
EOF
}

DEBUG=0
LONG_OUTPUT=0
while getopts "dhl" OPT; do
case "$OPT" in
    d) DEBUG=1;;
    h) usage; exit 0;;
    l) LONG_OUTPUT=1;;
    [?]) usage; exit 1;;
esac
done

if [[ $DEBUG == 0 ]]; then
    PENTOBI="$PENTOBI_OPT"
else
    PENTOBI="$PENTOBI_DBG"
fi

mkdir -p html
if [[ $LONG_OUTPUT != 0 ]]; then
    OPTIONS="-long"
fi
gogui-regress $OPTIONS -output html "$PENTOBI" $TESTS
RESULT=$?
if [[ $RESULT == 0 ]]; then
    echo "OK"
fi
exit $RESULT
