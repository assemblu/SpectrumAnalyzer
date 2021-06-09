#ifndef __TFTAPI_H__
#define __TFTAPI_H__

#include "main.h"
#include "lcd.h"


void GPIO_DrawMode(void);
void drawOutline(void);
void setRange(uint32_t* range, uint32_t* size);

#endif
