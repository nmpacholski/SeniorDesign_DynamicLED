'''
Author: Pun Arunsiritrakoon
Team Name: DynamicLED
Team Number: 52
Semester: Fall 2022
'''


from PIL import Image
import numpy as np
import math
import sys

#reference = ['a','b','c','d','e','f','g','h','i','j']

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
    f = open("result.txt", "w")
    print(newimg, file = f)
    f.close()
    newimg = np.array(newimg)
    #print(newimg)
    #im = Image.fromarray(newimg) #Save array into a new image file
    #im.save("result.bmp")
    print('Success')
    
    return(newimg)

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
    y = cropimage[0,0,0]
    #print(y)
    #print(type(y))
    #x = Image.fromarray(cropimage)
    #x.save("cropresult.bmp")
    print('Crop Success')
    return(cropimage) #Return the cropped image to main

'''def number_2_word(n):
 
    # If all the digits are encountered return blank string
    if(n==0):
        return "a"
     
    else:
        # compute spelling for the last digit
        small_ans = reference[n%10]
 
        # keep computing for the previous digits and add the spelling for the last digit
        ans = number_2_word(int(n/10)) + small_ans
     
    # Return the final answer
    return ans'''

def printnumber(number):
    if (number < 10):
        convertnumber = "0"+"0"+"0"+str(number)
    elif ((number < 100) and (number >= 10)):
        convertnumber = "0"+"0"+str(number)
    elif ((number < 1000) and (number >= 100)):
        convertnumber = "0"+str(number)
    elif ((number < 10000) and (number >= 1000)):
        convertnumber =str(number)
    return(convertnumber)
        

np.set_printoptions(threshold=sys.maxsize)
n = 0
crop = 0
numframe = 1
gifcheck = 0
test = []
framelimit = 9
while n == 0:
    filename = input('Enter filename: ') #Ask user for the name of the image file
    if (filename[-4:] == '.png') or (filename[-4:] == '.ico') or (filename[-4:] == '.jpg') or (filename[-4:] == '.bmp') or (filename[-4:] == '.gif'):
        n = 1
    else:
        print('Wrong image format. Program only support .png, .ico, .jpg, .bmp, .gif')
inputfile = Image.open(filename)
'''if (filename[-4:] == '.png') or (filename[-4:] == '.ico'): 
    inputfile = pngtobmp(inputfile) #Convert .png and .ico to .bmp file'''
if (filename[-4:] == '.gif'):
    numframe = inputfile.n_frames
    gifcheck = 1
    print(numframe)
    '''if numframe > framelimit:
        print('The ESP32 can only show 8 frames of animation due to memory issue')
        startingframe = input('Please choose the starting frame: ')
        endingframe = input('Please choose the ending frame: ')'''
for i in range(0,numframe,1):
    if gifcheck == 1:
        if i >= numframe - 1:
            continue
        print('current seek =', i+1)
        inputfile.seek(i + 1)
    #print('continue check')
    img = np.array(inputfile)
    [height,width,d] = img.shape
    if d == 4:
        tempimg = Image.fromarray(img)
        fiximg = pngtobmp(tempimg)
        img = np.array(fiximg)
    if i == 0:
        if width != height:
            print("Image doesn't have the same width and height, would you like to crop the image to prevent image stretching")
            crop = input('1 for Yes, 0 for No: ') #Ask user if they want to crop the image
    if crop == '1':
        img = cropimg(img, width, height) #Crop the image
        [height,width,d] = img.shape
    newimg = scale32(img, width, height) #Scale the image to 32x32 resolution and save it to the new file
    if i == 0:
        resultname = input('Enter the result image name: ')
    if gifcheck == 1:
        '''answer = number_2_word(i)
        print(type(answer))
        print(answer)'''
        convertnumber = printnumber(i)
        print(str(convertnumber))
        savename = "g_"+resultname+"_"+str(convertnumber)+".bmp"
    else:
        savename = "i_"+resultname+".bmp"
    im = Image.fromarray(newimg) #Save array into a new image file
    im.save(savename)
    '''if i == 0:
        final = newimg
    else:
        final = np.concatenate((final, newimg), axis = 0)

print(final.shape)
f = open("result.txt", "w")
final = final.tolist()
print(final, file = f)
f.close()
final = np.array(final, dtype=np.uint8)
im = Image.fromarray(final) #Save array into a new image file
im.save("result.bmp")'''


