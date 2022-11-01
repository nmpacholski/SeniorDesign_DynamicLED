#ifndef _SD_FUNCS_H_
#define _SD_FUNCS_H_

#include "FS.h"

#define BUFFPIXEL 32
void bmpDraw(fs::FS &fs, const char * filename, uint8_t sdbuffer[3*BUFFPIXEL]);

void createDir(fs::FS &fs, const char * path);
void removeDir(fs::FS &fs, const char * path);

void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);

void testFileIO(fs::FS &fs, const char * path);

void loop_SD();

#endif
