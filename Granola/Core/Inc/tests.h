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
 * @brief status codes for tests
 */
enum granola_status {
	GR_OK = 0,
	GR_ERR,
	GR_WAR,
	GR_BUSY
};

/*
 * @brief Check if ADC can retrieve value
 *
 * ----- The tests performed assume ADC0
 * is connected and receives values from
 * 0 to 4096
 */
enum granola_status ADC_check(void);

#endif /* INC_TESTS_H_ */
