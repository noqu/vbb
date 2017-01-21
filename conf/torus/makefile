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

$(PROB).$(NR)t.cfb: \
    inst/$(PROB).$(NR)t.cfb \
    ;

$(PROB).$(NR)t.btl: \
    inst/$(PROB).$(NR)t.btl \
    ;

$(PROB).all:
	$(MAKE) $(PROB).1t.btl NR=1
	$(MAKE) $(PROB).2t.btl NR=2
	$(MAKE) $(PROB).4t.btl NR=4
	$(MAKE) $(PROB).9t.btl NR=9
	$(MAKE) $(PROB).16t.btl NR=16
	$(MAKE) $(PROB).25t.btl NR=25
	$(MAKE) $(PROB).36t.btl NR=36
	$(MAKE) $(PROB).49t.btl NR=49
	$(MAKE) $(PROB).64t.btl NR=64

clean:
	rm -f inst/*.cfs inst/*.dsc inst/*.cfb inst/*.btl 2>/dev/null

#------------------------------------------------------------
inst/$(PROB).$(NR)t.cfb: \
    $(LKU)/$(PROB).lku \
    $(LKU)/$(PROB).red.lku \
    inst/$(PROB).$(NR)t.cfs \
	;
	icconf -G\
	    inst/$(PROB).$(NR)t.cfs \
	    -o inst/$(PROB).$(NR)t.cfb;

#------------------------------------------------------------
inst/$(PROB).$(NR)t.btl: \
    $(LKU)/$(PROB).lku \
    $(LKU)/$(PROB).red.lku \
    inst/$(PROB).$(NR)t.cfb \
	;
	icollect \
	    inst/$(PROB).$(NR)t.cfb \
	    -o inst/$(PROB).$(NR)t.btl
#------------------------------------------------------------
