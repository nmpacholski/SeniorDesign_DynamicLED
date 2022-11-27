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
and append those values into a new array.
Input: Array with shape (x, y, 3), width of the array, height of the array
Output: Array with shape (32, 32, 3)



display.h
Function
displayimage
Description: This function will convert the data from the memory pointer into an integer array of 32x32x3 and map each pixel of the data to the LED matrix
Input: memory pointer(point to array with shape of 32,32,3), height of image, width of image
Output: N/A

displaydimimage
Description: This function is similar to the displayimage but the brightness level is turned down base on the lambda value
Input: memory pointer(point to array with shape of 32,32,3), height of image, width of image, lambda
Output: N/A

displaytime
Description: This function will read the time data input in the format of [hh:mm] and display the number according to the time data input
Input: Array with shape of 2
Output: N/A

displayequalizer
Description: This function will take the FFT array and display audio spectrum visualizer (similar to figure 1.1)
Input: Array with shape of 16 (This array is the new FFT), memory pointer (point to the current FFT [array with shape of 16])
output: Array with shape of 16
