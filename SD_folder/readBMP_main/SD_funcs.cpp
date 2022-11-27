#include "SD_funcs.h"
using namespace std;

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void bmpDraw(fs::FS &fs, const char * filename, int disp_arr[32][32][3]) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(*sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  uint32_t file_size;
  uint32_t red_mask;
  uint32_t blu_mask;
  uint32_t grn_mask;
  uint32_t alpha_mask;
  Serial.println(filename);
  bmpFile = fs.open(filename);
  if (bmpFile == NULL) {
    Serial.print("File not found\n");
    return;
  }


///////////////////////////////////////////////////////////
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("BMP signature good\n");
    file_size = read32(bmpFile);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    (void)read32(bmpFile); //header size

    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      Serial.print("Planes good\n");
      bmpDepth = read16(bmpFile); // bits per pixel
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed(24bit/pix) or 3=compressed(32bit/pix)
        Serial.print("Compress good\n");
        (void)read32(bmpFile); //image size
        (void)read32(bmpFile); //x - pixels per meter
        (void)read32(bmpFile); //y - pixels per meter
        (void)read32(bmpFile); //colors used
        (void)read32(bmpFile); //colors important
        goodBmp = true; // Supported BMP format -- proceed!
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
        h = bmpHeight;
        w = bmpWidth;
        bx1 = by1 = 0;
      }
      else {
        Serial.print("Incorrect compress\n");
      }
    }
    else {
      Serial.print("Incorrect planes\n");
    }
////////////////////////////////////////////////////////////////
    red_mask = read32(bmpFile);
    grn_mask = read32(bmpFile);
    blu_mask = read32(bmpFile);
    alpha_mask = read32(bmpFile);
    (void)read32(bmpFile); //Default "sRGB" (0x73524742)
    (void)read32(bmpFile); //unused for sRBG

    for (row=0; row<h; row++) { // For each scanline...
      int rgb = 0;
      pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
      pos += bx1 * 3; // Factor in starting column (bx1)
      if(bmpFile.position() != pos) {
        bmpFile.seek(pos);
        buffidx = sizeof(*sdbuffer); // Force buffer reload
      }
      for (col=0; col<w; col++) { // For each pixel...
        // Time to read more pixel data?
        if (buffidx >= sizeof(*sdbuffer)) { // Indeed
          for(int rgb=0; rgb<3; rgb++) {
            bmpFile.read(sdbuffer, sizeof(*sdbuffer));
            disp_arr[row][col][2-rgb] = sdbuffer[0];
          }

          buffidx = 0; // Set index to beginning
        }
        buffidx++; buffidx++; buffidx++;
      }
    }
  }
  else {
    Serial.print("Incorrect BMP signature\n");
  }
  bmpFile.close();
  Serial.print("File is closed");
}
