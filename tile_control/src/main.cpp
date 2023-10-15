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

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], 16);
    }
}

void loop() {
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  delay(500);

}
