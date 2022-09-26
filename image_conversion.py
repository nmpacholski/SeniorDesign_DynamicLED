from PIL import Image
import numpy as np

def scale32(imagearray, width, heigth):
    twidth = 32
    theight = 32
    stepw = int(width / twidth)
    steph = int(height / theight)
    newimg = []
    count1 = 0
    for i in range(0, width, stepw):
        count2 = 0
        for q in range(0, height, steph):
            [r, g, b] = imagearray[i, q]
            newimg.append([r, g, b])
            count2 = count2 + 1
            if count2 >= twidth:
                break
        count1 = count1 + 1
        if count1 >= twidth:
                break
    newimg = np.array(newimg)
    newimg = np.array_split(newimg, 32)
    newimg = np.array(newimg)
    #print(newimg)
    #print(newimg.shape)
    im = Image.fromarray(newimg)
    im.save("result.bmp")
    print('Success')
    return()

def pngtobmp(imagearray):
    r,g,b,a = imagearray.split()
    imgbmp = Image.merge("RGB",(r,g,b))
    return(imgbmp)

n = 0
while n == 0:
    filename = input('Enter filename')
    if (filename[-4:] == '.png') or (filename[-4:] == '.ico') or (filename[-4:] == '.jpg') or (filename[-4:] == '.bmp'):
        n = 1
    else:
        print('Wrong image format. Program only support .png, .ico, .jpg, .bmp')
inputfile = Image.open(filename)
if (filename[-4:] == '.png') or (filename[-4:] == '.ico'):
    inputfile = pngtobmp(inputfile)
img = np.array(inputfile)
[width,height,d] = img.shape
if width != height:
    print("Image doesn't have the same width and height, would you like to crop the image to prevent image stretching")
    crop = input('1 for Yes, 0 for No')
scale32(img, width, height)



