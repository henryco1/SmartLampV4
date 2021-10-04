/*
 * ws2812b.h
 *
 *  Created on: May 25, 2021
 *      Author: henryco1
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Array of LED colors. G/R/B/G/R/B/...
#define NUM_LEDS  ( 52 )
#define LED_BYTES ( ( NUM_LEDS * 3 * 8 ) + 64 )
// Max brightness (out of a possible 255)
#define MAX_B ( 63 )
// How quickly to increment/decrement the colors.
#define B_INC ( 1 )

#define grid_x 4
#define grid_y 13

uint8_t COLORS[ LED_BYTES ];
unsigned int frame_buf[grid_x*grid_y];

uint32_t get_rgb_color( uint8_t r, uint8_t g, uint8_t b );
void set_color( size_t led_index, uint32_t col );
uint8_t get_led_r( size_t led_num );
uint8_t get_led_g( size_t led_num );
uint8_t get_led_b( size_t led_num );
void rainbow( void );
void reset_leds( void );
void set_fire_effect(void);
void set_bi_effect(void);
void set_funky_effect(void);
void init_fire_effect(void);
void set_L_effect(void);
void delay_cycles(uint32_t);

#endif /* INC_WS2812B_H_ */
