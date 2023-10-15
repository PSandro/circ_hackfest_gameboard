#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <SSD1306Wire.h>

#define RST_PIN D3
#define SS_PIN D8

MFRC522 mfrc522(SS_PIN, RST_PIN);
SSD1306Wire display(0x3c, SDA, SCL);


void init_mfrc522() {
	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();
    mfrc522.PCD_SetAntennaGain(MFRC522::RxGain_max);
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void init_oled() {
  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.display();

}

void setup() {
	Serial.begin(115200);
    init_oled();
	SPI.begin();	
    init_mfrc522();
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
  display.drawString(0, 0, s);
  display.display();

}


int card_timeout = 0;

void loop() {
  mfrc522.PCD_Init();
  delay(4);
  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    if (card_timeout > 0) {
      if (card_timeout == 1) {
        Serial.println("card removed");
        display.clear();
        display.display();
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
