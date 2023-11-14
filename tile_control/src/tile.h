#pragma once

#include <stdint.h>

typedef enum 
{
  STATIC = 0,
  BLINK = 1
}
led_action;

typedef struct
{
    bool is_healthy;

    uint8_t cs_pin;

    uint8_t tag_id[4];

    short red;
    short green;
    short blue;
    led_action action;

} tile_info_t;
