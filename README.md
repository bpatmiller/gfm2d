[![Build Status](https://travis-ci.com/bpatmiller/gfm2d.svg?branch=master)](https://travis-ci.com/bpatmiller/gfm2d)
[![Coverage Status](https://coveralls.io/repos/github/bpatmiller/gfm2d/badge.svg?branch=master)](https://coveralls.io/github/bpatmiller/gfm2d?branch=master)

### config.json

to add fluids, we use the "fluid" entry type in the json. A fluid entry consists of a name,
a density, and a phi definition. As of now, the only supported phi computation is of a circle
(exterior and interior).


### dependencies
https://github.com/nlohmann/json
