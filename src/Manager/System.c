#include "stm32f1xx.h"

void EXTI0_IRQHandler(void){


}

void SysTick_Handler(void) {
  HAL_IncTick(); // 每1ms调用一次，确保HAL_Delay正常计时
} 