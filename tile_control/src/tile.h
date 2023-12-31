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

    uint8_t led_start;

    uint8_t tag_id[4];

    uint8_t red_1;
    uint8_t green_1;
    uint8_t blue_1;

    uint8_t red_2;
    uint8_t green_2;
    uint8_t blue_2;

    led_action action;

} tile_info_t;
