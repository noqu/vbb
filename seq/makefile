#------------------------------------------------------------
# MAKEFILE fuer sequentielle Kern-Library auf Linux
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.5.93
#------------------------------------------------------------
MYCC=cc
OBJ=obj
SRC=src
#------------------------------------------------------------
CFLAGS=-O
#------------------------------------------------------------
seq_kernel.a : \
    $(OBJ)/startup.o \
    $(OBJ)/heap.o \
    $(OBJ)/mem.o \
    $(OBJ)/expand.o \
    $(OBJ)/read.o \
    $(OBJ)/set.o \
    $(OBJ)/trace.o \
    ;
	ar rc seq_kernel.a \
	    $(OBJ)/startup.o \
	    $(OBJ)/heap.o \
	    $(OBJ)/mem.o \
	    $(OBJ)/expand.o \
	    $(OBJ)/read.o \
	    $(OBJ)/set.o \
	    $(OBJ)/trace.o \
	    ;
	ranlib seq_kernel.a
#------------------------------------------------------------
clean:
	rm -f obj/*.o 2>/dev/null
	rm -f seq_kernel.a 2>/dev/null
#------------------------------------------------------------
$(OBJ)/startup.o : \
    $(SRC)/startup.c \
    $(SRC)/error.h \
    $(SRC)/startup.h \
    $(SRC)/heap.h \
    $(SRC)/mem.h \
    $(SRC)/expand.h \
    $(SRC)/u_types.h \
    $(SRC)/types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    $(SRC)/read.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/startup.c 
	@mv startup.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/trace.o : \
    $(SRC)/trace.c \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/trace.c
	@mv trace.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/heap.o : \
    $(SRC)/heap.c \
    $(SRC)/types.h \
    $(SRC)/error.h \
    $(SRC)/heap.h \
    $(SRC)/trace.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/heap.c 
	@mv heap.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/expand.o : \
    $(SRC)/expand.c \
    $(SRC)/error.h \
    $(SRC)/heap.h \
    $(SRC)/mem.h \
    $(SRC)/expand.h \
    $(SRC)/u_types.h \
    $(SRC)/types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/expand.c 
	@mv expand.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/mem.o : \
    $(SRC)/mem.c \
    $(SRC)/mem.h \
    $(SRC)/error.h \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/mem.c
	@mv mem.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/read.o : \
    $(SRC)/read.c \
    $(SRC)/read.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/read.c
	@mv read.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/set.o : \
    $(SRC)/set.c \
    $(SRC)/set.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/set.c
	@mv set.o $(OBJ)
#------------------------------------------------------------
