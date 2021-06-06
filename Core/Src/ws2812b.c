/*
 * ws2812b.c
 *
 *  Created on: May 25, 2021
 *      Author: henryco1
 */
#include "ws2812b.h"

//void __attribute__( ( optimize( "O0" ) ) )
//delay_cycles( uint32_t cyc ) {
//  for ( uint32_t d_i = 0; d_i < cyc; ++d_i ) { asm( "NOP" ); }
//}

// Get a 32-bit representation of R/G/B colors.
uint32_t get_rgb_color( uint8_t r, uint8_t g, uint8_t b ) {
  return ( g << 16 | r << 8 | b );
}
// Set an LED to an RGB color.
void set_color( size_t led_index, uint32_t col ) {
  size_t led_base = led_index * 24;
  uint8_t r = ( col >> 8 ) & 0xFF;
  uint8_t g = ( col >> 16 ) & 0xFF;
  uint8_t b = ( col ) & 0xFF;
  for ( size_t i = 0; i < 8; ++i ) {
    if ( g & ( 1 << ( 7 - i ) ) ) { COLORS[ i + led_base ] = 0xF8; }
    else { COLORS[ i + led_base ] = 0xC0; }
  }
  for ( size_t i = 0; i < 8; ++i ) {
    if ( r & ( 1 << ( 7 - i ) ) ) { COLORS[ i + led_base + 8 ] = 0xF8; }
    else { COLORS[ i + led_base + 8 ] = 0xC0; }
  }
  for ( size_t i = 0; i < 8; ++i ) {
    if ( b & ( 1 << ( 7 - i ) ) ) { COLORS[ i + led_base + 16 ] = 0xF8; }
    else { COLORS[ i + led_base + 16 ] = 0xC0; }
  }
}
// Get the red component of an LED color.
uint8_t get_led_r( size_t led_num ) {
  uint8_t r = 0x00;
  for ( size_t i = 0; i < 8; ++i ) {
    if ( COLORS[ ( led_num * 24 ) + 8 + i ] != 0xC0 ) {
      r = r | ( 1 << ( 7 - i ) );
    }
  }
  return r;
}
// Get the green component of an LED color.
uint8_t get_led_g( size_t led_num ) {
  uint8_t g = 0x00;
  for ( size_t i = 0; i < 8; ++i ) {
    if ( COLORS[ ( led_num * 24 ) + i ] != 0xC0 ) {
      g = g | ( 1 << ( 7 - i ) );
    }
  }
  return g;
}
// Get the blue component of an LED color.
uint8_t get_led_b( size_t led_num ) {
  uint8_t b = 0x00;
  for ( size_t i = 0; i < 8; ++i ) {
    if ( COLORS[ ( led_num * 24 ) + 16 + i ] != 0xC0 ) {
      b = b | ( 1 << ( 7 - i ) );
    }
  }
  return b;
}

// Cycle the array of colors through a rainbow.
// Red -> Purple -> Blue -> Teal -> Green -> Yellow -> Red
// - If red > 0 and < max, if blue is 0, add red.
// - If red is max and blue is < max, add blue.
// - If blue is max and red is > 0, remove red.
// - If blue is max and green < 0, add green.
// - If green is max and blue > 0, remove blue.
// - If green is max and red < 0, add red.
// - If red is max and green is > 0, remove green.
void rainbow( void ) {
  uint8_t r = get_led_r( 0 );
  uint8_t g = get_led_g( 0 );
  uint8_t b = get_led_b( 0 );
  for ( int i = 0; i < NUM_LEDS; ++i ) {
    if ( r == 0 && g == 0 && b == 0 ) { r = B_INC; }
    if ( r > 0 && r < MAX_B && b == 0 ) { r += B_INC; }
    else if ( r >= MAX_B && b < MAX_B && g == 0 ) { b += B_INC; }
    else if ( b >= MAX_B && r > 0 ) { r -= B_INC; }
    else if ( b >= MAX_B && g < MAX_B ) { g += B_INC; }
    else if ( g >= MAX_B && b > 0 ) { b -= B_INC; }
    else if ( g >= MAX_B && r < MAX_B ) { r += B_INC; }
    else if ( r >= MAX_B && g > 0 ) { g -= B_INC; }
    else { r = 0; g = 0; b = 0; }
    set_color( i, get_rgb_color( r, g, b ) );
  }
}

// reset leds
void reset_leds( void ) {
	// Set initial colors to 'off'.
	for ( size_t i = 0; i < 300; ++i ) {
		set_color( i, get_rgb_color( 0x00, 0x00, 0x00 ) );
	}
	// Set the latching period to all 0s.
	for ( size_t i = LED_BYTES - 64; i < LED_BYTES; ++i ) {
		COLORS[ i ] = 0x00;
	}
}

void set_red( void ) {
	for ( size_t i = 0; i < NUM_LEDS; i++) {
		set_color (i, get_rgb_color( 0x0C, 0x00, 0x00 ) );
	}
}

/*
 * fire effect based on this gradient https://www.schemecolor.com/fire-gradient.php
 */
void set_fire_effect(void) {
	int grid_x = 6;
	int grid_y = 6;
	/*
	 * Flame gradient key
	 * {255, 0, 0},		red
	 * {210, 4, 0},		red orange
	 * {161, 16, 0},	orange
	 * {191, 32, 0},	yellow orange
	 * {243, 60, 4},	light orange
	 *
	 * gradient table organize in a "fire" pattern
	 */
	uint8_t fire_colors[10][3] = {
			 {255, 0, 0},
			 {210, 4, 0},
			 {161, 16, 0},
			 {191, 32, 0},
			 {243, 60, 4},
			 {243, 60, 4},
			 {191, 32, 0},
			 {161, 16, 0},
			 {210, 4, 0},
			 {255, 0, 0}
	};

//	uint8_t fire_colors[10][3] = {
//			 {128, 0, 0},
//			 {105, 2, 0},
//			 {80, 8, 0},
//			 {95, 16, 0},
//			 {122, 30, 2},
//			 {122, 30, 2},
//			 {95, 16, 0},
//			 {80, 8, 0},
//			 {105, 2, 0},
//			 {128, 0, 0}
//	};

	const uint8_t FIRE_COLOR_SIZE = 10;
	size_t i = 0;
	// generate a pyramid shape
	for (size_t x=0; x<grid_x-1; x++) {
		size_t iterator = 0;
		for (size_t y=0; y<grid_y-1, i<NUM_LEDS; y++, i++) {
			double brightness = (double)rand() / (double)RAND_MAX;
			iterator = (x+y) % FIRE_COLOR_SIZE;
			set_color(i, get_rgb_color(fire_colors[iterator][0]*brightness, fire_colors[iterator][1]*brightness, fire_colors[iterator][2]*brightness));
		}
		delay_cycles( 500000 );
//		delay_cycles( 100000 );
	}
}
