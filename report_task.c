#include "report_task.h"
#include "hw_init.h"
#include "adc_task.h"
#include "dac_task.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_conf.h"
#include <stdio.h>

/* FreeRTOS handle */
TaskHandle_t xReportTaskHandle;

void vReportTask(void *pvParameters)
{
    uint16_t adc_values[3];
    uint32_t dac_value;
    char uart_buffer[64];
    TickType_t xLastWakeTime;
    
    /* Initialize OLED display */
    ssd1306_Init();
    
    /* Initialize the xLastWakeTime variable with the current time */
    xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
        /* Wait for new ADC values */
        if (xQueueReceive(xADCQueue, adc_values, pdMS_TO_TICKS(100)) == pdPASS)
        {
            /* Get latest DAC value */
            xQueuePeek(xDACQueue, &dac_value, 0);
            
            /* Format string for UART */
            int len = snprintf(uart_buffer, sizeof(uart_buffer),
                             "DAC: %4lu  ADC1: %4u  ADC2: %4u  ADC3: %4u\r\n",
                             dac_value, adc_values[0], adc_values[1], adc_values[2]);
            
            /* Send to UART */
            HAL_UART_Transmit(&huart1, (uint8_t*)uart_buffer, len, HAL_MAX_DELAY);
            
            // Update OLED display
            
            // Display DAC value
            snprintf(uart_buffer, sizeof(uart_buffer), "DAC:  %4lu", dac_value);
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString(uart_buffer, Font_7x10, White);
            
            // Display ADC values
            snprintf(uart_buffer, sizeof(uart_buffer), "ADC1: %4u", adc_values[0]);
            ssd1306_SetCursor(0, 16);
            ssd1306_WriteString(uart_buffer, Font_7x10, White);
            
            snprintf(uart_buffer, sizeof(uart_buffer), "ADC2: %4u", adc_values[1]);
            ssd1306_SetCursor(0, 32);
            ssd1306_WriteString(uart_buffer, Font_7x10, White);
            
            snprintf(uart_buffer, sizeof(uart_buffer), "ADC3: %4u", adc_values[2]);
            ssd1306_SetCursor(0, 48);
            ssd1306_WriteString(uart_buffer, Font_7x10, White);
            
            // Update display
            ssd1306_UpdateScreen();

        }
        
        /* Wait for the next cycle (100ms) */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}
