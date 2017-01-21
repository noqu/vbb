#------------------------------------------------------------
# MAKEFILE fuer cfs, cfb und dsc - Files
#------------------------------------------------------------
# Norbert Kuck 
#------------------------------------------------------------
# 10.5.93
#------------------------------------------------------------
LKU=../../lku
#------------------------------------------------------------

$(PROB).cfs+dsc:
	../../tools/mk_prob $(PROB) inst $(LKU)

$(PROB).$(NR)c.cfb: \
    inst/$(PROB).$(NR)c.cfb \
    ;

$(PROB).$(NR)c.btl: \
    inst/$(PROB).$(NR)c.btl \
    ;

$(PROB).all:
	$(MAKE) $(PROB).1c.btl NR=1
	$(MAKE) $(PROB).2c.btl NR=2
	$(MAKE) $(PROB).4c.btl NR=4
	$(MAKE) $(PROB).8c.btl NR=8
	$(MAKE) $(PROB).16c.btl NR=16
	$(MAKE) $(PROB).32c.btl NR=32
	$(MAKE) $(PROB).64c.btl NR=64

clean:
	rm -f inst/*.cfs inst/*.dsc inst/*.cfb inst/*.btl 2>/dev/null

#------------------------------------------------------------
inst/$(PROB).$(NR)c.cfb: \
    $(LKU)/$(PROB).lku \
    $(LKU)/$(PROB).red.lku \
    inst/$(PROB).$(NR)c.cfs \
	;
	icconf -G\
	    inst/$(PROB).$(NR)c.cfs \
	    -o inst/$(PROB).$(NR)c.cfb;

#------------------------------------------------------------
inst/$(PROB).$(NR)c.btl: \
    $(LKU)/$(PROB).lku \
    $(LKU)/$(PROB).red.lku \
    inst/$(PROB).$(NR)c.cfb \
	;
	icollect \
	    inst/$(PROB).$(NR)c.cfb \
	    -o inst/$(PROB).$(NR)c.btl
#------------------------------------------------------------
