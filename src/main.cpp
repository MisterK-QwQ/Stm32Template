#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"
int main(void) {
    HAL_Init();
    SystemClock_Config();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
    manager.init();

    LogF.logF(LogLevel::INFO,"Initialized");
    LogF.logF(LogLevel::INFO,"Gpio Size:%d GPIOA:%d GPIOB:%d GPIOC:%d"
        ,manager.gpio.GetGpioSize()
        ,manager.gpio.clock[0].second
        ,manager.gpio.clock[1].second
        ,manager.gpio.clock[2].second
    );
    manager.LDC.init();
    manager.LDC.drawLine(0,0,127,63);

    while (true) {
        manager.read();      
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif

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