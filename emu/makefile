#------------------------------------------------------------
# MAKEFILE for the minimal emulation of a transputer
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 31.01.2017
#------------------------------------------------------------
MYCC=cc
OBJ=obj
SRC=src
#------------------------------------------------------------
CFLAGS=-g -pthread -I../par/src
LFLAGS=-g -pthread
#------------------------------------------------------------
all : \
    emulation.a \
    test_emu \
    torus_links \
    bin_scripts \
    ;
#------------------------------------------------------------
emulation.a : \
    $(OBJ)/channel.o \
    $(OBJ)/process.o \
    $(OBJ)/misc.o \
    ;
	ar rc emulation.a \
	    $(OBJ)/channel.o \
	    $(OBJ)/process.o \
	    $(OBJ)/misc.o \
	    ;
	ranlib emulation.a
#------------------------------------------------------------
torus_links:
	./link_torus.sh 0 0 0 torus/1
	./link_torus.sh 1 0 0 torus/2
	./link_torus.sh 2 0 0 torus/4
	./link_torus.sh 3 1 1 torus/9
	./link_torus.sh 4 3 2 torus/16
	./link_torus.sh 5 3 4 torus/25
	./link_torus.sh 6 5 3 torus/36
	./link_torus.sh 7 4 5 torus/49
	./link_torus.sh 8 5 5 torus/64
bin_scripts:
	./make_bin_scripts.sh ../bin tsp 1 2 4 9 16 25 36 49 64
	./make_bin_scripts.sh ../bin scp 1 2 4 9 16 25 36 49 64
	./make_bin_scripts.sh ../bin msp 1 2 4 9 16 25 36 49 64
#------------------------------------------------------------
clean:
	rm -f obj/*.o test_emu 2>/dev/null || true
	rm -f emulation.a 2>/dev/null || true
	rm -f torus/*/tr_*_ch_*_* torus/*/config.txt 2>/dev/null || true
	rmdir torus/* 2>/dev/null || true
	rmdir torus 2>/dev/null || true
	rm -f ../bin/tsp.*t ../bin/scp.*t ../bin/msp.*t 2>/dev/null || true
#------------------------------------------------------------
$(OBJ)/channel.o : \
    $(SRC)/channel.c \
    $(SRC)/channel.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/channel.c 
	@mv channel.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/process.o : \
    $(SRC)/process.c \
    $(SRC)/process.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/process.c 
	@mv process.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/misc.o : \
    $(SRC)/misc.c \
    $(SRC)/misc.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/misc.c 
	@mv misc.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/test_emu.o : \
    $(SRC)/test_emu.c \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/test_emu.c 
	@mv test_emu.o $(OBJ)
#------------------------------------------------------------
test_emu: \
    $(OBJ)/test_emu.o \
    emulation.a \
    ../par/par_kernel.a \
    ;
	$(MYCC) $(LFLAGS) \
	$(OBJ)/test_emu.o \
	emulation.a \
	../par/par_kernel.a \
	-o test_emu
#------------------------------------------------------------

