# TODO's
List of things that need to be done but were left to do later.
Of course, it does not include things I have not started working on (list would be endless).

## Paging
1. Find better way to keep track of available virtual pages
2. Find better way to keep track of unmapped physical pages
3. Currently there are 256 preallocated pages (for the kernel); What to do when it grows?

## SDT's
1. Organise header files/structs better: 
Currently ACPISTDheader is defined in rsdt.h and it's conflicting with other SDT h files.
2. Take size of structs into account when mapping pages in rsdt.c.
In a similar note, find better way to do it instead of just mapping all SDT's even
if they fall into the same page.

## PS/2 (currently working on)
1. Initialise the controller
2. Write simple keyboard driver
