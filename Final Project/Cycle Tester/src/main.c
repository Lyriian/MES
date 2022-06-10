#include "main.h"
#include "console.h"
#include "encoder.h"
#include "I2C_LCD16x02.h"
#include <stdio.h>
#include <inttypes.h>

/* Pin Definitions */
#define MOTOR_PIN_FORWARD GPIO_PIN_5
#define MOTOR_PIN_REVERSE GPIO_PIN_4
#define MOTOR_PORT GPIOD

/* clocks */
#define MOTOR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

void MOTOR_Init(void);
void UserButton_Init(void);
void EXTI0_Callback(uint16_t GPIO_Pin);
void SystemClock_Config(void);
void Error_Handler(void);
void LCD_Init(void);
void CycleTest(void);
void Encoder(void);
void Encoder_GPIO_Init();
void Encoder_Init_Tim2();
void Direction();

uint32_t cycles = 0;
char MSG[100] = {'\0'};
int main(void) {

  HAL_Init();

  // Setup motor and dev board button
  MOTOR_Init();

  // Configure system timer
  SystemClock_Config();

  // Configure LCD
  LCD_Init();

  // start timer
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  // Setup Command Line Console
  ConsoleInit();

  // call encoder setup
  Encoder_GPIO_Init();
  Encoder_Init_Tim2();
  TIM2->EGR = 1;
  TIM2->CR1 = 1;

  while(1) {
    //ConsoleProcess();
    //CycleTest();
    Direction();
    
    //HAL_Delay(100);
  }

}

void CycleTest(){
  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)){
    LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
    LCD_Send_String("Motor stopped");
  }
  else{
    cycles++;
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_FORWARD, GPIO_PIN_SET);
    LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
    LCD_Send_String("Motor Reverse");
    HAL_Delay(5000);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_FORWARD, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_REVERSE, GPIO_PIN_SET);
    LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
    LCD_Send_String("Motor Forward");
    HAL_Delay(5000);
    HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_REVERSE, GPIO_PIN_RESET);
    return;
  }
}

void Encoder(){
  int16_t count;
  count=TIM2->CNT;
  char str [80];
  if (HAL_GPIO_ReadPin (GPIOD, GPIO_PIN_7)){
    LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
    LCD_Send_String_On_Line1("encoder released");
    sprintf(str, "%u", count);
    LCD_Send_String_On_Line2(str);
  }
  else
  {
    LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
    LCD_Send_String_On_Line1("Encoder pressed");
    LCD_Send_String_On_Line2("test");
  }
  HAL_Delay(100);
}

void Direction(){
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET)  // If the OUTA is RESET
	  {
		  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET)  // If OUTB is also reset... CCK
		  {
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET){};  // wait for the OUTB to go high
			  LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
        LCD_Send_String("Reverse");
			  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){};  // wait for the OUTA to go high
			  HAL_Delay (10);  // wait for some more time
		  }

		  else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET)  // If OUTB is also set
		  {
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET){};  // wait for the OUTB to go LOW.. CK
			  LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
        LCD_Send_String("Forward");
			  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){};  // wait for the OUTA to go high
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET){};  // wait for the OUTB to go high
			  HAL_Delay (10);  // wait for some more time
		  }
	  }
}

/* configure the motor GPIOs */
void MOTOR_Init(){
    MOTOR_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStruct);
}

/* void SysTick_Handler(void)
{
  HAL_IncTick();
}
*/
/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == (GPIO_B, GPIO_PIN_8)){
    HAL_GPIO_TogglePin(MOTOR_PORT, MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE);
  }
}
*/

/* function to read pin state for console */
int PinState(){
  return HAL_GPIO_ReadPin(MOTOR_PORT, MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE);
}

int timer(){
  return ((TIM2->CNT)>>2);
}
/* function to read cycle count for console */
int getCycleCount(){
  return cycles;
}

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

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}
