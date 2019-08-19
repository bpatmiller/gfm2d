import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from matplotlib.colors import Normalize
import matplotlib.cm as cm
from scipy.interpolate import griddata
import json

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


with open('config.json') as config_file:
    data = json.load(config_file)

xmax = data['horizontal_cells'] * data['cell_size']
ymax = data['vertical_cells'] * data['cell_size']

# DRAW PHI

phi_datablocks = read_blocks(phi_location, 0, 10)
vel_datablocks = read_blocks(velocity_location, 0, 10)
print("drawing phi", len(phi_datablocks), "time samples")
fig, axs = plt.subplots(2, int(len(phi_datablocks) / 2), figsize=(20, 20))

if len(phi_datablocks) == 1:
    axs = [axs]
else:
    axs = axs.reshape(-1)

for i, ax in enumerate(axs):
    x, y, z, fluid_id, pressure = np.loadtxt(phi_datablocks[i], unpack=True)
    # define grid.
    xi = np.linspace(0.0, xmax, 100)
    yi = np.linspace(0.0, ymax, 100)
    xi, yi = np.meshgrid(xi, yi)
    # grid the data.
    zi = griddata((x, y), fluid_id, (xi, yi), method='linear')
    boundaryi = griddata((x, y), -z, (xi, yi), method='nearest')

    # use for phi
    # cm.GnBu

    ax.imshow(
        zi,
        origin='lower',
        cmap=cm.Pastel2,
        interpolation='nearest',
        extent=[
            np.min(x),
            np.max(x),
            np.min(y),
            np.max(y)],
        vmin=0,
        vmax=2)
    contour = ax.contour(
        xi,
        yi,
        boundaryi,
        levels=8,
        linewidths=1,
        cmap="RdBu_r")

    x, y, u, v = np.loadtxt(vel_datablocks[i], unpack=True)

    ax.set_xlim(0, xmax)
    ax.set_ylim(0, ymax)
    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')

    ax.quiver(
        x,
        y,
        u,
        v,
        np.sqrt(
            u**2 +
            v**2),
        angles='xy',
        scale_units='xy',
        scale=15,
        width=0.0015,
        headwidth=2)

    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')

fig.tight_layout()
plt.savefig('plot/images/phi.png', bbox_inches='tight')
