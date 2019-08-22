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
h_cells = data['horizontal_cells']
v_cells = data['vertical_cells']
number_of_fluids = len(data['fluids'])

# DRAW PHI

phi_datablocks = read_blocks(phi_location, 0, 10)
vel_datablocks = read_blocks(velocity_location, 0, 10)

if len(phi_datablocks) % 2 == 0:
    number_of_rows = 2
    number_of_cols = len(phi_datablocks) // 2
else:
    number_of_rows = len(phi_datablocks)
    number_of_cols = 1

print("drawing phi", len(phi_datablocks), "time samples")
fig, axs = plt.subplots(number_of_rows, number_of_cols, figsize=(20, 20))

if len(phi_datablocks) == 1:
    axs = [axs]
else:
    axs = axs.reshape(-1)

for i, ax in enumerate(axs):
    x, y, z, fluid_id, pressure = np.loadtxt(phi_datablocks[i], unpack=True)
    # define grid.
    xi = np.linspace(0.0, xmax, h_cells)
    yi = np.linspace(0.0, ymax, v_cells)
    xi, yi = np.meshgrid(xi, yi)
    # grid the data.
    phii = griddata((x, y), z, (xi, yi), method='nearest')
    zi = griddata((x, y), fluid_id, (xi, yi), method='nearest')
    boundaryi = griddata((x, y), z, (xi, yi), method='nearest')

    # use for phi
    # cm.GnBu

    ax.imshow(
        zi,
        origin='lower',
        cmap=cm.Pastel2,
        # cmap=cm.Set2,
        interpolation='bilinear',
        extent=[
            np.min(x),
            np.max(x),
            np.min(y),
            np.max(y)],
        vmin=0,
        vmax=number_of_fluids
    )
    # contour = ax.contour(
    #     xi,
    #     yi,
    #     boundaryi,
    #     levels=8,
    #     linewidths=1,
    #     cmap=cm.RdBu)

    x, y, u, v = np.loadtxt(vel_datablocks[i], unpack=True)

    xii= np.linspace(0.0, xmax, h_cells // 3)
    yii = np.linspace(0.0, ymax, v_cells // 3)
    xii, yii = np.meshgrid(xii, yii)
    ui = griddata((x,y), u, (xii, yii), method='linear')
    vi = griddata((x,y), v, (xii, yii), method='linear')

    ax.set_xlim(0, xmax)
    ax.set_ylim(0, ymax)
    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')
    ax.quiver(
        xii,
        yii,
        ui,
        vi,
        np.sqrt(
            ui**2 +
            vi**2),
        angles='xy',
        scale_units='xy',
        scale=20,
        width=0.0015,
        headwidth=2,
        # headlength=1,
        pivot='mid')
    

    ax.set_title("t=" + str(i))
    ax.set_aspect("equal")
    ax.axis('off')

fig.tight_layout()
plt.savefig('plot/images/phi.png', bbox_inches='tight')
