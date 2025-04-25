/**
 * Multi-ADC simultaneous sampling with DMA for STM32F744RE
 * Uses all three ADC instances in simultaneous mode with DMA
 * Running under FreeRTOS
 */

#include "hw_init.h"
#include "adc_task.h"
#include "dac_task.h"
#include "report_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

int main(void)
{
    /* Reset of all peripherals, initializes the Flash interface and the Systick */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all hardware */
    HW_Init();

    /* Create mutex for ADC data protection */
    xADCDataMutex = xSemaphoreCreateMutex();
    if (xADCDataMutex == NULL) {
        Error_Handler();
    }

    /* Create queues */
    xADCQueue = xQueueCreate(1, sizeof(uint16_t[3]));
    if (xADCQueue == NULL) {
        Error_Handler();
    }
    vQueueAddToRegistry(xADCQueue, "ADCQueue");

    xDACQueue = xQueueCreate(1, sizeof(uint32_t));
    if (xDACQueue == NULL) {
        Error_Handler();
    }
    vQueueAddToRegistry(xDACQueue, "DACQueue");

    /* Create tasks */
    
    BaseType_t xReturn;

    xReturn = xTaskCreate(vADCTask, "ADC Task",
        configMINIMAL_STACK_SIZE * 2,
        NULL, tskIDLE_PRIORITY + 3, &xADCTaskHandle);
    if (xReturn != pdPASS) {
        Error_Handler();
    }

    xReturn = xTaskCreate(vDACTask, "DAC Task",
        configMINIMAL_STACK_SIZE * 2,
        NULL, tskIDLE_PRIORITY + 3, &xDACTaskHandle);
    if (xReturn != pdPASS) {
        Error_Handler();
    }

    xReturn = xTaskCreate(vReportTask, "Report Task",
        configMINIMAL_STACK_SIZE * 3,
        NULL, tskIDLE_PRIORITY + 2, &xReportTaskHandle);
    if (xReturn != pdPASS) {
        Error_Handler();
    }

    /* Start the scheduler */
    vTaskStartScheduler();

    /* We should never get here as control is now taken by the scheduler */
    while (1) {}
}


void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    while(1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number */
}
#endif
