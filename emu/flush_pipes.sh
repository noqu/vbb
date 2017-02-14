#!/bin/sh

if [ $# -ne 1 ] ; then
    echo "Usage: flush_pipes.sh <dir>" >&2
    exit 1
fi

# Just for paranoia, read once from all named pipes, to make sure nothing 
# was left from previous runs (unlikely but possible)
DIR=$1
for PIPE in `/bin/ls $DIR/tr_*_ch_*_in 2>/dev/null`; do
    /bin/dd if=$PIPE of=/dev/null iflag=nonblock > /dev/null 2>&1
done
