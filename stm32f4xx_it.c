/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "adc_task.h"

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;
extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;

/* Private function prototypes -----------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/* NOTE: SVC_Handler, PendSV_Handler, and SysTick_Handler are provided by FreeRTOS */

/**
  * @brief This function handles DMA2 stream0 global interrupt (ADC1).
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* Call standard HAL IRQ handler first to clear any flags */
  HAL_DMA_IRQHandler(&hdma_adc1);
  
  /* Since the HAL callback chain is not working properly, directly notify the ADC task */
  /* This is a workaround for a HAL library internal state tracking issue */
  HAL_ADC_ConvCpltCallback(&hadc1);
}

/* In triple simultaneous mode, only ADC1's DMA is relevant */

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}
