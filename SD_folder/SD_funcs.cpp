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

#define BUFFPIXEL 32

void bmpDraw(fs::FS &fs, const char * filename, uint8_t sdbuffer[3*BUFFPIXEL]) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  //uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
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

  if ((bmpFile = fs.open(filename)) == NULL) {
    //Serial.print(F("File not found"));
    //digitalWrite(21, HIGH);
    return;
  }


///////////////////////////////////////////////////////////
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    file_size = read32(bmpFile);
    //digitalWrite(23, HIGH);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    (void)read32(bmpFile); //header size

    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
//    (void)read16(bmpFile);
//      digitalWrite(21, HIGH);
      bmpDepth = read16(bmpFile); // bits per pixel
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed(24bit/pix) or 3=compressed(32bit/pix)
//        digitalWrite(23, HIGH);
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
    }
////////////////////////////////////////////////////////////////
    red_mask = read32(bmpFile);
    grn_mask = read32(bmpFile);
    blu_mask = read32(bmpFile);
    alpha_mask = read32(bmpFile);
    (void)read32(bmpFile); //Default "sRGB" (0x73524742)
    (void)read32(bmpFile); //unused for sRBG

//    digitalWrite(21, HIGH);
    for (row=0; row<h; row++) { // For each scanline...
//      digitalWrite(23, HIGH);
      pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
      pos += bx1 * 3; // Factor in starting column (bx1)
      if(bmpFile.position() != pos) {
//        digitalWrite(21, HIGH);
        bmpFile.seek(pos);
        buffidx = sizeof(*sdbuffer); // Force buffer reload
      }
      for (col=0; col<w; col++) { // For each pixel...
        //digitalWrite(21, HIGH);
        // Time to read more pixel data?
        if (buffidx >= sizeof(*sdbuffer)) { // Indeed
          //digitalWrite(23, HIGH);
          bmpFile.read(sdbuffer, sizeof(*sdbuffer));
          //red_mask = read32(bmpFile);
          //grn_mask = read32(bmpFile);
          //blu_mask = read32(bmpFile);
          //alpha_mask = read32(bmpFile);

          buffidx = 0; // Set index to beginning
        }
      }
      //digitalWrite(23, HIGH);
    }
  }
  bmpFile.close();
}

void createDir(fs::FS &fs, const char * path){
  if(fs.mkdir(path)){
    // created dir
  } else {
    // failed to create dir
  }
}

void removeDir(fs::FS &fs, const char * path){
  if(fs.rmdir(path)){
    // removed dir
  } else {
    // failed to remove dir
  }
}

void readFile(fs::FS &fs, const char * path){
  string maybe;
  string read_file = "1";

  File file = fs.open(path);
  if(!file){
    // failed to open file
    return;
  }

  maybe = file.read();

  // read full file
  //int p = 0;
  //while(file.available()){
  //  disp_arr[p] = file.read();
  //  p++;
  //}
  file.close();
  if (maybe == read_file) {
    digitalWrite(23, LOW);
    //digitalWrite(23, LOW);
  }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    // failed to open file
    return;
  }
  if(file.print(message)){
    // wrote file
  } else {
    // failed to write file
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    // failed to open file
    return;
  }
  if(file.print(message)){
    // appended file
  } else {
    // failed to append file
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  if (fs.rename(path1, path2)) {
    // renamed file
  } else {
    // failed to rename file
  }
}

void deleteFile(fs::FS &fs, const char * path){
  if(fs.remove(path)){
    // deleted file
  } else {
    // failed to delete file
  }
}

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    //Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    //Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if(!file){
    //Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  //Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

void loop_SD(){
}
