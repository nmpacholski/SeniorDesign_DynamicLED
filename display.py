from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

filename = 'result.bmp'
inputfile = Image.open(filename)
img = np.array(inputfile)
[width,height,d] = img.shape
ax = plt.axes()
ax.set_facecolor('xkcd:black')
for y in range(width):
    for x in range(height):
        r = img[y,x,0]/255.0
        g = img[y,x,1]/255.0
        b = img[y,x,2]/255.0
        plt.scatter(x, y, c = [r, g, b])
plt.gca().invert_yaxis()
plt.show()
