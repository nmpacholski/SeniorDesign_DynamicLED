Author: Pun Arunsiritrakoon
Team name: DynamicLED
Team number: 52
Subsystem: LED Display

image_conversion.py
Description: Program that allow the user to convery .png, .ico, .jpg, .bmp of any resolution that is bigger than 32x32 into bmp image file with 32x32 resolution.
In the case where the aspect ratio of the user selected image doesn't have 1:1 ratio, the user has the option to crop the image first before descaling to prevent the stretching
or shrinking of the image. The program will compile the scale down image into a new image file

Input: jpg, ico, png, or bmp image file

Output: 32x32 bmp image file

Function:
pngtobmp
Description: This function will takes out the alpha value (determine the transparency of a pixel) that is presented in .png and .ico format while keeping the original RGB values.
Input: Array with shape (x, y, 4)
Output: Array with shape (x, y, 3)

cropimg
Description: This function will crop the image so that it has an aspect ratio of 1:1 by taking the out the pixel on the left and right edges or top and bottom edge. The amount
of pixel that is taken out is determine by rounding half of the difference between width and height of the original image. If width is more than height then the left and right edge
will be taken out. If hight is more than width then the top and bottom edge will be taken out.
Input: Array with shape (x, y, 3), width of the original image, height of the original image
Output: Array with shape (x, x, 3) or Array with shape (y, y, 3)

scale32
Description: This function will scale down the image to 32x32 resolution. It will divide the input image into 32x32 grid and will takes the RGB values of the first pixel in each grid
and append those values into a new array. The array is converted into a bmp image format and is compile into a new image file.
Input: Array with shape (x, y, 3), width of the array, height of the array
Output: N/A



display.py
**This program will not be in the final product**

Description: This program will simulate how the 32x32 bmp will look on LED screen. It takes the bmp image file and plot each point on the graph with each point having the same color as 
the pixel in the original image.

Input: 32x32 bmp image file
Output: N/A



display.ino
Description: This program will act as a driver that will takes the input data from the microcontroller and display it on the LED matrix screen.

Function
displayimage
Description: This function will takes either an animation file which is a series of bmp image stack on top of each other or a single bmp image, and display the input data on the screen
Input: Data array pointer, width of the array, height of the array
Output: N/A

displaydimimage
Description: This function is similar to displayimage but it reduce the brightness of the screen
Input: Data array pointer, width of the array, height of the array, lambda
Output: N/A

displaytime
Description: This function will display the current time in the military time format. It will shows hour and minute and the number is 7 segment format
Input: Array with shape of (2)
Output: N/A