
#ifndef SCAN_H
#define SCAN_H

extern const char *filename;
extern String scannedTag;
extern String rowLast;
extern String string1scan;
extern String row1;;
extern String row2;
extern bool rowScanned;
extern 

void scanTag();
void searchTag(const char *filename, std::string &book);
bool saveTag(const char *filename, String &row, String book);
void printFile(const char *filename);
bool deleteTag(const char *filename, String &book);


#endif