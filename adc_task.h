#ifndef ADC_TASK_H
#define ADC_TASK_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern QueueHandle_t xADCQueue;
extern SemaphoreHandle_t xADCDataMutex;
extern TaskHandle_t xADCTaskHandle;

void vADCTask(void *pvParameters);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif /* ADC_TASK_H */
