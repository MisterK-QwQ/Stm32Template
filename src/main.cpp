#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

MathTools::ServoControl servo(0.0f, 180.0f, 500, 2500, 50);
int current_angle = 0;  // 初始角度90°（对应1500us脉冲）
TimerUtil button_timer;  // 按键防抖定时器


void ServoControlHandler(GpioEvent& event) {
    if (event.Port != GPIOC || event.pin != GPIO_PIN_13) {
        return;
    }

    if (!button_timer.hasTimedElapsed(200, true)) {
        return;
    }

    if (event.state == GPIO_PIN_RESET) {
        current_angle += 10;
        if (current_angle > 180) {
            current_angle = 0;
        }
        LogF.logF(LogLevel::INFO, "按键按下，当前角度: %d°", current_angle);
        uint16_t pulse = servo.angleToPulse(current_angle);
        auto [pwm_port, pwm_pin, pwm_data] = manager.gpio.FindIf([](GPIO_TypeDef* p, uint16_t pi, GpioData* d) {
            return p == GPIOA && pi == GPIO_PIN_0;  // 匹配PA0的PWM引脚
        });
        if (pwm_data && pwm_data->base) {
            static_cast<PWMChannel*>(pwm_data->base)->SetDuty(pulse);
            LogF.logF(LogLevel::INFO, "设置脉冲: %dμs (对应角度: %d°)", pulse, current_angle);
        }
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
#ifdef _Dog
    IWDG_Init(); // 启动定时器中断  看门狗
#endif
    manager.init();

    manager.mDispatcher.registerListener<GpioEvent>(ServoControlHandler);

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