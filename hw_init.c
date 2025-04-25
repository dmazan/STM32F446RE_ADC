#include "hw_init.h"
#include "ssd1306.h"
#include "stm32f4xx_hal_flash.h"

/* Peripheral handles */
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;
DMA_HandleTypeDef hdma_adc3;
UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim2; /* Timer for ADC triggering */

static void ADC_Init(void);
static void DMA_Init(void);
static void UART_Init(void);
static void I2C_Init(void);
static void DAC_Init(void);
static void TIMER_Init(void); /* Timer initialization */

void HW_Init(void)
{
    /* Initialize DMA */
    DMA_Init();

    /* Initialize Timer for ADC triggering */
    TIMER_Init();
    
    /* Initialize ADCs */
    ADC_Init();

    /* Initialize UART and I2C */
    UART_Init();
    I2C_Init();

    /* Initialize DAC */
    DAC_Init();
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initialize the CPU, AHB and APB buses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initialize the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

static void ADC_Init(void)
{
    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    /* Enable ADC clocks and GPIO clock */
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_ADC2_CLK_ENABLE();
    __HAL_RCC_ADC3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure GPIO pins for ADC */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;  // PA0, PA1, PA2
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure ADC1 */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE; /* Disable continuous mode for timer control */
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING; /* Use rising edge of trigger */
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO; /* Timer 2 trigger output */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    HAL_ADC_Init(&hadc1);

    /* Configure ADC2 - Similar to ADC1 */
    hadc2.Instance = ADC2;
    hadc2.Init = hadc1.Init;
    HAL_ADC_Init(&hadc2);

    /* Configure ADC3 - Similar to ADC1 */
    hadc3.Instance = ADC3;
    hadc3.Init = hadc1.Init;
    HAL_ADC_Init(&hadc3);

    /* Configure Multi mode */
    multimode.Mode = ADC_TRIPLEMODE_REGSIMULT;
    multimode.DMAAccessMode = ADC_DMAACCESSMODE_3;
    multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
    HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);

    /* Configure ADC Channels */
    sConfig.Channel = ADC_CHANNEL_0;  // Using channel 0 for ADC1
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    sConfig.Channel = ADC_CHANNEL_1;  // Using channel 1 for ADC2
    HAL_ADC_ConfigChannel(&hadc2, &sConfig);

    sConfig.Channel = ADC_CHANNEL_2;  // Using channel 2 for ADC3
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);
}

static void DMA_Init(void)
{
    /* Enable DMA1 and DMA2 clocks */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA2 Stream0 Configuration for ADC1 */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_adc1);

    /* Associate ADC1 with DMA */
    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    /* Enable DMA2 Stream0 interrupt for ADC1 */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    /* DMA2 Stream3 Configuration for ADC2 */
    //hdma_adc2.Instance = DMA2_Stream1;
    //hdma_adc2.Init.Channel = DMA_CHANNEL_1;
    //hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    //hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    //hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    //hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    //hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    //hdma_adc2.Init.Mode = DMA_CIRCULAR;
    //hdma_adc2.Init.Priority = DMA_PRIORITY_HIGH;
    //HAL_DMA_Init(&hdma_adc2);

    /* Associate ADC2 with DMA */
    __HAL_LINKDMA(&hadc2, DMA_Handle, hdma_adc2);

    /* DMA2 Stream1 Configuration for ADC3 */
    hdma_adc3.Instance = DMA2_Stream2;
    hdma_adc3.Init.Channel = DMA_CHANNEL_2;
    hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc3.Init.Mode = DMA_CIRCULAR;
    hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_adc3);

    /* Associate ADC3 with DMA */
    __HAL_LINKDMA(&hadc3, DMA_Handle, hdma_adc3);
}

static void UART_Init(void)
{
    /* Enable USART1 clock */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* USART1 GPIO Configuration
     * PA9  -> USART1_TX
     * PA10 -> USART1_RX
     */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 initialization */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void I2C_Init(void)
{
    /* Enable I2C1 clock */
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C1 GPIO Configuration
     * PB6 -> I2C1_SCL
     * PB7 -> I2C1_SDA
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    */

    /* I2C1 initialization */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000; // 100KHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}

static void TIMER_Init(void)
{
    /* Enable TIM2 clock */
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    /* Time base configuration */
    /* Timer base frequency = APB1_Timer_Clock / (Prescaler + 1) */
    /* APB1_Timer_Clock = 84 MHz (168MHz/2*1) */
    /* Desired frequency = 100 Hz */
    /* Period = Timer_Clock / (Desired_Frequency * (Prescaler + 1)) - 1 */
    
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 8399; /* 84 MHz / 8400 = 10,000 Hz */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 99; /* 10,000 Hz / 100 = 100 Hz */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Configure TIM2 TRGO to trigger ADC */
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Start Timer */
    HAL_TIM_Base_Start(&htim2);
}

static void DAC_Init(void)
{
    /* Enable DAC clock */
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure GPIO pin for DAC */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;  // PA5 for DAC1_OUT2
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure DAC */
    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure DAC channel */
    DAC_ChannelConfTypeDef sConfig = {0};
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Start DAC */
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}
