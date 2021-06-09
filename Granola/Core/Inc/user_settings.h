#ifndef INC_USER_SETTINGS_H_
#define INC_USER_SETTINGS_H_

#define  WIDTH    ((uint16_t)240)
#define  HEIGHT   ((uint16_t)320)

/****************** delay in microseconds ***********************/
extern TIM_HandleTypeDef htim1;
void delay (uint32_t time)
{
	/* change your code here for the delay in microseconds */
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim1))<time);
}

#define write_8(d) { \
 GPIOA->BSRR = 0b1000000000100000 << 16; \
 GPIOB->BSRR = 0b0000000001111011 << 16; \
 GPIOA->BSRR = (((d) & (1<<2)) << 13) \
             | (((d) & (1<<7)) >> 2); \
 GPIOB->BSRR = (((d) & (1<<0)) << 0) \
             | (((d) & (1<<1)) << 0) \
		   | (((d) & (1<<3)) << 0) \
		   | (((d) & (1<<4)) << 0) \
		   | (((d) & (1<<5)) << 0) \
		   | (((d) & (1<<6)) << 0); \
  }

#define read_8() (          (((GPIOB->IDR & (1<<0)) >> 0) \
                         | ((GPIOB->IDR & (1<<1)) >> 0) \
                         | ((GPIOA->IDR & (1<<15)) >> 13) \
                         | ((GPIOB->IDR & (1<<3)) >> 0) \
                         | ((GPIOB->IDR & (1<<4)) >> 0) \
                         | ((GPIOB->IDR & (1<<5)) >> 0) \
                         | ((GPIOB->IDR & (1<<6)) >> 0) \
                         | ((GPIOA->IDR & (1<<5)) << 2)))


#define WRITE_DELAY { }
#define READ_DELAY  { RD_ACTIVE;  }

#define RD_PORT GPIOA
#define RD_PIN  GPIO_PIN_0
#define WR_PORT GPIOA
#define WR_PIN  GPIO_PIN_1
#define CD_PORT GPIOA          // RS PORT
#define CD_PIN  GPIO_PIN_4     // RS PIN
#define CS_PORT GPIOB
#define CS_PIN  GPIO_PIN_0
#define RESET_PORT GPIOC
#define RESET_PIN  GPIO_PIN_1

#define D0_PORT GPIOA
#define D0_PIN GPIO_PIN_9
#define D1_PORT GPIOC
#define D1_PIN GPIO_PIN_7
#define D2_PORT GPIOA
#define D2_PIN GPIO_PIN_10
#define D3_PORT GPIOB
#define D3_PIN GPIO_PIN_3
#define D4_PORT GPIOB
#define D4_PIN GPIO_PIN_5
#define D5_PORT GPIOB
#define D5_PIN GPIO_PIN_4
#define D6_PORT GPIOB
#define D6_PIN GPIO_PIN_10
#define D7_PORT GPIOA
#define D7_PIN GPIO_PIN_8

#endif /* INC_USER_SETTINGS_H_ */
