#include "scan.h"

const uint8_t pageAddr = 0x06;
MFRC522 mfrc522(0x28);

MFRC522::StatusCode rfidStatus;
tag *rowTag;
tag *bookTag;

void rfidInit()
{
    
    mfrc522.PCD_Init();
}

bool checkBook(tag *book, StaticJsonDocument<520> doc)
{
    for (int i = 0; i < doc["Reihe1"].size(); i++)
    {
        if (doc["Reihe1"].getElement(i) == book->name)
        {
            return false;
        }
    }

    for (int i = 0; i < doc["Reihe2"].size(); i++)
    {
        if (doc["Reihe2"].getElement(i) == book->name)
        {
            return false;
        }
    }
    return true;
}
// Loads the configuration from a file
String saveTag(const char *filename, tag *row, tag *book) {
  // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<512> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  file.close();

  if(checkBook(book, doc) == false)
  {
      return "gibt es schon";
  }

  file = SD.open(filename, FILE_WRITE);
  doc[row->name].add(book->name);

  if (serializeJson(doc, file) == 0) {
    M5.Lcd.println(F("Failed to write to file"));
  }

  file.close();
  return "Gespeichert";
}

void searchTag(const char *filename, std::string &book)
{
  // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<512> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  M5.Lcd.setCursor(0, 80);
  M5.Lcd.setTextSize(2);
  file.close();

  for(int i = 0; 0 < doc["Reihe1"].size(); i++)
  {
    if ( doc["Reihe1"].getElement(i) == book.c_str())
    {
      M5.Lcd.print("Ihr Ordner is in der ");
      M5.Lcd.println("reihe 1");
      redLed();
      return;
    }
  }

  for(int i = 0; 0 < doc["Reihe2"].size(); i++)
  {
    if ( doc["Reihe2"].getElement(i) == book.c_str())
    {
        M5.Lcd.print("Ihr Ordner is in der ");
        M5.Lcd.println("reihe 2");
        yellowLed();
        return;
    }
  }
  
    M5.Lcd.println("gibt es nicht");
}

bool deleteTag(const char *filename, tag *book)
{
  File file = SD.open(filename);
  StaticJsonDocument<520> doc;
  DeserializationError error = deserializeJson(doc, file);

  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  M5.Lcd.setCursor(0, 150);
  file.close();

  for(int i = 0; i < doc["Reihe1"].size(); i++)
  {
    if (doc["Reihe1"].getElement(i) == (book->name))
    {
        JsonArray array = doc["Reihe1"].to<JsonArray>();
        array.remove(i);
                  
        file = SD.open(filename, FILE_WRITE);
        if (serializeJson(doc, file) == 0) {
            M5.Lcd.println(F("Failed to write to file"));
        }
        file.close();
        return true;
    }
  }

  for(int i = 0; i < doc["Reihe2"].size(); i++)
  {
    if (doc["Reihe2"].getElement(i) == (book->name))
    {
        JsonArray array = doc["Reihe2"].to<JsonArray>();
        array.remove(i); 
        file = SD.open(filename, FILE_WRITE);

        if (serializeJson(doc, file) == 0)
        {
            M5.Lcd.println(F("Failed to write to file"));
        }

        file.close();
        return true;
    }
  }

return false;
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    M5.Lcd.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    M5.Lcd.print((char)file.read());
  }
  M5.Lcd.println();

  // Close the file
  file.close();
}

bool writeTag (const char* text)
{

    if(sizeof(text)>BUF_SIZE-PARITY_SIZE){
        return false;
    }

    byte buffer[BUF_SIZE];

    memcpy(buffer,text,BUF_SIZE-PARITY_SIZE);

    for (int i=0; i < 4; i++) {
        mfrc522.MIFARE_Ultralight_Write(pageAddr+i, &buffer[i*PAGE_SIZE], PAGE_SIZE);
    }
    return true;
}

char* readTag(){

    byte buffer[BUF_SIZE] = {0};
    byte size = sizeof(buffer);
    char convert[BUF_SIZE];

    mfrc522.MIFARE_Read(pageAddr, buffer, &size);

    for (uint8_t i = 0; i < BUF_SIZE-PARITY_SIZE; i++){
        convert[i] = (char)buffer[i];
    }
    
    char* content = (char*) malloc(sizeof(convert));
    if(content==NULL){
        return nullptr;
    }

    return strcpy(content, convert);
}

void scanTag(int option)
{
    String tagName;

    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
        delay(50);
        return;
    }

    M5.Lcd.println(" ");

    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        scannedTag += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        scannedTag += String(mfrc522.uid.uidByte[i], HEX);
    }

    tagName = readTag();

    if (option == 0)
    {
        if((tagName == "Reihe1" || tagName == "Reihe2") && rowTag == nullptr)
        {
            printScan();
            rowTag = new tag();
            rowTag->uid = scannedTag;
            rowTag->name = tagName;
            M5.Lcd.setCursor(0, 80);
            M5.Lcd.setTextSize(2);
            M5.Lcd.print("Reihe: ");
            M5.Lcd.println(rowTag->name);
        }
        else if(tagName != nullptr && rowTag != nullptr && (tagName != "Reihe1" && tagName != "Reihe2") && bookTag == nullptr)
        {
            bookTag = new tag();
            bookTag->uid = scannedTag;
            bookTag->name = tagName;
            printScan();
            M5.Lcd.setCursor(0, 80);
            M5.Lcd.setTextSize(2);
            M5.Lcd.print("Reihe: ");
            M5.Lcd.println(rowTag->name);
            M5.Lcd.print("Ordner: ");
            M5.Lcd.println(bookTag->name);
            M5.Lcd.println(saveTag(filename, rowTag, bookTag));
            delete rowTag;
            delete bookTag;
            rowTag = nullptr;
            bookTag = nullptr;
        }
        else
        {
            M5.Lcd.setCursor(0, 150);
            M5.Lcd.setTextSize(2);
            if(tagName != nullptr  && (tagName != "Reihe1" && tagName != "Reihe2") && rowTag != nullptr)
            {
                M5.Lcd.print("Ordner kenn ich nicht");
            }
            else if(rowTag == nullptr)
            {
                M5.Lcd.print("Reihe nicht gefunden");
            }
            else if(rowTag != nullptr && bookTag == nullptr)
            {
                M5.Lcd.print("Reihe wurde schon gescannt");
            }
        }    
    }
    else if(option == 1)
    {
        if(bookTag == nullptr)
        {
            if(tagName != "Reihe1" && tagName != "Reihe2")
            {
                //printDelete();
                printScan();
                M5.Lcd.setCursor(0, 80);
                M5.Lcd.setTextSize(2);
                M5.Lcd.print("Tagname: ");
                M5.Lcd.println(tagName);
                
                bookTag = new tag();
                bookTag->uid = scannedTag;
                bookTag->name = tagName;
                if(deleteTag(filename, bookTag) == true)
                {
                    M5.Lcd.println("Erfolgreich gelöscht");
                        
                }
                else
                {
                    M5.Lcd.println("Fehlgeschlagen");
                        
                }
                delete bookTag;
                bookTag = nullptr;
            }
        }
    }

  scannedTag.clear();
  delay(1000);
}


