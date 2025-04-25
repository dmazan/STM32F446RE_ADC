/**
 * @file stm32_hal_timebase_tim.c
 * @brief HAL timebase redirected to TIM6
 */

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

TIM_HandleTypeDef htim6;

/* The period is set for a 1ms timebase */
#define TIM_PERIOD             (1000 - 1)
#define TIM_PRESCALER          ((SystemCoreClock / 1000000) - 1)

/**
 * @brief  This function configures TIM6 as a time base source
 * @note   The time source is configured to have 1ms time base with a dedicated
 *         Tick interrupt priority.
 * @retval HAL status
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef clkconfig;
    uint32_t uwTimclock, uwAPB1Prescaler = 0U;
    uint32_t uwPrescalerValue = 0U;
    uint32_t pFLatency;

    /* Enable TIM6 clock */
    __HAL_RCC_TIM6_CLK_ENABLE();

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;

    /* Compute TIM6 clock */
    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIM6 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t)((uwTimclock / 1000000U) - 1U);

    /* Initialize TIM6 */
    htim6.Instance = TIM6;
    htim6.Init.Period = 999; /* 1ms (1000-1) */
    htim6.Init.Prescaler = uwPrescalerValue;
    htim6.Init.ClockDivision = 0;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Start the TIM time Base generation in interrupt mode */
    if (HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Set the priority for TIM6 interrupt */
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0);

    /* Enable the TIM6 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    /* Return function status */
    return HAL_OK;
}

/**
 * @brief  Suspend the time base source
 * @retval None
 */
void HAL_SuspendTick(void)
{
    /* Disable TIM6 update interrupt */
    __HAL_TIM_DISABLE_IT(&htim6, TIM_IT_UPDATE);
}

/**
 * @brief  Resume the time base source
 * @retval None
 */
void HAL_ResumeTick(void)
{
    /* Enable TIM6 update interrupt */
    __HAL_TIM_ENABLE_IT(&htim6, TIM_IT_UPDATE);
}

/**
 * @brief  HAL_Delay implementation with FreeRTOS awareness
 * @param  Delay: Delay in milliseconds
 * @retval None
 */
void HAL_Delay(uint32_t Delay)
{
    /* If we're in a FreeRTOS context, use proper task delay */
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        vTaskDelay(pdMS_TO_TICKS(Delay));
    }
    else
    {
        /* Otherwise use polling delay */
        uint32_t tickstart = HAL_GetTick();
        uint32_t wait = Delay;

        /* Add a period to guarantee minimum wait */
        if (wait < HAL_MAX_DELAY)
        {
            wait += 1;
        }

        while ((HAL_GetTick() - tickstart) < wait)
        {
        }
    }
}

/**
 * @brief  TIM6 IRQHandler
 * @retval None
 */
void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

/**
 * @brief  Timer PeriodElapsed callback in non-blocking mode
 * @param  htim: TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        HAL_IncTick();
    }
}

/**
 * @brief  Returns the HAL tick count (using HAL internal uwTick)
 * @retval Tick count
 */
uint32_t HAL_GetTick(void)
{
    /* Use the HAL's internal uwTick variable */
    extern __IO uint32_t uwTick;
    return uwTick;
}

/**
 * @brief  Increment the HAL tick count
 * @retval None
 */
void HAL_IncTick(void)
{
    /* Use the HAL's internal uwTick variable */
    extern __IO uint32_t uwTick;
    uwTick++;
}
