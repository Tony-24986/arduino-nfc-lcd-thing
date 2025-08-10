#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <LCDI2C_Multilingual_MCD.h>
#include <stdio.h>
#include <Servo.h>

#define PN532_IRQ   2
#define PN532_RESET 3
#define sPin 11
#define closed 100
#define open 0
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
LCDI2C_Katakana_Vietnamese lcd(0x26, 20, 4);
Servo servo;
int angle = 90;

byte db[][4] = {{0x5A,0xBD,0x25,0xE9},{0xB6,0xA1,0xE8,0x3},{0x74,0x3E,0xFE,0x6E},{0x72,0xE8,0x92,0x02},{0x33,0x3,0xF1,0x2C},{0xF2,0x6F,0x74,0x6},{0x2,0xAE,0x6E,0xD}};
int lg = sizeof(db)/4;
bool auth(byte id[4]){
  for (int i = 0; i < lg; i++){
    int c = 0;
    for (int j = 0; j < 4; j++){
      Serial.print(db[i][j], 16);
      Serial.print(id[j], 16);
      Serial.println(i);
      Serial.println("");
      if (db[i][j] == id[j])c++;
    }
    if (c == 4)return 1;
  }
  return 0;
}

void setup(void) {
  Serial.begin(115200);
  
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  Serial.println(F("Waiting for an ISO14443A Card ..."));
  lcd.init();
  lcd.backlight();
  lcd.clear();
  servo.attach(sPin);
  servo.write(closed);
  delay(500);
  
}


void loop(void) {
  lcd.setCursor(0, 0);
  lcd.print("Touch your card.");
  lcd.setCursor(0,1);
  lcd.print("Chạm thẻ.       ");
  lcd.setCursor(0, 2);
  lcd.print("カードヲタッチシテクダサイ。");
  uint8_t success;
  uint8_t uid[4] = { 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    if (auth(uid)){
      Serial.println(F("CARD AUTHORISED"));
      lcd.clear();
      lcd.println("Card/Thẻ/カード OK");
      for(angle = closed; angle > open; angle--) {
        servo.write(angle);
        delay(1);
      }
      delay(4000);
      for(angle = open; angle < closed; angle++) {
        servo.write(angle);
        delay(1);
      }
    }
    else {
      Serial.println(F("CARD NOT Authorised"));
      lcd.clear();
      lcd.println("Card/Thẻ NOT OK.");
      lcd.println("カードハダメ。");
      delay(3000);
    }
    lcd.clear();
  }
}
