
## Build

(Unix only, tested on Linux)

`make all`

## Sequential execution

Example: Traveling salesman example problem Nr. 7

```
$ bin/tsp.seq mat=test/tsp/TSPs/tsp30.7

TIME: 0.285
   0   7   8  29   3   2  23  18  10  19  20  17  28  14   6  27  22  24   1  21  11   5  25   4  12  13   9  15  16  26
F = 140
```

Result value (length) and list of nodes are printed by default.

## Parallel execution

The original implementation was for multiple processes on a network of transputers, the reconstruction
uses multiple threads on a "network" of processes connected by named pipes. 

Example:  Traveling salesman example problem Nr. 7, using 16 processors, with parameters (from the file `para.16`) as described for the speedup tests. 

```
$ bin/tsp.16t @test/tsp/speedup/para.16 mat=test/tsp/TSPs/tsp30.7

SU  30258   505979  443336   32341    0  44  16  1712      0   1859  43458
TIME: 0.571
```

Note that no solution is printed because the parameter file contains the parameter `NO_SOLUTION`. Instead,
some execution statistics is emitted (but without the corresponding header) - twiddle with the parameters
to adjust that.
