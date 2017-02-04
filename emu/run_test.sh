#!/bin/sh

# Configure two transputers, connected by 1 channel
mkdir -p links
rm -f links/tr_*_ch_*_* links/config.txt
mkfifo links/tr_0_ch_0_in
mkfifo links/tr_1_ch_0_in
ln -s tr_0_ch_0_in links/tr_1_ch_0_out
ln -s tr_1_ch_0_in links/tr_0_ch_0_out

# Dump fixed part of configuration into file
echo "
TP_RING_LAST=1
TP_NUM_TPS=2
TP_NUM_CHANS=1
TP_PIPE_DIR=links
TP_PGID=$$
" > links/config.txt

# Run two instances of test_emu
./test_emu @links/config.txt TP_SELF=0 &
./test_emu @links/config.txt TP_SELF=1 &

wait
