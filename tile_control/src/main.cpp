#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D3
#define SS_PIN D8

MFRC522 mfrc522(SS_PIN, RST_PIN);

void init_mfrc522() {
	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();
    mfrc522.PCD_SetAntennaGain(MFRC522::RxGain_max);
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void setup() {
	Serial.begin(115200);
	SPI.begin();	
    init_mfrc522();
}

void dump_rfid_uid(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], 16);
    }
}


int card_timeout = 0;

void loop() {
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    if (card_timeout > 0) {
      if (card_timeout == 1) {
        Serial.println("card removed");
      }
      --card_timeout;
    }
  } else {
    if (card_timeout == 0) {
      Serial.print(F("Card UID:"));
      dump_rfid_uid(mfrc522.uid.uidByte, mfrc522.uid.size);
      Serial.println();
    }
    card_timeout = 3;
  }

  delay(200);

}
