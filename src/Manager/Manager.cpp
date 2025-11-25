#include "Manager.hpp"
#include "Utils/Utils.hpp"
#include "Data/Data.hpp"

Manager::Manager(/* args */){
    //mDispatcher = EmbeddedEvent::Dispatcher(); 
}

Manager::~Manager(){
    if(gpio.clock[0].first){
        __HAL_RCC_GPIOA_CLK_DISABLE();
    }
     if(gpio.clock[1].first){
        __HAL_RCC_GPIOB_CLK_DISABLE();
    }
     if(gpio.clock[2].first){
        __HAL_RCC_GPIOC_CLK_DISABLE();
    }
}
void Manager::read() {
    gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->Gpio_initialized){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::read(GPIO_TypeDef port) {
     gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->Gpio_initialized&&data->port==port){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher.trigger(event);
        }
    });
}

void Manager::init() {
    gpio.Add(GPIOA,{GPIO_PIN_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SDA
    gpio.Add(GPIOA,{GPIO_PIN_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SCL
    gpio.Add(GPIOA,{GPIO_PIN_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //CS
    gpio.Add(GPIOA,{GPIO_PIN_3,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //RES
    gpio.Add(GPIOA,{GPIO_PIN_2,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //A0
    gpio.Add(GPIOB, {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});    //背光A
 

/*     gpio.Add(GPIOA,{GPIO_PIN_0,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //中断
    gpio.Add(GPIOA,{GPIO_PIN_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SCK ：时钟信号线
    gpio.Add(GPIOA,{GPIO_PIN_6,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //MISO输入
    gpio.Add(GPIOA,{GPIO_PIN_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //MOSI输出
    gpio.Add(GPIOA,{GPIO_PIN_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //CS数据
    gpio.Add(GPIOC,{GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW});  //复位 */
    
    {
        Data.Uart_c.huart1.Instance = USART1;
        Data.Uart_c.huart1.Init.BaudRate = 9600;
        Data.Uart_c.huart1.Init.WordLength = UART_WORDLENGTH_8B;
        Data.Uart_c.huart1.Init.StopBits = UART_STOPBITS_1;
        Data.Uart_c. huart1.Init.Parity = UART_PARITY_NONE;
        Data.Uart_c. huart1.Init.Mode = UART_MODE_TX_RX;
        Data.Uart_c. huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        Data.Uart_c. huart1.Init.OverSampling = UART_OVERSAMPLING_16;

        gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},&Data.Uart_c);   // USART1_TX  发送
        gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},&Data.Uart_c);    // USART1_RX  接受

    }

    gpio.InitAll();
    LogF.SetHuart(Data.Uart_c.huart1);
    initManager=true;
}
Manager manager = Manager();
