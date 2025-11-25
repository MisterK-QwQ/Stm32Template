#include "Manager/Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"
const uint8_t TEST_DATA[] = {0xAA, 0x55, 0x33, 0xCC, 0x11, 0xEE};
const uint16_t DATA_LEN = sizeof(TEST_DATA) / sizeof(TEST_DATA[0]);
TimerUtil Time1;
void onTestTrigger(GpioEvent& event) {
    std::array<uint8_t, DATA_LEN> rxBuffer = {0};
    if(!Time1.hasTimedElapsed(500,true)){
        return;
    }
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(
        &Data.hspi1,
        const_cast<uint8_t*>(TEST_DATA),
        rxBuffer.data(),
        DATA_LEN,
        500
    );
    if (status == HAL_OK) {
        LogF.logF(LogLevel::INFO, "SPIOK");
        LogF.logF(LogLevel::INFO, "TX: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
            TEST_DATA[0], TEST_DATA[1], TEST_DATA[2],
            TEST_DATA[3], TEST_DATA[4], TEST_DATA[5]);
        LogF.logF(LogLevel::INFO, "RX: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
            rxBuffer[0], rxBuffer[1], rxBuffer[2],
            rxBuffer[3], rxBuffer[4], rxBuffer[5]);

        bool loopbackOk = true;
        for (uint16_t i = 0; i < DATA_LEN; i++) {
            if (rxBuffer[i] != TEST_DATA[i]) {
                loopbackOk = false;
                break;
            }
        }
        if (loopbackOk) {
            LogF.logF(LogLevel::INFO, "OK");
        } else {
            LogF.logF(LogLevel::ERROR, "Data Not Data");
        }
    } else {
        LogF.logF(LogLevel::ERROR, "SPI Not code: %d", status);
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
   /*  if (HAL_SPI_Init(&Data.hspi1) != HAL_OK) {
        LogF.logF(LogLevel::ERROR, "SPI1 初始化失败");
    } */
    LogF.logF(LogLevel::INFO,"Initialized");
   // manager.LDC.init();

    manager.mDispatcher.registerListener<GpioEvent>(onTestTrigger);

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