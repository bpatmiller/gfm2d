[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![codecov](https://codecov.io/gh/bpatmiller/gfm2d/branch/master/graph/badge.svg)](https://codecov.io/gh/bpatmiller/gfm2d)
[![BCH compliance](https://bettercodehub.com/edge/badge/bpatmiller/gfm2d?branch=master)](https://bettercodehub.com/)

### BIG TODOS:
* [ ] do some simple graphics
* [ ] do level set work
* [ ] do some tests

...

* [x] get coverage working
* [x] set up auto code review
* [x] get travis build working
* [x] get catch2 working
* [x] set up array2
* [x] make skeleton data structures
* [x] level set initialization
### about the project


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

### CI/CD stuff
- Travis CI
- CodeCov
- bettercodehub
- doxygen

### dependencies
https://github.com/nlohmann/json
