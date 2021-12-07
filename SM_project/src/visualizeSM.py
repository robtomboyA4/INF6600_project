from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt

from PIL import Image
import numpy as np

xmin, xmax = 0 , 5000
ymin, ymax = 0 , 5000

Zx = []
Zy = []
Zz = []
SDx = []
SDy = []
SDz = []

im = Image.open("Transmission_0.bmp")
p = np.array(im)

for x in range(xmin, xmax):
    for y in range(ymin, ymax):
        pixel = p[4999-y][x]

        if pixel[0] != 0:
            val = (pixel[0] << 16) | (pixel[1] << 8) | pixel[2]

            newZ = val & 0xFFF
            if(newZ != 0xFFF):
                Zx.append(x)
                Zy.append(y)
                Zz.append(newZ)

                SDx.append(x)
                SDy.append(y)
                SDz.append(-(val >> 12))


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(Zx, Zy, Zz)
ax.scatter(SDx, SDy, SDz)

plt.show()
