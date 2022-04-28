#include "main.h"

/*LED Pin Definitions */
#define GREEN_LED_PIN GPIO_PIN_13
#define RED_LED_PIN GPIO_PIN_14
#define LED_GPIO_PORT GPIOG

/*Button Pin Definitions */
#define USER_BUTTON_PORT GPIOA
#define USER_BUTTON_PIN GPIO_PIN_0

/* clocks */
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define USER_BUTTON_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

int main(void)
{
    HAL_Init();
    LED_GPIO_CLK_ENABLE();
    USER_BUTTON_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GREEN_LED_PIN | RED_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USER_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(USER_BUTTON_PORT, &GPIO_InitStruct);

    //HAL_GPIO_TogglePin(LED_GPIO_PORT, GREEN_LED_PIN);

    while (1)
    {
            if (HAL_GPIO_ReadPin(GPIOA, USER_BUTTON_PIN))
            {
               HAL_GPIO_TogglePin(LED_GPIO_PORT, GREEN_LED_PIN | RED_LED_PIN);
            }
            /*HAL_GPIO_TogglePin(LED_GPIO_PORT, GREEN_LED_PIN);
            HAL_GPIO_TogglePin(LED_GPIO_PORT, RED_LED_PIN); */
    }

}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
