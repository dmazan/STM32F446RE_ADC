#ifndef DAC_TASK_H
#define DAC_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t xDACQueue;
extern TaskHandle_t xDACTaskHandle;

void vDACTask(void *pvParameters);

#endif /* DAC_TASK_H */
