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

$(PROB).seq.cfb: \
    inst/$(PROB).seq.cfb \
    ;

$(PROB).seq.btl: \
    inst/$(PROB).seq.btl \
    ;

clean:
	rm -f inst/*.cfs inst/*.dsc inst/*.cfb inst/*.btl 2>/dev/null

#------------------------------------------------------------
inst/$(PROB).seq.cfb: \
    $(LKU)/$(PROB).seq.lku \
    inst/$(PROB).seq.cfs \
	;
	icconf -G\
	    inst/$(PROB).seq.cfs \
	    -o inst/$(PROB).seq.cfb;

#------------------------------------------------------------
inst/$(PROB).seq.btl: \
    $(LKU)/$(PROB).seq.lku \
    inst/$(PROB).seq.cfb \
	;
	icollect \
	    inst/$(PROB).seq.cfb \
	    -o inst/$(PROB).seq.btl
#------------------------------------------------------------
