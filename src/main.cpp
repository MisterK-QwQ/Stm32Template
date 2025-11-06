#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

uint8_t currentAngle = 0;    // 初始角度0°
uint8_t Angle = 10;
void OnOpenLedEvent(GpioEvent& event) {
    if(event.pin==GPIO_PIN_13){
        HAL_GPIO_TogglePin(event.Port,event.pin);
        HAL_Delay(100);

    }
}
int main(void) {
    HAL_Init();
    SystemClock_Config();
    //使能接口
   // __HAL_RCC_ADC1_CLK_ENABLE();  
   __HAL_RCC_GPIOC_CLK_ENABLE(); 
    // __HAL_RCC_I2C1_CLK_ENABLE();
   // __HAL_RCC_GPIOB_CLK_ENABLE();
   // __HAL_RCC_TIM3_CLK_ENABLE();  // 使能TIM3时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
#ifdef _Log
    USART1_UART_Init();  //logger USART1初始化
#endif
    manager->init();
    manager->mDispatcher->registerListener<GpioEvent>(OnOpenLedEvent); // 注册事件监听器
    LogF.logF(LogLevel::INFO,"Initialized");
    while (true) {
        manager->read();      
#ifdef _Dog
        HAL_IWDG_Refresh(&Data.hiwdg);  // 喂狗
#endif
    //   LogF.logF(LogLevel::INFO,"Tick");
    }
}
extern "C" void SysTick_Handler(void){   //每1msTick运行一次
  HAL_IncTick();  
}

extern "C" void EXTI0_IRQHandler(void){
  
}
