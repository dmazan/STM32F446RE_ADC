#include "dac_task.h"
#include "hw_init.h"

/* FreeRTOS handles */
QueueHandle_t xDACQueue;
TaskHandle_t xDACTaskHandle;

void vDACTask(void *pvParameters)
{
    uint32_t dac_value = 250;
    const uint32_t increment = 100;
    const uint32_t max_value = 4095; // 12-bit DAC
    TickType_t xLastWakeTime;

    /* Initialize the xLastWakeTime variable with the current time */
    xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        /* Set DAC output */
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value);

        /* Update queue with current value */
        xQueueOverwrite(xDACQueue, &dac_value);

        /* Increment value */
        if (dac_value >= max_value)
        {
            dac_value = 0;
        }
        else
        {
            dac_value += increment;
        }

        /* Wait for 1 second */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
    }
}
