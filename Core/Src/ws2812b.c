/*
 * ws2812b.c
 *
 *  Created on: May 25, 2021
 *      Author: henryco1
 */
#include "ws2812b.h"

void __attribute__( ( optimize( "O0" ) ) )
delay_cycles( uint32_t cyc ) {
  for ( uint32_t d_i = 0; d_i < cyc; ++d_i ) { asm( "NOP" ); }
}

const uint8_t gamma_table[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

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
	memset(frame_buf, 0, grid_x * grid_y * sizeof(unsigned int));
}

void set_red( void ) {
	for ( size_t i = 0; i < NUM_LEDS; i++) {
		set_color (i, get_rgb_color( 0x0C, 0x00, 0x00 ) );
	}
}

void init_frame_buffer(void) {
	srand(time(NULL));
}

/*
 * Initializes the frame buf so that the fire can start or be "ignited" from the bottom row of the grid
 * The initial value of the bottom row should be brightest element from the fire color array.
 * By default, the fire color array is sorted from darkest to lightest in ascending order.
 */
void init_fire_effect(void) {
    for (int i=0; i<grid_x; i++) {
        frame_buf[(grid_y-1) * grid_x + i] = 10;
    }
}
/*
 * Creates a fire animation using the WS2812B LEDs. This function has two major parts:
 * 1. generating a fire model using a 2d array
 * 2. use the fire model to generate color data for the LEDs
 *
 * fire effect based on this gradient https://www.schemecolor.com/fire-gradient.php
 */
void set_fire_effect(void) {
	/*
	 * Flame gradient key
	 * {0, 0, 0},		black
	 * {255,102,0}		dark orange
	 * {255,148,46}		orange
	 *
	 * all non black colors in the table are between dark orange and
	 * orange
	 *
	 * gradient table organize in a "fire" pattern
	 * note that my LEDs might be damaged so the color table might be off
	 *
	 * https://meyerweb.com/eric/tools/color-blend/#FFCC66:FF6600:10:rgbd
	 */
	uint8_t fire_colors[14][3] = {
			 {0, 0, 0},
			 {0, 0, 0},
			 {0, 0, 0},
			 {255,102,0},
			 {255,111,9},
			 {255,121,19},
			 {255,130,28},
			 {255,139,37},
			 {255,148,46},
			 {255,139,37},
			 {255,130,28},
			 {255,121,19},
			 {255,111,9},
			 {255,102,0},
	};

	// funky colors
//	 {204, 255, 0},
//	 {209,240,26},
//	 {214,224,51},
//	 {219,209,77},
//	 {66, 2, 0},
//	 {107, 8, 0},
//	 {192, 10, 0},
//	 {180, 12, 0},
//	 {116, 12, 0},
//	 {116, 12, 0},
//	 {255, 32, 0},

	// fire colors
//	 {0, 0, 0},
//	 {0, 0, 0},
//	 {0, 0, 0},
//	 {16, 1, 0},
//	 {66, 2, 0},
//	 {107, 8, 0},
//	 {192, 10, 0},
//	 {180, 12, 0},
//	 {116, 12, 0},
//	 {116, 12, 0},
//	 {255, 32, 0},

	// l colors
//	 {204, 255, 0},
//	 {209,240,26},
//	 {214,224,51},
//	 {219,209,77},
//	 {224,194,102},
//	 {230,179,128},
//	 {235,163,153},
//	 {240,148,179},
//	 {245,133,204},
//	 {250,117,230},
//	 {255,102,255},

	/*
	 * Use a 2D array to model a fire being ignited
	 * This code ignites the flame from the bottom row. The bottom array needs to be initialized from init_fire_effect().
	 * The r_val_spread variable is needed for a fire flickering effect.
	 * An r_val_spread factor value of 1 means no spread. Higher values increased the spread.
	 * Adjust r_val_height to control the height of the embers in the fire. Lower this value to decrease the height
	 */
	int r_val_spread = 0;
	int r_val_spread_factor = 4; // 3
	int r_val_height = 3; // 3
    for (int x = 0; x < grid_x; x++) {
        for (int y = 1; y < grid_y; y++) {
        	r_val_spread = (rand() % r_val_spread_factor );
            int prev = y * grid_x + x;
            int curr = prev - grid_x;
            frame_buf[curr] = abs(frame_buf[prev] - 1 - (r_val_spread & ((rand() % r_val_height) )));
        }
    }

    // input the 2d fire model into the LEDs
    for (int i=0; i<NUM_LEDS; i++) {
    	set_color(i, get_rgb_color(gamma_table[fire_colors[frame_buf[i]][0]], gamma_table[fire_colors[frame_buf[i]][1]], gamma_table[fire_colors[frame_buf[i]][2]]));
    }
	delay_cycles( 500000 );
}

void set_bi_effect(void) {
	/*
	 * Flame gradient key
	 * {0, 0, 0},		black
	 * {255,102,0}		dark orange
	 * {255,148,46}		orange
	 *
	 * all non black colors in the table are between dark orange and
	 * orange
	 *
	 * gradient table organize in a "fire" pattern
	 * note that my LEDs might be damaged so the color table might be off
	 *
	 * https://meyerweb.com/eric/tools/color-blend/#FFCC66:FF6600:10:rgbd
	 */
	uint8_t fire_colors[11][3] = {
		 {0, 0, 100},
		 {26, 2, 100},
		 {46, 2, 100},
		 {66, 4, 100},
		 {107, 8, 100},
		 {152, 9, 100},
		 {192, 10, 100},
		 {180, 12, 100},
		 {116, 12, 100},
		 {116, 12, 100},
		 {255, 32, 100},
	};

	/*
	 * Use a 2D array to model a fire being ignited
	 * This code ignites the flame from the bottom row. The bottom array needs to be initialized from init_fire_effect().
	 * The r_val_spread variable is needed for a fire flickering effect.
	 * An r_val_spread factor value of 1 means no spread. Higher values increased the spread.
	 * Adjust r_val_height to control the height of the embers in the fire. Lower this value to decrease the height
	 */
	int r_val_spread = 0;
	int r_val_spread_factor = 2; // 3
	int r_val_height = 8; // 3
    for (int x = 0; x < grid_x; x++) {
        for (int y = 1; y < grid_y; y++) {
        	r_val_spread = (rand() % r_val_spread_factor );
            int prev = y * grid_x + x;
            int curr = prev - grid_x;
            frame_buf[curr] = abs(frame_buf[prev] - 1 - (r_val_spread & ((rand() % r_val_height) )));
        }
    }

    // input the 2d fire model into the LEDs
    for (int i=0; i<NUM_LEDS; i++) {
    	set_color(i, get_rgb_color(gamma_table[fire_colors[frame_buf[i]][0]], gamma_table[fire_colors[frame_buf[i]][1]], gamma_table[fire_colors[frame_buf[i]][2]]));
    }
	delay_cycles( 500000 );
}

void set_funky_effect(void) {
	/*
	 * Flame gradient key
	 * {0, 0, 0},		black
	 * {255,102,0}		dark orange
	 * {255,148,46}		orange
	 *
	 * all non black colors in the table are between dark orange and
	 * orange
	 *
	 * gradient table organize in a "fire" pattern
	 * note that my LEDs might be damaged so the color table might be off
	 *
	 * https://meyerweb.com/eric/tools/color-blend/#66FF00:CC0000:10:rgbd
	 */
	uint8_t fire_colors[11][3] = {
		 {111,232,0},
		 {121,209,0},
		 {130,185,0},
		 {139,162,0},
		 {148,139,0},
		 {158,116,0},
		 {167,93,0},
		 {176,70,0},
		 {116, 12, 0},
		 {195,23,0},
		 {204,0,0},
	};

	/*
	 * Use a 2D array to model a fire being ignited
	 * This code ignites the flame from the bottom row. The bottom array needs to be initialized from init_fire_effect().
	 * The r_val_spread variable is needed for a fire flickering effect.
	 * An r_val_spread factor value of 1 means no spread. Higher values increased the spread.
	 * Adjust r_val_height to control the height of the embers in the fire. Lower this value to decrease the height
	 */
	int r_val_spread = 0;
	int r_val_spread_factor = 3; // 3
	int r_val_height = 3; // 3
    for (int x = 0; x < grid_x; x++) {
        for (int y = 1; y < grid_y; y++) {
        	r_val_spread = (rand() % r_val_spread_factor );
            int prev = y * grid_x + x;
            int curr = prev - grid_x;
            frame_buf[curr] = abs(frame_buf[prev] - 1 - (r_val_spread & ((rand() % r_val_height) )));
        }
    }

    // input the 2d fire model into the LEDs
    for (int i=0; i<NUM_LEDS; i++) {
    	set_color(i, get_rgb_color(gamma_table[fire_colors[frame_buf[i]][0]], gamma_table[fire_colors[frame_buf[i]][1]], gamma_table[fire_colors[frame_buf[i]][2]]));
    }
	delay_cycles( 500000 );
}

void set_L_effect(void) {
	/*
	 * Flame gradient key
	 * {0, 0, 0},		black
	 * {255,102,0}		dark orange
	 * {255,148,46}		orange
	 *
	 * all non black colors in the table are between dark orange and
	 * orange
	 *
	 * gradient table organize in a "fire" pattern
	 * note that my LEDs might be damaged so the color table might be off
	 *
	 * https://meyerweb.com/eric/tools/color-blend/#FFCC66:FF6600:10:rgbd
	 */
	uint8_t fire_colors[11][3] = {
		{204, 255, 0},
		{209,240,26},
		{214,224,51},
		{219,209,77},
		{224,194,102},
		{230,179,128},
		{235,163,153},
		{240,148,179},
		{245,133,204},
		{250,117,230},
		{255,102,255},
	};

	/*
	 * Use a 2D array to model a fire being ignited
	 * This code ignites the flame from the bottom row. The bottom array needs to be initialized from init_fire_effect().
	 * The r_val_spread variable is needed for a fire flickering effect.
	 * An r_val_spread factor value of 1 means no spread. Higher values increased the spread.
	 * Adjust r_val_height to control the height of the embers in the fire. Lower this value to decrease the height
	 */
	int r_val_spread = 0;
	int r_val_spread_factor = 3; // 3
	int r_val_height = 3; // 3
    for (int x = 0; x < grid_x; x++) {
        for (int y = 1; y < grid_y; y++) {
        	r_val_spread = (rand() % r_val_spread_factor );
            int prev = y * grid_x + x;
            int curr = prev - grid_x;
            frame_buf[curr] = abs(frame_buf[prev] - 1 - (r_val_spread & ((rand() % r_val_height) )));
        }
    }

    // input the 2d fire model into the LEDs
    for (int i=0; i<NUM_LEDS; i++) {
    	set_color(i, get_rgb_color(gamma_table[fire_colors[frame_buf[i]][0]], gamma_table[fire_colors[frame_buf[i]][1]], gamma_table[fire_colors[frame_buf[i]][2]]));
    }
	delay_cycles( 500000 );
}
