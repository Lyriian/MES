#include "main.h"
#include "console.h"
#include "encoder.h"
#include "I2C_LCD16x02.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/* Pin Definitions */
#define MOTOR_PIN_FORWARD GPIO_PIN_5
#define MOTOR_PIN_REVERSE GPIO_PIN_4
#define MOTOR_PORT GPIOD

#define E_STOP_PIN GPIO_PIN_0
#define E_STOP_PORT GPIOA

/* clocks */
#define MOTOR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
 
void MOTOR_Init(void);
void E_STOP_Init();
void EXTI0_Callback(uint16_t E_STOP);
void SystemClock_Config(void);
static void MX_NVIC_Init();
void Error_Handler(void);
void LCD_Init(void);
void CycleTest(void);
void Encoder(void);
void Encoder_GPIO_Init();
void Encoder_Init_Tim2();
void Direction();
void Start();
void Push();
void Pull();
void CycleLimit();

// default test parameters are set here
uint32_t cycles = 0;
uint32_t cyclelimit = 10000;
uint32_t push = 5000;
uint32_t pull = 5000;

int main(void) {

  HAL_Init();

  // Setup motor and dev board button
  MOTOR_Init();
  E_STOP_Init();

  // Configure system timer
  SystemClock_Config();
  MX_NVIC_Init();
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
  
  // Create the main menu
  char LCDMENU[][20] = {"Start test      ", "Push time       ", "Pull time       ", "Cycle limit     "};
  // Set where to start in the menu
  int MenuPos = 0;

  //main loop that the user will interact with
  while(1) {
    ConsoleProcess();
    LCD_Send_String_On_Line1(LCDMENU[MenuPos]);
    MenuPos = (TIM2->CNT)/4;
    if ((TIM2->CNT)/4 > 3){
      MenuPos = 3;
      TIM2->CNT = 0;
    }

    //GPIO 7 is the encoder switch, I should probably name my pins
    if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == GPIO_PIN_RESET){
      switch(MenuPos){
        case 0:
          Start();
          LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
          HAL_Delay(500);
          break;
        case 1:
          Push();
          LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
          HAL_Delay(500);
          break;
        case 2:
          Pull();
          LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
          HAL_Delay(500);
          break;
        case 3:
          CycleLimit();
          LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
          HAL_Delay(500);
          break;
      }
    }
  }

}

// stats the cycle test with whatever parameters you have set. Default of 10 second cyle with 10k cycle limit.
void Start(){
    HAL_Delay(500);
    int count = 0;
    char str[80];
    while(cyclelimit > 0){
              HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_FORWARD, GPIO_PIN_SET);
              LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
              LCD_Send_String_On_Line1("Forward");
              sprintf(str, "Cycles = %u", count);
              LCD_Send_String_On_Line2(str);
              HAL_Delay(push);
              HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_FORWARD, GPIO_PIN_RESET);
              HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_REVERSE, GPIO_PIN_SET);
              LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
              LCD_Send_String_On_Line1("Reverse");
              sprintf(str, "Cycles = %u", count);
              LCD_Send_String_On_Line2(str);
              HAL_Delay(pull);
              HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_REVERSE, GPIO_PIN_RESET);
              count++;
              cyclelimit--;
    }
}

// sets the delay for how long the motor extends. Default or 5 seconds
void Push(){
  HAL_Delay(500);
  TIM2->CNT=0;
  while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == GPIO_PIN_SET){
              push = ((TIM2->CNT)/4)*100;
              char str[80];
              LCD_Send_String_On_Line1("set push timer");
              sprintf(str, "timer = %lu ms", push);
              LCD_Send_String_On_Line2(str);
            }

}

// sets the delay for how long the motor retracts. default of 5.1 seconds (pull should be longer than push to account for gear slippage)
void Pull(){
  HAL_Delay(500);
  TIM2->CNT=0;
  while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == GPIO_PIN_SET){
              pull = ((TIM2->CNT)/4)*100;
              char str[80];
              LCD_Send_String_On_Line1("set pull timer");
              sprintf(str, "timer = %lu ms", pull);
              LCD_Send_String_On_Line2(str);
            }

}

//function for setting the maximum cycle count (default 10k, though I need to add a default state of infinite as my boss will 100% ask for this)
void CycleLimit(){
  HAL_Delay(500);
  TIM2->CNT=0;
  while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7) == GPIO_PIN_SET){
              cyclelimit = ((TIM2->CNT)/4);
              char str[80];
              LCD_Send_String_On_Line1("set cycle limit");
              sprintf(str, "cycles = %lu", cyclelimit);
              LCD_Send_String_On_Line2(str);
            }

}

//this function will cycle the motor once
//this was for testing and will be removed later
void CycleTest(){
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


// This function will test encoder switch and counter.
// this function was just for testing and will likely be removed.

void Encoder(){
  int16_t count;
  count=TIM2->CNT;
  char str [80];
  if (HAL_GPIO_ReadPin (GPIOD, GPIO_PIN_7) == GPIO_PIN_SET){
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

// Function will test direction you are turning the encoder and display it on LCD
// this function was just for testing and will likely be removed
void Direction(){
  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET)  // If the CH1 is RESET
	  {
		  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET)  // If CH2 is also reset... CCK
		  {
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET){};  // wait for the CH2 to go high
			  //LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
        //LCD_Send_String("Reverse");
			  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){};  // wait for the CH1 to go high
			  HAL_Delay (10);  // wait for some more time
		  }

		  else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET)  // If CH2 is also set
		  {
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET){};  // wait for the CH2 to go LOW.. CK
			  //LCD_Send_Cmd(LCD_CLEAR_DISPLAY);
        //LCD_Send_String("Forward");
			  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET){};  // wait for the CH1 to go high
			  while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET){};  // wait for the CH2 to go high
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

void E_STOP_Init(){
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = E_STOP_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(E_STOP_PORT, &GPIO_InitStruct);

}

/* function to read pin state for console */
int PinState(){
  return HAL_GPIO_ReadPin(MOTOR_PORT, MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE);
}

/* */
int timer(){
  return ((TIM2->CNT)>>2);
}
/* function to read cycle count for console */
int getCycleCount(){
  return cycles;
}

/* function to send test parameters to console */
int* getParameters(){
  
  //apparently it's important to dynamically allocate memory
  int *param = (int*) malloc(sizeof(int) * 3);

  *param = cyclelimit;
  *(param + 1) = push;
  *(param + 2) = pull;
   
  return (param) ;
}

void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(E_STOP_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t E_STOP)
{
  if(E_STOP == E_STOP_PIN){
    while(1){
      LCD_Send_String_On_Line1("ESTOP pressed");
      LCD_Send_String_On_Line2("reset board");
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_FORWARD, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_PIN_REVERSE, GPIO_PIN_RESET);
      
    }
  }
}

//Everything below here is auto gen stuff from cubeIDE
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage, generated from Cube
  */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure. generated from Cube
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

  /** Initializes the CPU, AHB and APB buses clocks, generated from Cube
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_NVIC_Init(void)
{
  /* EXTI0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}
