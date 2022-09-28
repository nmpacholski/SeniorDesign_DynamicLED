'''
Author: Pun Arunsiritrakoon
Team Name: DynamicLED
Team Number: 52
Semester: Fall 2022
'''

'''
Plot the image array into a scatter plot and each data point has the same color as the pixel RGB value
'''

from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

filename = 'result.bmp'
inputfile = Image.open(filename) #Open the bmp file and convert it into array
img = np.array(inputfile)
[width,height,d] = img.shape #Extract the width and height of the array
ax = plt.axes()
ax.set_facecolor('xkcd:black') #Set the background color of the graph to be black
for y in range(width):
    for x in range(height):
        r = img[y,x,0]/255.0 #Convert RGB value into the scatter plot color value which goes from 0 to 1
        g = img[y,x,1]/255.0
        b = img[y,x,2]/255.0
        plt.scatter(x, y, c = [r, g, b])
plt.gca().invert_yaxis() #Invert the y-axis
plt.show()
