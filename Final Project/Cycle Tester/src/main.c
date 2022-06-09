#include "main.h"
#include "console.h"
#include "STM32F407_I2C_LCD16x02_Driver.h"

/* Pin Definitions */
#define MOTOR_PIN_FORWARD GPIO_PIN_5
#define MOTOR_PIN_REVERSE GPIO_PIN_4
#define MOTOR_PORT GPIOD

#define USER_BUTTON_PORT GPIOA
#define USER_BUTTON_PIN GPIO_PIN_0

/* clocks */
#define MOTOR_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define USER_BUTTON_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

void MOTOR_Init();
void UserButton_Init();
void EXTI0_Callback(uint16_t GPIO_Pin);
void SystemClock_Config(void);
void Error_Handler(void);
void LCD_Init();
void CycleTest();

uint32_t cycles = 0;

int main(void) {

  HAL_Init();
  MOTOR_Init();
  UserButton_Init();
  LCD_Init();


  // Setup Command Line Console
  ConsoleInit();


  while(1) {
    ConsoleProcess();
    CycleTest();
  }

}

void CycleTest(){
  if (HAL_GPIO_ReadPin(USER_BUTTON_PORT, USER_BUTTON_PIN)){
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

/* configure the motor GPIOs */
void MOTOR_Init(){
    MOTOR_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStruct);
}

/* configure the user button */
void UserButton_Init(){
  USER_BUTTON_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = USER_BUTTON_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStruct);
}

/* void SysTick_Handler(void)
{
  HAL_IncTick();
}
*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == USER_BUTTON_PIN){
    HAL_GPIO_TogglePin(MOTOR_PORT, MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE);
  }
}


/* function to read pin state for console */
int PinState(){
  return HAL_GPIO_ReadPin(MOTOR_PORT, MOTOR_PIN_FORWARD | MOTOR_PIN_REVERSE);
}

/* function to read cycle count for console */
int getCycleCount(){
  return cycles;
}

