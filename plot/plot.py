import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from matplotlib.mlab import griddata
from matplotlib.colors import Normalize
import matplotlib.cm as cm

phi_location = "plot/data/phi.txt"
velocity_location = "plot/data/vel.txt"


def read_blocks(input_file, i, j):
    """ Split a data file by newlines/comments. this is used to split our datafile by time
    indexing begins at 0."""
    empty_lines = 0
    blocks = []
    for line in open(input_file):
        # Check for empty/commented lines
        if not line or line.startswith('#'):
            # If 1st one: new block
            if empty_lines == 0:
                blocks.append([])
            empty_lines += 1
        # Non empty line: add line in current(last) block
        else:
            empty_lines = 0
            blocks[-1].append(line)
    return blocks[i:j + 1]


# DRAW PHI

datablocks = read_blocks(phi_location, 0, 10)
print("drawing phi", len(datablocks), "time samples")
fig, axs = plt.subplots(2, int(len(datablocks) / 2), figsize=(20, 20))

if len(datablocks) == 1:
    axs = [axs]

for i, ax in enumerate(axs.reshape(-1)):
    x, y, z, fluid_id, pressure = np.loadtxt(datablocks[i], unpack=True)
    # define grid.
    xi = np.linspace(0.0, 5.0, 100)
    yi = np.linspace(0.0, 5.0, 100)
    # grid the data.
    # zi = griddata(x, y, np.log(-z), xi, yi, interp='linear')
    zi = griddata(x, y, pressure, xi, yi, interp='linear')
    boundaryi = griddata(x, y, fluid_id, xi, yi, interp='linear')
    ax.imshow(zi, origin='lower', cmap=cm.GnBu, interpolation='nearest',
              extent=[np.min(x), np.max(x), np.min(y), np.max(y)])
    contour = ax.contour(
        xi,
        yi,
        boundaryi,
        levels=1,
        linewidths=2,
        cmap="RdBu_r")

    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')

fig.tight_layout()
plt.savefig('plot/images/phi.png', bbox_inches='tight')

# DRAW VELOCITY

datablocks = read_blocks(velocity_location, 0, 10)
print("drawing velocity,", len(datablocks), "time samples")
fig, axs = plt.subplots(2, int(len(datablocks) / 2), figsize=(20, 20))
if len(datablocks) == 1:
    axs = [axs]

for i, ax in enumerate(axs.reshape(-1)):
    x, y, u, v = np.loadtxt(datablocks[i], unpack=True)

    ax.set_xlim(0, 5)
    ax.set_ylim(0, 5)
    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')

    ax.quiver(x, y, u, v, np.sqrt(u**2 + v**2),
              angles='xy', scale_units='xy', scale=15)

fig.tight_layout()
plt.savefig('plot/images/vel.png', bbox_inches='tight')
