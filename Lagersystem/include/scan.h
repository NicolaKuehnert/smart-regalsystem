
#ifndef SCAN_H
#define SCAN_H

extern const char *filename;
extern String scannedTag;
extern String rowLast;
extern String string1scan;
extern String row1;;
extern String row2;
extern bool rowScanned;

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
  

void scanTag();
void searchTag(const char *filename, std::string &book);
bool saveTag(const char *filename, String &row, String book);
void printFile(const char *filename);
bool deleteTag(const char *filename, String &book);


#endif
