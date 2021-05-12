/*
 * tests.c
 *
 *  Created on: May 12, 2021
 *      Author: Emirhan Gocturk
 */

#include "tests.h"

uint8_t ADC_check(ADC_HandleTypeDef *adc)
{
	uint8_t status = 1;
	uint16_t adc_value = 0;
	adc_value = HAL_ADC_GetValue(adc);

	//HAL_Delay(5);
	uint16_t previous_adc_value = adc_value;
	adc_value = HAL_ADC_GetValue(adc);

	if (adc_value > previous_adc_value)
	{
		adc_value -= previous_adc_value;
	}
	else
	{
		previous_adc_value -= adc_value;
		adc_value = previous_adc_value;
	}

	if (adc_value > 500)
	{
		status = 1;
	}
	else
	{
		status = 0;
	}

	return status;
}
