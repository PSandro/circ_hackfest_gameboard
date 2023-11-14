#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include "tile.h"

#define DEBUG

#define RST_PIN 10

tile_info_t tiles[] = 
{
  {false, 9, {0,0,0,0}, 0, 0, 0},
  {false, 8, {0,0,0,0}, 0, 0, 0},
  {false, 7, {0,0,0,0}, 0, 0, 0},
  {false, 6, {0,0,0,0}, 0, 0, 0},
  {false, 5, {0,0,0,0}, 0, 0, 0},
  {false, 4, {0,0,0,0}, 0, 0, 0},
  {false, 3, {0,0,0,0}, 0, 0, 0},
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
  while (!Serial)
    ;

  Serial.print(F("Setting up "));
  Serial.print(NR_OF_TILES);
  Serial.println(F(" Readers. Wish me luck."));

  SPI.begin();

  for (size_t i = 0; i < NR_OF_TILES; i++) {
    delay(20);
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

int read_tile_from_serial(tile_info_t *tile) {
  if (Serial.available() < 1) {
    // not enough data available
    return 1;
  }
  if (Serial.peek() != 0x24) {
    // magic char: $ (0x24)
    // no data for us, skip
    return 1;
  }

  //TODO: read tile (cs_pin, led rgb, led_action)

  return 1;
}



void print_tile_prefix(tile_info_t *tile) {
  Serial.print("Tile[cs=");
  Serial.print(tile->cs_pin);
  Serial.print("]: ");
}

void handle_incoming_tile() {
    tile_info_t incoming_tile;
    incoming_tile.cs_pin = UINT8_MAX;
    int ret = read_tile_from_serial(&incoming_tile);
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
  if (Serial.available() > 0) {
    handle_incoming_tile();
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
      }
    }

    //send tile update to raspberry
    int ret = send_tile_to_serial(*tile);
    if (ret != 0) {
      Serial.println("Could not send tile to serial!");
    }
  }
}
