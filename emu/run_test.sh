#!/bin/sh

if [ $# -lt 2 ]; then
    echo "Usage: run_test.sh <link_dir> <program> ..." >&2
    exit 1
fi
LINK_DIR=$1
PROGRAM=$2
shift 2

# Just for paranoia, read once from all named pipes, to make sure nothing 
# was left from previous runs (unlikely but possible)
for PIPE in `/bin/ls $LINK_DIR/tr_*_ch_*_in 2>/dev/null`; do
    /bin/dd if=$PIPE of=/dev/null iflag=nonblock > /dev/null 2>&1
done

# Read number of transputers from config file
CONFIG="$LINK_DIR/config.txt"
if [ ! -r "$CONFIG" ] ; then
    echo "Config file $CONFIG not found" >&2
    exit 1
fi
eval `grep '^TP_NUM_TPS=[0-9]*' $CONFIG`

# Start the appropriate number of processes
I=0
while [ $I -lt $TP_NUM_TPS ] ; do
    $PROGRAM @$CONFIG TP_PIPE_DIR=$LINK_DIR TP_SELF=$I TP_PGID=$$ $* &
    I=`expr $I + 1`
done

wait
