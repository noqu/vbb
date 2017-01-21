#------------------------------------------------------------
# MAKEFILE fuer lku-Files
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.5.93
#------------------------------------------------------------
MYCC=icc
INST=../inst
PAR_LIB=../par
SEQ_LIB=../seq
#------------------------------------------------------------
CFLAGS= -t800 
LFLAGS= -t800 
#------------------------------------------------------------
clean:
	rm -f *.lku *.tco 2>/dev/null

#------------------------------------------------------------
$(PROB) : \
    ;
	cd $(INST) ; $(MAKE) obj/u_$(PROB).tco 
	cd $(PAR_LIB) ; $(MAKE) par_kernel.lib
	$(MAKE) $(PROB).lku
#------------------------------------------------------------
$(PROB).red : \
    fake_io.tco \
    ;
	cd $(INST) ; $(MAKE) obj/u_$(PROB).tco 
	cd $(PAR_LIB) ; $(MAKE) par_kernel.lib
	$(MAKE) $(PROB).red.lku
#------------------------------------------------------------
$(PROB).seq : \
    ;
	cd $(INST) ; $(MAKE) obj/u_$(PROB).tco
	cd $(SEQ_LIB) ; $(MAKE) seq_kernel.lib
	$(MAKE) $(PROB).seq.lku
#------------------------------------------------------------
fake_io.tco : \
    fake_io.c \
    ;
	$(MYCC) $(CFLAGS) fake_io.c
#------------------------------------------------------------
$(PROB).lku : \
    $(INST)/obj/u_$(PROB).tco \
    $(PAR_LIB)/par_kernel.lib \
    ;
	ilink $(LFLAGS) -f startup.lnk\
	    $(INST)/obj/u_$(PROB).tco \
	    $(PAR_LIB)/par_kernel.lib \
	    -o $(PROB).lku
#------------------------------------------------------------
$(PROB).red.lku : \
    $(INST)/obj/u_$(PROB).tco \
    $(PAR_LIB)/par_kernel.lib \
    fake_io.tco \
    ;
	ilink $(LFLAGS) -f startrd.lnk\
	    $(INST)/obj/u_$(PROB).tco \
	    $(PAR_LIB)/par_kernel.lib \
	    fake_io.tco \
	    -o $(PROB).red.lku
#------------------------------------------------------------
$(PROB).seq.lku : \
    $(INST)/obj/u_$(PROB).tco \
    $(SEQ_LIB)/seq_kernel.lib \
    ;
	ilink $(LFLAGS) -f startup.lnk\
	    $(INST)/obj/u_$(PROB).tco \
	    $(SEQ_LIB)/seq_kernel.lib \
	    -o $(PROB).seq.lku
#------------------------------------------------------------
