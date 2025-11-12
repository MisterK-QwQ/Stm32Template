#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"
#include "DigitalCircuit/OLED.hpp"


int LED_PWM = 0; 
void OnOpenLedEvent(GpioEvent& event) {
    if (event.pin == GPIO_PIN_14 && event.Port == GPIOC) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 
                        (event.state == GPIO_PIN_RESET) ?
                        GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    LED_PWM+=10;
    if(LED_PWM>=19999) LED_PWM=0;
    if(event.pin == GPIO_PIN_0 && event.Port == GPIOA) {
        HAL_Delay(20);
        event.Data->hardware_info.pwm_channel.SetDuty(LED_PWM);
    }
}

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
    manager.mDispatcher.registerListener<GpioEvent>(OnOpenLedEvent); // 注册事件监听器
    LogF.logF(LogLevel::INFO,"Initialized");
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

  }
}

/* extern "C" void EXTI0_IRQHandler(void){
  
}
 */