#ifndef HW_INIT_H
#define HW_INIT_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_dac.h"

/* Peripheral handles */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;
extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;
extern DAC_HandleTypeDef hdac;

void SystemClock_Config(void);
void HW_Init(void);
void Error_Handler(void) __attribute__((weak));

#endif /* HW_INIT_H */
