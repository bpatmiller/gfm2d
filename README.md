[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![codecov](https://codecov.io/gh/bpatmiller/gfm2d/branch/master/graph/badge.svg)](https://codecov.io/gh/bpatmiller/gfm2d)


### BIG TODOS:
- [x] get travis build working
- [x] get catch2 working
- [] set up array2
- [] do some simple graphics
- [] make skeleton data structures
- [] do level set work
- [] do some tests

...

- [] get coverage working
- [] set up auto code review

### about the project



### config.json

to add fluids, we use the "fluid" entry type in the json. A fluid entry consists of a name,
a density, and a phi definition. As of now, the only supported phi computation is of a circle
(exterior and interior).


### dependencies
https://github.com/nlohmann/json
