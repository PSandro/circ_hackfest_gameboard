#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);


void setup() {
	Serial.begin(115200);
	SPI.begin();	
	mfrc522.PCD_Init();
	delay(200);
    mfrc522.PCD_DumpVersionToSerial();  // Show version of PCD - MFRC522 Card Reader
    Serial.println();
}

void dump_rfid_uid(byte *buffer, byte bufferSize) {
  String s;
    for (byte i = 0; i < bufferSize; i++) {
      if (buffer[i] < 0x10) {
        s += '0';
      }
      s += String(buffer[i], 16);
    }
  Serial.print(s);

}


void loop() {
   if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    Serial.println();
    dump_rfid_uid(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();

}
