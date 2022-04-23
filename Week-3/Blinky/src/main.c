#include "stm32f4xx_hal.h"

/* PIN Definitions */
#define GREEN_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14
#define LED_GPIO_PORT GPIOG

#define USER_BUTTON_PORT GPIOA
#define USER_BUTTON_PIN GPIO_PIN_0

/* Clocks */
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define USER_BUTTON_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

void LED_Init();
void UserButton_Init();
void SystemClock_Config(void);
void Error_Handler(void);
void EXTI0_Callback(uint16_t GPIO_Pin);
static void MX_NVIC_Init();

uint32_t interruptGenerated = 0;

int main(void) {
    volatile int counter = 0;
    HAL_Init();
    SystemClock_Config();
    MX_NVIC_Init();
    LED_Init();
    UserButton_Init();

    while (1)
    {
      // no code here is as the interrupt turns the led on/off       
    }
    
    return 0;
}



void UserButton_Init() {
    USER_BUTTON_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = USER_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStruct);
    
}

void LED_Init() {
    LED_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GREEN_LED_PIN | RED_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

}

void SysTick_Handler(void) {
  HAL_IncTick();
}

// copied from Cube IDE Generated code
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
   __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

// copied from Cube IDE Generated code
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}

uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
uint32_t counterOutside = 0;
uint32_t counterInside = 0;


// programmer provided callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    counterOutside++;
    currentMillis = HAL_GetTick();

      if(GPIO_Pin == USER_BUTTON_PIN) {
 
        counterInside++;
        HAL_GPIO_TogglePin(LED_GPIO_PORT, GREEN_LED_PIN);
        previousMillis = currentMillis;
        interruptGenerated = 1;
    }
   
}

// copied from Cube IDE Generated code
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

// copied from Cube IDE Generated code
static void MX_NVIC_Init(void)
{
  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  
}