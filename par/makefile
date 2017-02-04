#------------------------------------------------------------
# MAKEFILE fuer Kernel-Library
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.3.93
#------------------------------------------------------------
#ifdef VBB_ORIGINAL
MYCC=icc
# (and originally: *.tco for *.o, *.lib for *.a, ilibr for ar rc ...)
#else
MYCC=cc
#endif
OBJ=obj
SRC=src
#------------------------------------------------------------
#ifdef VBB_ORIGINAL
CFLAGS= -O
#else
CFLAGS= -O -I../emu/src
#endif
#------------------------------------------------------------
par_kernel.a : \
    $(OBJ)/startup.o \
    $(OBJ)/disp.o \
    $(OBJ)/heap.o \
    $(OBJ)/mem.o \
    $(OBJ)/expand.o \
    $(OBJ)/read.o \
    $(OBJ)/queue.o \
    $(OBJ)/inout.o \
    $(OBJ)/set.o \
    $(OBJ)/trace.o \
    ;
	ar rc par_kernel.a \
	    $(OBJ)/startup.o \
	    $(OBJ)/disp.o \
	    $(OBJ)/heap.o \
	    $(OBJ)/mem.o \
	    $(OBJ)/expand.o \
	    $(OBJ)/read.o \
	    $(OBJ)/queue.o \
	    $(OBJ)/inout.o \
	    $(OBJ)/set.o \
	    $(OBJ)/trace.o \
	    ;
	ranlib par_kernel.a
#------------------------------------------------------------
clean:
	rm -f obj/*.o 2>/dev/null
	rm -f par_kernel.a 2>/dev/null
#------------------------------------------------------------
$(OBJ)/startup.o : \
    $(SRC)/startup.c \
    $(SRC)/error.h \
    $(SRC)/inout.h \
    $(SRC)/startup.h \
    $(SRC)/disp.h \
    $(SRC)/heap.h \
    $(SRC)/mem.h \
    $(SRC)/expand.h \
    $(SRC)/u_types.h \
    $(SRC)/types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/queue.h \
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
    $(SRC)/queue.h \
    $(SRC)/trace.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/trace.c
	@mv trace.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/heap.o : \
    $(SRC)/heap.c \
    $(SRC)/types.h \
    $(SRC)/queue.h \
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
    $(SRC)/disp.h \
    $(SRC)/heap.h \
    $(SRC)/mem.h \
    $(SRC)/expand.h \
    $(SRC)/u_types.h \
    $(SRC)/types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/queue.h \
    $(SRC)/trace.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/expand.c 
	@mv expand.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/disp.o : \
    $(SRC)/disp.c \
    $(SRC)/disp.h \
    $(SRC)/error.h \
    $(SRC)/queue.h \
    $(SRC)/heap.h \
    $(SRC)/trace.h \
    $(SRC)/types.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/disp.c
	@mv disp.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/mem.o : \
    $(SRC)/mem.c \
    $(SRC)/mem.h \
    $(SRC)/error.h \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    $(SRC)/queue.h \
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
$(OBJ)/queue.o : \
    $(SRC)/queue.c \
    $(SRC)/queue.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/queue.c
	@mv queue.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/inout.o : \
    $(SRC)/inout.c \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/inout.h \
    $(SRC)/queue.h \
    $(SRC)/trace.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/inout.c
	@mv inout.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/set.o : \
    $(SRC)/set.c \
    $(SRC)/set.h \
    ;
	$(MYCC) $(CFLAGS) -c $(SRC)/set.c
	@mv set.o $(OBJ)
#------------------------------------------------------------
