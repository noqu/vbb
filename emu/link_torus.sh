#!/bin/sh

# Create a torus configuration with NxN nodes, row offset x, col offset y, in dir
if [ $# -ne 4 ] ; then
    echo "link_torus.sh: Create quadratic shifted torus configs" >&2
    echo "" >&2
    echo "Usage: link_torus.sh <n> <x> <y> <dir>" >&2
    echo "  <n>: Number of lines/columns (total n x n nodes) " >&2
    echo "  <x>: Row offset (last of line i -> first of line (i + x) % n" >&2
    echo "  <y>: Column offset (last of col i -> first of col (i + y) % n" >&2
    echo "  <dir>: Links are created in this directory" >&2
    exit 1
fi

export N=$1
export N2=`expr $N \* $N`
export X=$2
export Y=$3
export DIR=$4

# Create or clear the destination directory
mkdir -p $DIR
rm -f $DIR/tr_*_ch_*_* $DIR/config.txt
echo "# Configuration for N=$N X=$X Y=$Y" > $DIR/config.txt

# Create num_ch channels for num_tr transputers
# Usage: make_channels num_tr num_ch
make_channels() {
    NUM_TR=$1
    NUM_CH=$2

    # Record config info
    echo "TP_NUM_TPS=$NUM_TR" >> $DIR/config.txt
    echo "TP_NUM_CHANS=$NUM_CH" >> $DIR/config.txt

    TR=0
    # Iterate transputers
    while [ $TR -lt $NUM_TR ]; do
        CH=0
        # Iterate channels
        while [ $CH -lt $NUM_CH ]; do
            # Create the in channel
            mkfifo ${DIR}/tr_${TR}_ch_${CH}_in
            CH=`expr $CH + 1`
        done
        TR=`expr $TR + 1`
    done
}

# Link out channel ch1 of transputer tr1 to in channel ch2 of tr2 (and vice versa)
# Usage: link_channels tr1 ch1 tr2 ch2
link_channels() {
    TR1=$1
    CH1=$2
    TR2=$3
    CH2=$4
    ln -s tr_${TR1}_ch_${CH1}_in ${DIR}/tr_${TR2}_ch_${CH2}_out
    ln -s tr_${TR2}_ch_${CH2}_in ${DIR}/tr_${TR1}_ch_${CH1}_out
}

# Return the number of the transputer that is logically to the right of i
# Usage: right_of i
right_of() {
    I=$1
    # Right edge ?
    if [ `expr $I % $N` -eq `expr $N - 1` ]; then
        # Start of line X lines down
        expr \( $I - \( $I % $N \) + $X \* $N \) % $N2
    else
        # Simply the right neighbor
        expr $I + 1
    fi
}

# Return the number of the transputer that is logically to the bottom of i
# Usage: bottom_of i
bottom_of() {
    I=$1
    # Bottom edge ?
    if [ `expr $I + $N` -ge $N2 ]; then
        # Top line Y columns to the right
        expr \( $I + $Y \) % $N
    else
        # Simply the node one line down
        expr $I + $N
    fi
}

# The cases where n x n is less than 9 are handled specially
if [ $N -eq 0 ]; then
    # 0x0 means single transputer: no channels at all
    make_channels 1 0
    echo "TP_RING_LAST=0" >> $DIR/config.txt
elif [ $N -eq 1 ]; then
    # 1x1 means a pair of transputers: just one channel
    make_channels 2 1
    link_channels 0 0 1 0
    echo "TP_RING_LAST=1" >> $DIR/config.txt
elif [ $N -eq 2 ]; then
    # 2x2 is too small for a torus - simply connect all
    make_channels 4 3
    # Build the outer square as the ring
    link_channels 0 0 1 1
    link_channels 1 0 2 1
    link_channels 2 0 3 1
    link_channels 3 0 0 1
    echo "TP_RING_LAST=3" >> $DIR/config.txt
    # And cross the corners
    link_channels 0 2 2 2
    link_channels 1 2 3 2
else
    # From 3 x 3 onwards, things are more regular
    make_channels $N2 4
    TR=0
    # Iterate transputers
    while [ $TR -lt $N2 ]; do
        # channel 0 goes to right neighbor's 1
        link_channels $TR 0 `right_of $TR` 1
        # channel 3 goes to bottom neighbor's 2
        link_channels $TR 3 `bottom_of $TR` 2
        # left neighbor of TP 0 is last in ring
        if [ `right_of $TR` -eq 0 ] ; then
            echo "TP_RING_LAST=$TR" >> $DIR/config.txt
        fi
        TR=`expr $TR + 1`
    done
fi
