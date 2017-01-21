#------------------------------------------------------------
# MAKEFILE fuer Instanzfunktionen
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.5.93
#------------------------------------------------------------
TCC=icc
SCC=cc
OBJ=obj
#------------------------------------------------------------
TCFLAGS= -t800 
SCFLAGS= -DSUN_CC -O
#------------------------------------------------------------
all: \
    $(OBJ)/u_tsp.tco \
    $(OBJ)/u_scp.tco \
    $(OBJ)/u_msp.tco \
    $(OBJ)/u_tsp.o \
    $(OBJ)/u_scp.o \
    $(OBJ)/u_msp.o \
    ;
#------------------------------------------------------------
clean:
	rm -f $(OBJ)/*.tco $(OBJ)/*.o 2>/dev/null
#------------------------------------------------------------
$(OBJ)/u_tsp.tco : \
    u_tsp.c \
    u_tsp_types.h \
    u_funcs.h \
    read.h \
    error.h \
    ;
	$(TCC) $(TCFLAGS) u_tsp.c 
	@mv u_tsp.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/u_scp.tco : \
    u_scp.c \
    u_scp_types.h \
    u_funcs.h \
    read.h \
    set.h \
    error.h \
    ;
	$(TCC) $(TCFLAGS) u_scp.c 
	@mv u_scp.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/u_msp.tco : \
    u_msp.c \
    u_msp_types.h \
    u_funcs.h \
    read.h \
    error.h \
    ;
	$(TCC) $(TCFLAGS) u_msp.c 
	@mv u_msp.tco $(OBJ)
#------------------------------------------------------------
$(OBJ)/u_tsp.o : \
    u_tsp.c \
    u_tsp_types.h \
    u_funcs.h \
    read.h \
    error.h \
    ;
	$(SCC) $(SCFLAGS) -c u_tsp.c 
	@mv u_tsp.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/u_scp.o : \
    u_scp.c \
    u_scp_types.h \
    u_funcs.h \
    read.h \
    set.h \
    error.h \
    ;
	$(SCC) $(SCFLAGS) -c u_scp.c 
	@mv u_scp.o $(OBJ)
#------------------------------------------------------------
$(OBJ)/u_msp.o : \
    u_msp.c \
    u_msp_types.h \
    u_funcs.h \
    read.h \
    error.h \
    ;
	$(SCC) $(SCFLAGS) -c u_msp.c 
	@mv u_msp.o $(OBJ)
#------------------------------------------------------------
