[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![codecov](https://codecov.io/gh/bpatmiller/gfm2d/branch/master/graph/badge.svg)](https://codecov.io/gh/bpatmiller/gfm2d)
[![BCH compliance](https://bettercodehub.com/edge/badge/bpatmiller/gfm2d?branch=master)](https://bettercodehub.com/)

### BIG TODOS:
* [ ] get travis build working
* [x] get catch2 working
* [x] set up array2
* [ ] level set initialization
* [ ] do some simple graphics
* [x] make skeleton data structures
* [ ] do level set work
* [ ] do some tests

...

* [x] get coverage working
* [x] set up auto code review

### about the project



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
