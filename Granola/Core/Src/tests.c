/*
 * tests.c
 *
 *  Created on: May 12, 2021
 *      Author: Emirhan Gocturk
 */

#include "tests.h"

enum granola_status ADC_check(void)
{
	enum granola_status status = GR_ERR;
	uint16_t adc_value = 0;
	adc_value = HAL_ADC_GetValue(&hadc1);

	HAL_Delay(100);
	uint16_t previous_adc_value = adc_value;
	adc_value = HAL_ADC_GetValue(&hadc1);

	if (adc_value > previous_adc_value)
	{
		adc_value -= previous_adc_value;
	}
	else
	{
		previous_adc_value -= adc_value;
		adc_value = previous_adc_value;
	}

	(adc_value > 500) ? status = GR_ERR : status = GR_OK;

	return status;
}
