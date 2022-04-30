#include "main.h"
#include "console.h"

int main(void) {

  HAL_Init();

  // Setup Command Line Console
  ConsoleInit();


  while(1) {
    ConsoleProcess();
  }

}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
