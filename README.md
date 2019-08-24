[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![codecov](https://codecov.io/gh/bpatmiller/gfm2d/branch/master/graph/badge.svg)](https://codecov.io/gh/bpatmiller/gfm2d)

### about
Please read [this paper describing the project](writeup.pdf). The majority of the information comes from the original Particle Level Set Method paper, Level Set Methods and Dynamic Implicit Surfaces by Fedkiw and Osher, Multiple Interacting Fluids by Losasso et al, Stable Fluids by Stam et al, and various university pdfs scattered across the internet.

The name of this program comes from the Ghost Fluid Method, which is not used in this program.

### BIG TODOS:
* [ ] fix cmake like a real human
* [ ] more examples (!) (like stored in file)
* [ ] do more test / fix catch2

...

* [x] get coverage working
* [x] set up auto code review
* [x] get travis build working
* [x] get catch2 working
* [x] set up array2
* [x] make skeleton data structures
* [x] level set initialization
* [x] fix weird cyclic dependency
* [x] do some simple graphics
* [x] do level set work
* [x] phi projection method
* [x] levelset reinitialization
* [x] density sampling
* [x] poisson coefficient matrix
* [x] poisson rhs
* [x] pressure projection!!!
* [x] make levelset initialization better
* [x] impliment reactions (or something else cool)


### data format for graphing
within the plot/ folder, there are seperate files for:
- pressure
- velocities
- phi/fluid id pairs
- (maybe) particles for each fluid
  
the format for pressure is:

x position, y position, pressure

the format for velocities are:

x position, y position, x velocity, y velocity

the format for phi/fluid id pairs is:

x position, y position, phi, fluid id

>Note: these phi values will always be negative

The locus of each fluid will be drawn with solid cells, where the hue will be chosen according to which fluid exists at the center of the cell, and the intensity will be chosen based on the value of phi.
Velocity will be drawn on top of the cell centers as an arrow pointing along its interpolated velocity.

### config.json

to add fluids, we use the "fluid" entry type in the json. A fluid entry consists of a name,
a density, and a phi definition. As of now, the only supported phi computation is of a circle
(exterior and interior).

### Dependencies
nlohmann/json
catch2
Eigen3
GLM

### me learning that O3 is very good
debug:

[ 0.852 seconds have passed ] exporting frame 0 at time 0.000000
[ 1.420 seconds have passed ] exporting frame 1 at time 0.250000
[ 9.732 seconds have passed ] exporting frame 2 at time 0.500000
[ 25.893 seconds have passed ] exporting frame 3 at time 0.750000
[ 48.799 seconds have passed ] exporting frame 4 at time 1.000000
=======================================================================

release:

[ 0.033 seconds have passed ] exporting frame 0 at time 0.000000
[ 0.085 seconds have passed ] exporting frame 1 at time 0.250000
[ 0.377 seconds have passed ] exporting frame 2 at time 0.500000
[ 0.936 seconds have passed ] exporting frame 3 at time 0.750000
[ 1.690 seconds have passed ] exporting frame 4 at time 1.000000
===============================================================================

# performance benefits of solving with an initial guess
with guess:

[ 0.00s elapsed ] exporting frame 0 at time 0.00
[ 0.02s elapsed ] exporting frame 1 at time 1.00
[ 19.56s elapsed ] exporting frame 2 at time 2.00
[ 44.51s elapsed ] exporting frame 3 at time 3.00
[ 78.75s elapsed ] exporting frame 4 at time 4.00

without guess:

TODO finish this
