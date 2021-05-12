/*
 * tests.h
 *
 *  Created on: May 12, 2021
 *      Author: Emirhan Gocturk
 */

#ifndef INC_TESTS_H_
#define INC_TESTS_H_


#include "main.h"

/*
 * @brief Check if ADC can retrieve value
 *
 * ----- The tests performed assume ADC0
 * is connected and receives values from
 * 0 to 4096
 */
uint8_t ADC_check(ADC_HandleTypeDef *adc);

#endif /* INC_TESTS_H_ */
