#!/bin/sh

if [ $# -lt 2 ] ; then
    echo "Usage: make_bin_scripts <dir> <prob> ..." >&2
    exit 1
fi

DIR=$1
PROB=$2
shift 2

for I in $*; do
    SCRIPT=$DIR/$PROB.${I}t
    echo '#!/bin/sh' > $SCRIPT
    echo '' >> $SCRIPT
    echo 'BIN=`dirname $0`' >> $SCRIPT
    echo 'EMU=$BIN/../emu' >> $SCRIPT
    echo 'exec $EMU/run_test.sh $EMU/torus/'$I' $BIN/'$PROB'.par $*' >> $SCRIPT
    chmod +x $SCRIPT
done
