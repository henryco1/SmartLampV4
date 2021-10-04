/*
 * ws2812b_LED_mock.h
 *
 *  Created on: Oct 4, 2021
 *      Author: Lommb
 */

#ifndef TESTS_WS2812B_LED_MOCK_H_
#define TESTS_WS2812B_LED_MOCK_H_

#include "ws2812b.h"

unsigned int* led_mock_init(void);
void display_led_test(unsigned int mock_buf[grid_x*grid_y]);
void set_rain_effect_test(void);

#endif /* TESTS_WS2812B_LED_MOCK_H_ */
