-- ICC hardware configurer version 1.1  (920520) mf,hs,mp,mr
-- Copyright (C) 1991, 1992 Parsytec Chemnitz GmbH
PROC 0 0	type: 7 	memory: 4096 k	( transp )
PROC 1 1	type: 7 	memory: 4096 k	( transp )
PROC 2 2	type: 7 	memory: 4096 k	( transp )
PROC 3 3	type: 7 	memory: 4096 k	( transp )
LINK 0 1,	1 2
LINK 1 1,	2 2
LINK 2 1,	3 2
LINK 3 1,	0 2
LINK 0 3,	2 3
LINK 1 3,	3 3
