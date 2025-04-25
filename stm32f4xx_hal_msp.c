#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_dma.h"

void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if(hadc->Instance == ADC1 || hadc->Instance == ADC2 || hadc->Instance == ADC3)
    {
        /* ADC GPIO Configuration */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_ADC2_CLK_ENABLE();
        __HAL_RCC_ADC3_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC DMA Init */
        if(hadc->Instance == ADC1)
        {
            static DMA_HandleTypeDef hdma_adc1;
            __HAL_RCC_DMA2_CLK_ENABLE();
            hdma_adc1.Instance = DMA2_Stream0;
            hdma_adc1.Init.Channel = DMA_CHANNEL_0;
            hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
            hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_adc1.Init.Mode = DMA_CIRCULAR;
            hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
            hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            HAL_DMA_Init(&hdma_adc1);
            __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);
        }
        else if(hadc->Instance == ADC2)
        {
            static DMA_HandleTypeDef hdma_adc2;
            __HAL_RCC_DMA2_CLK_ENABLE();
            hdma_adc2.Instance = DMA2_Stream2;
            hdma_adc2.Init.Channel = DMA_CHANNEL_1;
            hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
            hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_adc2.Init.Mode = DMA_CIRCULAR;
            hdma_adc2.Init.Priority = DMA_PRIORITY_HIGH;
            hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            HAL_DMA_Init(&hdma_adc2);
            __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc2);
        }
        else if(hadc->Instance == ADC3)
        {
            static DMA_HandleTypeDef hdma_adc3;
            __HAL_RCC_DMA2_CLK_ENABLE();
            hdma_adc3.Instance = DMA2_Stream1;
            hdma_adc3.Init.Channel = DMA_CHANNEL_2;
            hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
            hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_adc3.Init.Mode = DMA_CIRCULAR;
            hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
            hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            HAL_DMA_Init(&hdma_adc3);
            __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc3);
        }
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_I2C1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hdac->Instance == DAC)
    {
        __HAL_RCC_DAC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}
