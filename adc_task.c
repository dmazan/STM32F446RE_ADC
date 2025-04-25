#include "adc_task.h"
#include "hw_init.h"

/* ADC buffer with 4-byte alignment for proper DMA transfers */
static uint16_t adc_values[3] __attribute__((aligned(4)));

/* FreeRTOS handles */
QueueHandle_t xADCQueue;
SemaphoreHandle_t xADCDataMutex;
TaskHandle_t xADCTaskHandle;

void vADCTask(void *pvParameters)
{
    /* In triple mode, we need to ensure proper alignment and use appropriate DMA settings */
    /* Start master ADC with DMA to capture all three ADC values */
    /* With timer triggering, we only need to start the ADC with DMA */
    /* The timer configured at 100Hz will trigger the ADC conversions */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_values[0], 1);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&adc_values[1], 1);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t*)&adc_values[2], 1);

    uint16_t local_adc_values[3] = {99, 99, 99};
    for (;;)
    {
        /* Wait for notification from ADC conversion complete callback */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Take mutex to safely access ADC values */
        if (xSemaphoreTake(xADCDataMutex, pdMS_TO_TICKS(10)) == pdTRUE)
        {
            /* Copy values to local buffer */
            local_adc_values[0] = adc_values[0];
            local_adc_values[1] = adc_values[1];
            local_adc_values[2] = adc_values[2];

            /* Release mutex */
            xSemaphoreGive(xADCDataMutex);
            
            /* Send values to reporting task */
            xQueueOverwrite(xADCQueue, &local_adc_values);
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    /* For debugging - check ADC common data register contents */
    volatile uint32_t cdr_value = ADC->CDR;
    volatile uint16_t adc1_value = ADC1->DR;
    volatile uint16_t adc2_value = ADC2->DR;
    volatile uint16_t adc3_value = ADC3->DR;
    
    /* The following are the actual buffer values we're getting from DMA */
    volatile uint16_t dma_value1 = adc_values[0];
    volatile uint16_t dma_value2 = adc_values[1];
    volatile uint16_t dma_value3 = adc_values[2];
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    /* Notify the ADC task that new data is available */
    if (hadc->Instance == ADC1) {
        vTaskNotifyGiveFromISR(xADCTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
