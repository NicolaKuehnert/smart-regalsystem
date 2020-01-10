
#ifndef SCAN_H
#define SCAN_H

#include <Arduino.h>
#include <M5Stack.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <ArduinoJson.h>


#include "MFRC522_I2C.h"
#include "led.h"
#include "printDisplay.h"

#define RFID_ADDRESS 0x28
#define BUF_SIZE 18
#define PARITY_SIZE 2
#define PAGE_SIZE 4

extern const char *filename;
extern String scannedTag;

struct tag
{
    String uid;
    String name;
};

enum ascii:byte {
    BLANK = 0x20,
    EXCLAMATION,
    QUOTES,
	  HASH,
    PERCENT = 0x25,
    AND,
    STAR = 0x2A,
    COMMA = 0x2C,
    DOT = 0x2D,
    ZERO = 0x30,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    COLON,
    SEMICOLON,
    QUESTION = 0x3F,
    CHAR_A = 0x40,
    CHAR_B,
    CHAR_C,
    CHAR_D,
    CHAR_E,
    CHAR_F,
    CHAR_G,
    CHAR_H,
    CHAR_I,
    CHAR_J,
    CHAR_K,
    CHAR_L,
    CHAR_M,
    CHAR_N,
    CHAR_O,
    CHAR_P,
    CHAR_Q,
    CHAR_R,
    CHAR_S,
    CHAR_T,
    CHAR_U,
    CHAR_V,
    CHAR_W,
    CHAR_X,
    CHAR_Y,
    CHAR_Z,
    UNDERLINE = 0x5F,
    CHAR_a = 0x61,
    CHAR_b,
    CHAR_c,
    CHAR_d,
    CHAR_e,
    CHAR_f,
    CHAR_g,
    CHAR_h,
    CHAR_i,
    CHAR_j,
    CHAR_k,
    CHAR_l,
    CHAR_m,
    CHAR_n,
    CHAR_o,
    CHAR_p,
    CHAR_q,
    CHAR_r,
    CHAR_s,
    CHAR_t,
    CHAR_u,
    CHAR_v,
    CHAR_w,
    CHAR_x,
    CHAR_y,
    CHAR_z,
    UMLAUT_A = 0xE1,
    UMLAUT_O = 0xEF,
    UMLAUT_U = 0xF5    
};
  
bool checkBook(tag *book, StaticJsonDocument<520> doc);
void scanTag(int option);
void searchTag(const char *filename, std::string &book);
String saveTag(const char *filename, tag *row, tag *book);
void printFile(const char *filename);
bool deleteTag(const char *filename, tag *book);
bool writeTag (const char* text);
char* readTag();
void rfidInit();


#endif
