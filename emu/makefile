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
clean:
	rm -f obj/*.o test_emu 2>/dev/null || true
	rm -f emulation.a 2>/dev/null || true
	rm -f links/tr_*_ch_*_* links/config.txt 2>/dev/null || true
	rmdir links 2>/dev/null || true

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

