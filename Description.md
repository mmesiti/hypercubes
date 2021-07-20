# Description of Hypercubes

It has been noticed that 
in order to port Sombrero 
and its parent code - HiRep -
to GPU architectures
it is necessary to change 
the memory layout that is used in the application.
This would as well apply
to any optimisation effort
aimed at enabling an efficient 
use multithreading.

Defining the memory layout 
is both a very important 
and very defined problem 
that can be treated separately 
from the other concerns 
in Lattice Field Theory codes.

There is also a lot of diversity 
both in architectures 
and in memory layout structures 
among lattice codes,
with almost each code 
defining their own distinctive memory layout.

The memory layout used in HiRep is, for example,
very different from the one used in Grid,
and while the Grid memory layout is aimed 
at streamlining the SIMD processing capabilities
the HiRep memory layout has been devised 
in a radically different way
to streamline MPI communications.

The memory layout library developed so far 
exposes an API that can be used 
to define a memory layout 
in a reasonable generality,
providing facilities 
to automatically create lookup tables
and filling communication buffers, 
while providing support for:
- Lattices of any number of dimensions;
- Domain decomposition and halo exchange;
- Even/Odd preconditioning (i.e., checkerboarding)
  in any subset of dimensions;
- Hierarchical cache blocking 
  with virtually any number of levels
  (cache oblivious memory layouts
  using space filling curves
  could be implemented as well
  on top of the existing features);
- Generalised Array of Structures (AoS)
  or Structure of Arrays (SoA) and 
  in-between approaches, 
  including Grid's virtual nodes layout;
- Non-equal domain decomposition 
  (this capability can be expanded 
  to allow efficient use of etherogeneous machines).

For further improvements
surely further optimisations 
need to be applied to the code 
that use, as a client, the memory layout library.





