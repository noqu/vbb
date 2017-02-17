#------------------------------------------------------------
# MAKEFILE fuer paralleles generisches Branch-and-Bound
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.5.93
#------------------------------------------------------------
# Hilfe mit 'make help'
#------------------------------------------------------------
LKU_D=lku
CONF_D=conf
BIN_D=bin
SEQ_D=seq
PAR_D=par
INST_D=inst
TOOLS_D=tools
SCC=cc
SLFLAGS=
SH_CMD=echo `expr X$(CONF) : "X\([tc]\).*"` 
L=$(SH_CMD:sh)
TOP_D:sh=pwd
#------------------------------------------------------------
help:
	@sed -n -e 's/^##//p' makefile
## 
## Generisches Branch-and-Bound auf einem Transputer-Netzwerk
## ----------------------------------------------------------
## make help: 
##	Gibt diese Hilfeseite aus
## 
## make install:
##	Erzeugt im aktuellen Verzeichnis aus vbb_42.taz
##	den kompletten Quellcode fuer die Installation.
##	Dabei wird ein Baum mit allen noetigen Verzeichnissen
##	angelegt.
##
## make all:
## 	Uebersetzt die gesamte Installation
##	Muss mindestens einmal zu Beginn aufgerufen werden
## 
## make clean:
## 	Entfernt alles bis auf Quellcode
## 	=> Zustand wie vor der Installation
## 
## make <inst>.<conf>:
## 	<inst> fuer die Instanz (tsp, scp oder msp)
## 	<conf> fuer die Konfiguration (torus oder circle)
## 	Erzeugt alle Groessen fuer die gewaehlte Instanz
## 	und Konfiguration neu
## 
## make <inst>.seq:
## 	Erzeugt eine Instanz mit sequentiellem Kern neu
## 
## #ifndef VBB_ORIGINAL
## make emulation:
##      Creates the libraries and test program for the transputer
##      emulation
##
## make <inst>.par:
## 	Erzeugt eine Instanz mit parallelem Kern neu
## #endif
## 
## make single PROB=<inst> NR=<groesse> CONF=<conf>:
## 	Erzeugt einzelne Groessen fuer eine Instanz
## 	und Konfiguration neu
## 

#------------------------------------------------------------
install:
	uncompress < vbb_42.taz | tar xvf - && rm vbb_42.taz
	@make help

#------------------------------------------------------------
all:
#ifndef VBB_ORIGINAL
	$(MAKE) tsp.seq scp.seq msp.seq tsp.par scp.par msp.par
	cd emu; $(MAKE) all
	cd tools; $(MAKE) all
_not_used:
#else
	cd $(PAR_D) ; $(MAKE) par_kernel.lib
	cd $(SEQ_D) ; $(MAKE) seq_kernel.a
	cd $(INST_D) ; $(MAKE) all
	$(MAKE) tsp.seq.intern PROB=tsp
	$(MAKE) scp.seq.intern PROB=scp
	$(MAKE) msp.seq.intern PROB=msp
	cd $(LKU_D) ; $(MAKE) tsp.lku PROB=tsp
	cd $(LKU_D) ; $(MAKE) tsp.seq.lku PROB=tsp
	cd $(LKU_D) ; $(MAKE) tsp.red.lku PROB=tsp
	cd $(LKU_D) ; $(MAKE) scp.lku PROB=scp
	cd $(LKU_D) ; $(MAKE) scp.seq.lku PROB=scp
	cd $(LKU_D) ; $(MAKE) scp.red.lku PROB=scp
	cd $(LKU_D) ; $(MAKE) msp.lku PROB=msp
	cd $(LKU_D) ; $(MAKE) msp.seq.lku PROB=msp
	cd $(LKU_D) ; $(MAKE) msp.red.lku PROB=msp
	cd $(CONF_D)/torus ; $(MAKE) tsp.cfs+dsc tsp.all \
			     CONF=torus PROB=tsp L=t
	cd $(CONF_D)/torus ; $(MAKE) scp.cfs+dsc scp.all \
			     CONF=torus PROB=scp L=t
	cd $(CONF_D)/torus ; $(MAKE) msp.cfs+dsc msp.all \
			     CONF=torus PROB=msp L=t
	cd $(CONF_D)/circle ; $(MAKE) tsp.cfs+dsc tsp.all \
			     CONF=circle PROB=tsp L=c
	cd $(CONF_D)/circle ; $(MAKE) scp.cfs+dsc scp.all \
			     CONF=circle PROB=scp L=c
	cd $(CONF_D)/circle ; $(MAKE) msp.cfs+dsc msp.all \
			     CONF=circle PROB=msp L=c
	cd $(CONF_D)/seq ; $(MAKE) tsp.cfs+dsc tsp.seq.btl \
			     CONF=seq PROB=tsp L=
	cd $(CONF_D)/seq ; $(MAKE) scp.cfs+dsc scp.seq.btl \
			     CONF=seq PROB=scp L=
	cd $(CONF_D)/seq ; $(MAKE) msp.cfs+dsc msp.seq.btl \
			     CONF=seq PROB=msp L=
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) tsp all torus $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) scp all torus $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) msp all torus $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) tsp all circle $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) scp all circle $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) msp all circle $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) tsp seq seq $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) scp seq seq $(TOP_D)
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) msp seq seq $(TOP_D)
	cd $(TOOLS_D) ; $(MAKE) all
#endif
#------------------------------------------------------------
single:
	$(MAKE) $(PROB).$(NR)$(L) CONF=$(CONF) L=$(L)
#------------------------------------------------------------
clean:
	rm -f $(BIN_D)/*.* 2>/dev/null
	cd $(PAR_D) ; $(MAKE) clean
	cd $(SEQ_D) ; $(MAKE) clean
	cd $(INST_D) ; $(MAKE) clean
	cd $(CONF_D)/circle ; $(MAKE) clean;
	cd $(CONF_D)/torus ; $(MAKE) clean;
	cd $(CONF_D)/seq ; $(MAKE) clean;
	cd $(LKU_D) ; $(MAKE) clean;
	cd $(TOOLS_D) ; $(MAKE) clean;
#ifndef VBB_ORIGINAL
	cd emu ; $(MAKE) clean
#endif
#------------------------------------------------------------
tsp.torus:
	$(MAKE) tsp.all.torus PROB=tsp CONF=torus L=t
tsp.circle:
	$(MAKE) tsp.all.circle PROB=tsp CONF=circle L=c
tsp.seq:
	$(MAKE) tsp.seq.intern PROB=tsp
#ifndef VBB_ORIGINAL
tsp.par:
	$(MAKE) tsp.par.intern PROB=tsp
#endif
scp.torus:
	$(MAKE) scp.all.torus PROB=scp CONF=torus L=t
scp.circle:
	$(MAKE) scp.all.circle PROB=scp CONF=circle L=c
scp.seq:
	$(MAKE) scp.seq.intern PROB=scp
#ifndef VBB_ORIGINAL
scp.par:
	$(MAKE) scp.par.intern PROB=scp
#endif
msp.torus:
	$(MAKE) msp.all.torus PROB=msp CONF=torus L=t
msp.circle:
	$(MAKE) msp.all.circle PROB=msp CONF=circle L=c
msp.seq:
	$(MAKE) msp.seq.intern PROB=msp
#ifndef VBB_ORIGINAL
msp.par:
	$(MAKE) msp.par.intern PROB=msp
#endif
#------------------------------------------------------------
$(PROB).all.$(CONF):
	cd $(LKU_D) ; $(MAKE) $(PROB) $(PROB).red 
	cd $(CONF_D)/$(CONF) ; $(MAKE) $(PROB).all
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) $(PROB) all $(CONF) $(TOP_D)
#------------------------------------------------------------
$(PROB).seq.intern:
	cd $(SEQ_D) ; $(MAKE) seq_kernel.a 
	cd $(INST_D) ; $(MAKE) obj/u_$(PROB).o 
	$(SCC) $(SLFLAGS) \
	$(SEQ_D)/seq_kernel.a \
	$(INST_D)/obj/u_$(PROB).o \
	-o $(BIN_D)/$(PROB).seq
#------------------------------------------------------------
#ifndef VBB_ORIGINAL
$(PROB).par.intern:
	cd $(PAR_D) ; $(MAKE) par_kernel.a 
	cd emu ; $(MAKE) emulation.a
	cd $(INST_D) ; $(MAKE) obj/u_$(PROB).o 
	$(SCC) $(SLFLAGS) \
	$(PAR_D)/par_kernel.a \
	emu/emulation.a \
	$(INST_D)/obj/u_$(PROB).o \
	-pthread \
	-o $(BIN_D)/$(PROB).par

emulation:
	cd $(PAR_D) ; $(MAKE) par_kernel.a
	cd emu ; $(MAKE) test_emu
#endif
#------------------------------------------------------------
$(PROB).$(NR)$(L): 
	cd $(LKU_D) ; $(MAKE) $(PROB) $(PROB).red 
	cd $(CONF_D)/$(CONF) ; $(MAKE) $(PROB).$(NR)$(L).btl
	$(TOOLS_D)/mk_exe $(CONF_D) $(BIN_D) $(PROB) $(NR) $(CONF) $(TOP_D)
#------------------------------------------------------------
taz:
	tar cvf - * | compress > vbb_42.taz
