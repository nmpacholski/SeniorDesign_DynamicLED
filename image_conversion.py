'''
Author: Pun Arunsiritrakoon
Team Name: DynamicLED
Team Number: 52
Semester: Fall 2022
'''


from PIL import Image
import numpy as np
import math

def scale32(imagearray, width, heigth):
    '''
    Scale the imagearray from width by height to target width by target height
    '''
    twidth = 32 #Target width and height of the final image
    theight = 32
    stepw = int(width / twidth) #Step width and height for forloop
    steph = int(height / theight)
    newimg = []
    count1 = 0
    for i in range(0, height, steph):
        count2 = 0
        for q in range(0, width, stepw):
            [r, g, b] = imagearray[i, q] #Extract RGB value from the pixel
            newimg.append([r, g, b]) #Append the RGB value to the final image
            count2 = count2 + 1
            if count2 >= twidth: #Counter to prevent the image to have width that exceed target width
                break
        count1 = count1 + 1
        if count1 >= theight: #Counter to prevent the image to have height that exceed target height
                break
    newimg = np.array(newimg)
    newimg = np.array_split(newimg, 32) #Reshape the array to be the target final dimension
    newimg = np.array(newimg)
    im = Image.fromarray(newimg) #Save array into a new image file
    im.save("result.bmp")
    print('Success')
    return()

#Code from: https://www.daniweb.com/programming/software-development/threads/253957/converting-an-image-file-png-to-a-bitmap-file
def pngtobmp(imagearray): 
    '''
    Convert .png and .ico to .bmp
    '''
    r,g,b,a = imagearray.split() 
    imgbmp = Image.merge("RGB",(r,g,b))
    return(imgbmp)

def cropimg(imagearray, width, height):
    '''
    Crop the imagearray so that they have the same width and height
    '''
    leftindent = 0 #Initialize the indent value
    rightindent = 0
    topindent = 0
    bottomindent = 0
    difflength = width - height
    half = abs(difflength / 2)
    cropimage = []
    count1 = 0
    if width < height: #Setting the target dimension as the shorter length between width and height
        targetdimension = width
    else:
        targetdimension = height
    if difflength > 0: #Determine the amount of pixel needed to be indented base of the difference of width and height
        leftindent = math.ceil(half)
        rightindent = math.floor(half)
    elif difflength < 0:
        topindent = math.ceil(half)
        bottomtindent = math.floor(half)
    for i in range(0 + topindent, height - bottomindent, 1):
        count2 = 0
        for q in range(0 + leftindent, width - rightindent, 1):
            [r, g, b] = imagearray[i, q] #Extract the RGB value from the pixel
            cropimage.append([r, g, b]) #Append the RGB value to the final image
            count2 = count2 + 1
            if count2 >= targetdimension: #Counter to prevent the image to have width that exceed target dimension
                break
        count1 = count1 + 1
        if count1 >= targetdimension: #Counter to prevent the image to have height that exceed target dimension
            break
    cropimage = np.array(cropimage)
    cropimage = np.array_split(cropimage, targetdimension) #Reshape the array to be the target final dimension
    cropimage = np.array(cropimage)
    #x = Image.fromarray(cropimage)
    #x.save("cropresult.bmp")
    print('Crop Success')
    return(cropimage) #Return the cropped image to main
        

n = 0
while n == 0:
    filename = input('Enter filename: ') #Ask user for the name of the image file
    if (filename[-4:] == '.png') or (filename[-4:] == '.ico') or (filename[-4:] == '.jpg') or (filename[-4:] == '.bmp'):
        n = 1
    else:
        print('Wrong image format. Program only support .png, .ico, .jpg, .bmp')
inputfile = Image.open(filename)
if (filename[-4:] == '.png') or (filename[-4:] == '.ico'): 
    inputfile = pngtobmp(inputfile) #Convert .png and .ico to .bmp file
img = np.array(inputfile)
[height,width,d] = img.shape
if width != height:
    print("Image doesn't have the same width and height, would you like to crop the image to prevent image stretching")
    crop = input('1 for Yes, 0 for No: ') #Ask user if they want to crop the image
if crop == '1':
    img = cropimg(img, width, height) #Crop the image
    [height,width,d] = img.shape
scale32(img, width, height) #Scale the image to 32x32 resolution and save it to the new file



