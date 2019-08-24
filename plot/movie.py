import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from matplotlib.colors import Normalize
import matplotlib.cm as cm
from scipy.interpolate import griddata
import json
from tqdm import tqdm

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

phi_datablocks = read_blocks(phi_location, 0, 1000)
vel_datablocks = read_blocks(velocity_location, 0, 1000)

for frame in tqdm(range(0, len(phi_datablocks))):
    phi_datablock = phi_datablocks[frame]
    vel_datablock = vel_datablocks[frame]


    #--------------------------------
    #   fluid types
    #--------------------------------
    x, y, z, fluid_id, pressure = np.loadtxt(phi_datablock, unpack=True)
    xi = np.linspace(0.0, xmax, 2 * h_cells)
    yi = np.linspace(0.0, ymax, 2 * v_cells)
    xi, yi = np.meshgrid(xi, yi)
    zi = griddata((x, y), fluid_id, (xi, yi), method='linear')

    fig = matplotlib.pyplot.gcf()
    fig.set_size_inches(10,10)

    plt.imshow(
        zi,
        origin='lower',
        cmap=cm.Pastel2,
        interpolation='bilinear',
        extent=[
            np.min(x),
            np.max(x),
            np.min(y),
            np.max(y)],
        vmin=0,
        vmax=number_of_fluids
    )

    #--------------------------------
    #   phi value contour lines
    #--------------------------------
    # boundaryi = griddata((x, y), z, (xi, yi), method='nearest')
    # contour = contour(
    #     xi,
    #     yi,
    #     boundaryi,
    #     levels=8,
    #     linewidths=1,
    #     cmap=cm.RdBu)

    #--------------------------------
    #   velocity field quiver
    #--------------------------------
    x, y, u, v = np.loadtxt(vel_datablock, unpack=True)
    xii = np.linspace(0.0, xmax, h_cells // 4)
    yii = np.linspace(0.0, ymax, v_cells // 4)
    xii, yii = np.meshgrid(xii, yii)
    ui = griddata((x, y), u, (xii, yii), method='linear')
    vi = griddata((x, y), v, (xii, yii), method='linear')

    # plt.set_xlim(0, xmax)
    # plt.set_ylim(0, ymax)
    # plt.set_title("t=" + str(frame))
    # plt.set_aspect("equal")
    plt.axis('off')
    plt.quiver(
        xii,
        yii,
        ui,
        vi,
        np.sqrt(
            ui**2 +
            vi**2),
        angles='xy',
        scale_units='xy',
        scale=10,
        width=0.0015,
        headwidth=2,
        # headlength=1,
        pivot='mid')

    # plt.set_title("t=" + str(frame))
    # plt.set_aspect("equal")
    # plt.axis('off')

    plt.tight_layout()
    plt.savefig('plot/images/phi{0:05d}.png'.format(frame), bbox_inches='tight')
    plt.clf()
    # print('frame saved to plot/images/phi{0:05d}.png'.format(frame))