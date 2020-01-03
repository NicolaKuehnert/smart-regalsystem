#ifndef PRINTDISPLAY_H
#define PRINTDISPLAY_H

#include <M5Stack.h>

void printScan();
void scanTag();
void searchTag(const char *filename, String &book);
bool saveTag(const char *filename, String &row, String book);
void printFile(const char *filename);
#endif