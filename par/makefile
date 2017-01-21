#------------------------------------------------------------
# MAKEFILE fuer Kernel-Library
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.3.93
#------------------------------------------------------------
MYCC=icc
OBJ=obj
SRC=src
#------------------------------------------------------------
CFLAGS= -t800 
#------------------------------------------------------------
par_kernel.lib : \
    $(OBJ)/startup.tco \
    $(OBJ)/disp.tco \
    $(OBJ)/heap.tco \
    $(OBJ)/mem.tco \
    $(OBJ)/expand.tco \
    $(OBJ)/read.tco \
    $(OBJ)/queue.tco \
    $(OBJ)/inout.tco \
    $(OBJ)/set.tco \
    $(OBJ)/trace.tco \
    ;
	ilibr \
	    $(OBJ)/startup.tco \
	    $(OBJ)/disp.tco \
	    $(OBJ)/heap.tco \
	    $(OBJ)/mem.tco \
	    $(OBJ)/expand.tco \
	    $(OBJ)/read.tco \
	    $(OBJ)/queue.tco \
	    $(OBJ)/inout.tco \
	    $(OBJ)/set.tco \
	    $(OBJ)/trace.tco \
	    -o par_kernel.lib
#------------------------------------------------------------
clean:
	rm -f obj/*.tco 2>/dev/null
	rm -f par_kernel.lib 2>/dev/null
#------------------------------------------------------------
$(OBJ)/startup.tco : \
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
	$(MYCC) $(CFLAGS) $(SRC)/startup.c 
	@mv startup.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/trace.tco : \
    $(SRC)/trace.c \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/queue.h \
    $(SRC)/trace.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/trace.c
	@mv trace.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/heap.tco : \
    $(SRC)/heap.c \
    $(SRC)/types.h \
    $(SRC)/queue.h \
    $(SRC)/error.h \
    $(SRC)/heap.h \
    $(SRC)/trace.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/heap.c 
	@mv heap.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/expand.tco : \
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
	$(MYCC) $(CFLAGS) $(SRC)/expand.c 
	@mv expand.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/disp.tco : \
    $(SRC)/disp.c \
    $(SRC)/disp.h \
    $(SRC)/error.h \
    $(SRC)/queue.h \
    $(SRC)/heap.h \
    $(SRC)/trace.h \
    $(SRC)/types.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/disp.c
	@mv disp.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/mem.tco : \
    $(SRC)/mem.c \
    $(SRC)/mem.h \
    $(SRC)/error.h \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/trace.h \
    $(SRC)/queue.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/mem.c
	@mv mem.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/read.tco : \
    $(SRC)/read.c \
    $(SRC)/read.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/read.c
	@mv read.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/queue.tco : \
    $(SRC)/queue.c \
    $(SRC)/queue.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/queue.c
	@mv queue.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/inout.tco : \
    $(SRC)/inout.c \
    $(SRC)/types.h \
    $(SRC)/u_types.h \
    $(SRC)/u_funcs.h \
    $(SRC)/inout.h \
    $(SRC)/queue.h \
    $(SRC)/trace.h \
    $(SRC)/error.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/inout.c
	@mv inout.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/set.tco : \
    $(SRC)/set.c \
    $(SRC)/set.h \
    ;
	$(MYCC) $(CFLAGS) $(SRC)/set.c
	@mv set.tco $(OBJ)
#------------------------------------------------------------
