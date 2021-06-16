#include "tftapi.h"

void GPIO_DrawMode() {
	/* GPIO Ports Clock Enable */
	__GPIOA_CLK_ENABLE()
	;
	__GPIOB_CLK_ENABLE()
	;
	HAL_NVIC_DisableIRQ(EXTI4_IRQn);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/*Configure GPIO pins: PA1 PA4 PA8 */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_8;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO data pin PB10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void drawOutline(void)
{
	GPIO_DrawMode();
	LCD_SetCursor(0, 0);
	// y
	LCD_DrawLine(20, 220, 300, 220, BLACK);
	// x
	LCD_DrawLine(20, 20, 20, 220, BLACK);
}

void setRange(uint32_t* range, uint32_t* size)
{
	uint32_t index = 0;
	uint8_t drawingBaseOffset = 20;
	uint8_t drawingOffset = 40;

	// print first element
	//LCD_SetCursor(drawingBaseOffset, 225);
	//LCD_Printf("0");
	//(range[0] < 1000) ? (LCD_Printf("500Hz")) : (LCD_Printf("1khz"));
	for (index = 0; index < size; index++)
	{
		LCD_SetCursor(drawingBaseOffset + drawingOffset * index, 225);
		LCD_Printf("%.1f", (float)range[index]/1000);

		// do something
	}
}
