#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>
#include <SPI.h>
#include "tile.h"

#define DEBUG

#define RST_PIN 10

#define LED_PIN 2
#define NR_OF_LEDS 14

Adafruit_NeoPixel leds(NR_OF_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

tile_info_t tiles[] = 
{
  {false, 9, 0, {0,0,0,0}, 0, 0, 0},
  {false, 8, 8, {0,0,0,0}, 0, 0, 0},
  {false, 7, 10, {0,0,0,0}, 0, 0, 0},
  {false, 6, 12, {0,0,0,0}, 0, 0, 0},
  {false, 5, 4, {0,0,0,0}, 0, 0, 0},
  {false, 4, 2, {0,0,0,0}, 0, 0, 0},
  {false, 3, 6, {0,0,0,0}, 0, 0, 0},
};

const size_t NR_OF_TILES = sizeof(tiles)/sizeof(tiles[0]);
MFRC522 mfrc522[NR_OF_TILES];


tile_info_t* find_tile_by_cs(uint8_t cs_pin) {
  for (size_t i = 0; i < NR_OF_TILES; i++) {
    if (tiles[i].cs_pin == cs_pin) {
      return &(tiles[i]);
    }
  }
  return NULL;
}

bool tile_has_card(tile_info_t *tile) {

  for (uint8_t i = 0; i < sizeof(tile->tag_id); i++) {
    if (tile->tag_id[i] != 0) {
      return true;
    }
  }
  return false;
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Initialize.
 */
void setup() {

  Serial.begin(115200);

  Serial.print(F("Setting up "));
  Serial.print(NR_OF_TILES);
  Serial.println(F(" Readers. Wish me luck."));

  leds.begin();
  SPI.begin();

  for (size_t i = 0; i < NR_OF_TILES; i++) {
    delay(50);
    mfrc522[i].PCD_Init(tiles[i].cs_pin, RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.flush();
    mfrc522[i].PCD_DumpVersionToSerial();
  }
}


int send_tile_to_serial(tile_info_t tile) {
  //TODO: implement
  Serial.flush();
  return 0;
}

int read_tile_from_serial(byte *buf, size_t len, tile_info_t *tile) {

  tile->cs_pin = buf[0];
  tile->red = buf[1];
  tile->green = buf[2];
  tile->blue = buf[3];
  tile->action = (led_action) buf[4];

#ifdef DEBUG
  Serial.println("Received tile form serial:");
  Serial.print(" cs_pin: ");
  Serial.println(tile->cs_pin);
  Serial.print(" red   : ");
  Serial.println(tile->red);
  Serial.print(" green : ");
  Serial.println(tile->green);
  Serial.print(" blue  : ");
  Serial.println(tile->blue);
  Serial.print(" action: ");
  Serial.println(tile->action);
#endif
  return 0;
}



void print_tile_prefix(tile_info_t *tile) {
  Serial.print("Tile[cs=");
  Serial.print(tile->cs_pin);
  Serial.print("]: ");
}

void handle_incoming_tile(byte *buf, size_t len) {
    tile_info_t incoming_tile;
    incoming_tile.cs_pin = UINT8_MAX;
    int ret = read_tile_from_serial(buf, len, &incoming_tile);
    if (ret == 0) {
      Serial.print(F("read tile "));
      Serial.print(incoming_tile.cs_pin);
      Serial.println();
      if (incoming_tile.cs_pin == UINT8_MAX) {
        Serial.print("Invalid CS_PIN from incoming tile, got ");
        Serial.println(incoming_tile.cs_pin);
        return;
      }

      tile_info_t* local_tile = find_tile_by_cs(incoming_tile.cs_pin);
      if (local_tile == NULL) {
        Serial.println(F("Could not find matching local tile."));
        return;
      }

      local_tile->red = incoming_tile.red;
      local_tile->green = incoming_tile.green;
      local_tile->blue = incoming_tile.blue;
      local_tile->action = incoming_tile.action;

      Serial.println(F("tile saved to local state."));
    } else if (ret < 0) {
      Serial.println("Error during tile reading.");
    }
}

void loop() {
  if (Serial.available() > 3) {
    // magic sequence: ###$
    if (Serial.peek() == 0x23) { // #
      byte buf[50];
      Serial.readBytesUntil(0x24, buf, 4); // read until $
      uint8_t len = Serial.read();
      if (len > 50) {
        Serial.print("raspberry wanted me to read more than 50 bytes: ");
        Serial.println(len);
        Serial.println("I'm not doing that.");
        return;
      }
      Serial.print("Reading ");
      Serial.print(len);
      Serial.println(" bytes form raspberry to buffer.");
      Serial.readBytes(buf, len);
      handle_incoming_tile(buf, len);
      
    }
  }

  MFRC522* reader = NULL;
  tile_info_t* tile = NULL;

  for (size_t i = 0; i < NR_OF_TILES; i++) {
    reader = &(mfrc522[i]);
    tile = &(tiles[i]);
#ifdef DEBUG
    // print_tile_prefix(tile);
    // Serial.println("start processing reader.");
#endif

    if (!reader->PICC_IsNewCardPresent()) {
      byte bufferATQA[2];
      byte bufferSize = sizeof(bufferATQA);

      MFRC522::StatusCode status = reader->PICC_WakeupA(bufferATQA, &bufferSize);
      if (status == MFRC522::STATUS_OK){
#ifdef DEBUG
        print_tile_prefix(tile);
        Serial.println("woke up card.");
#endif
      } else if (status == MFRC522::STATUS_TIMEOUT) {
        // ignore
      } else {
       print_tile_prefix(tile);
       Serial.print("Could not wakeup card status: ");
       Serial.println(MFRC522::GetStatusCodeName(status));
      }
    }

    if (reader->PICC_ReadCardSerial()) {
      tile->is_healthy = true;

      if (reader->uid.size != 4) {
        print_tile_prefix(tile);
        Serial.print("expected uid size of 4 but got.");
        Serial.println(reader->uid.size);
        tile->is_healthy = false;
      } else {
        if (!tile_has_card(tile)) {
          print_tile_prefix(tile);
          Serial.print("new card, uid=");
          dump_byte_array(reader->uid.uidByte, reader->uid.size);
          Serial.println();
          leds.setPixelColor(tile->led_start, leds.Color(0, 255, 255));
          leds.setPixelColor(tile->led_start+1, leds.Color(255, 0, 255));
          leds.show();
        }
        memcpy(tile->tag_id, reader->uid.uidByte, reader->uid.size);
      }

      // Halt PICC
      mfrc522[i].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[i].PCD_StopCrypto1();
    } else {
      if (tile_has_card(tile)) {
        memset(tile->tag_id, 0, 4);
        print_tile_prefix(tile);
        Serial.println("card removed.");
        leds.setPixelColor(tile->led_start, leds.Color(0, 0, 0));
        leds.setPixelColor(tile->led_start+1, leds.Color(0, 0, 0));
        leds.show();
      }
    }

    //send tile update to raspberry
    int ret = send_tile_to_serial(*tile);
    if (ret != 0) {
      Serial.println("Could not send tile to serial!");
    }
  }
}
