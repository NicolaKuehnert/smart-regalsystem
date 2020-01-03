#ifndef SCAN_H
#define SCAN_H

const char *filename = "/datenbank.txt";
String scannedTag= "";
String rowLast = "";
String string1scan = "";
String row1 = " 04 6d 95 a2 ec 5a 81";
String row2 = " 04 67 95 a2 ec 5a 81";
bool rowScanned = false;


void scanTag();
void searchTag(const char *filename, std::string &book);
bool saveTag(const char *filename, String &row, String book);
void printFile(const char *filename);
#endif