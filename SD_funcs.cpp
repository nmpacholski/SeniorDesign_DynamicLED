#include "SD_funcs.h"
using namespace std;

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
