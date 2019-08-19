[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![codecov](https://codecov.io/gh/bpatmiller/gfm2d/branch/master/graph/badge.svg)](https://codecov.io/gh/bpatmiller/gfm2d)

### about
Please read [this paper describing the project](writeup.pdf). The majority of the information comes from the original Particle Level Set Method paper, Level Set Methods and Dynamic Implicit Surfaces by Fedkiw and Osher, Multiple Interacting Fluids by Losasso et al, Stable Fluids by Stam et al, and various university pdfs scattered across the internet.

### BIG TODOS:
* [ ] fix cmake like a real human
* [ ] more examples (!) (like stored in file)
* [ ] impliment reactions (or something else cool)
* [ ] do more test / fix catch2

...

* [x] pressure projection!!!
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
* [x] make levelset initialization better

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
https://github.com/nlohmann/json
catch2
Eigen3
GLM
