/*
 * ws2812b_LED_mock.c
 *
 *  Created on: Oct 4, 2021
 *      Author: henryco1
 */

/*
 * This mocks is a test double for the LED grid.
 */

#include "ws2812b_LED_mock.h"

unsigned int* led_mock_init(void)
{
	unsigned int mock_buf[grid_x*grid_y];
	memset(mock_buf, 0, grid_x * grid_y * sizeof(unsigned int));
	return mock_buf;
}

void display_led_test(unsigned int mock_buf[grid_x*grid_y])
{
	for (int i=0; i<grid_x; i++)
	{
		for (int j=0; j<grid_y; j++)
		{
			printf("%c", mock_buf[i*j] + '0');
		}
		printf("\n");
	}
}

void set_rain_effect_test(void)
{
	unsigned int* rain_buf = led_mock_init();
	display_led_test(rain_buf);
}
