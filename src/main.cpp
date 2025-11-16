#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

int main(void) {
    HAL_Init();
    SystemClock_Config();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
#ifdef _Log
    USART1_UART_Init();  //logger USART1初始化
#endif
    manager.init();
    
    LogF.logF(LogLevel::INFO,"Initialized");
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    manager.LDC.init();
    manager.LDC.drawRect(10, 10, 50, 30, 1, true);    // 小填充矩形（左上角）
    manager.LDC.drawRect(77, 10, 117, 30, 1, false);  // 小描边矩形（右上角）
    manager.LDC.drawCircle(64, 48, 12, 1);           // 小圆形（下方居中）
    
    LogF.logF(LogLevel::INFO,"Gpio Size:%d GPIOA:%d GPIOB:%d GPIOC:%d"
        ,manager.gpio.GetGpioSize()
        ,manager.gpio.clock[0].second
        ,manager.gpio.clock[1].second
        ,manager.gpio.clock[2].second
    );


    while (true) {
        manager.read();      
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif
     //  LogF.logF(LogLevel::INFO,"Tick");
    }
}
extern "C" void SysTick_Handler(void){   //每1msTick运行一次
  HAL_IncTick();  
  if(manager.initManager){
    manager.tick=HAL_GetTick();
  }
}

/* extern "C" void EXTI0_IRQHandler(void){
  
}
 */