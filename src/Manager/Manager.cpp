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



/*     gpio.Add(GPIOA,{GPIO_PIN_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SDA
    gpio.Add(GPIOA,{GPIO_PIN_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SCL
    gpio.Add(GPIOA,{GPIO_PIN_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //CS
    gpio.Add(GPIOA,{GPIO_PIN_3,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW});  //RES
    gpio.Add(GPIOA,{GPIO_PIN_2,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //A0
    gpio.Add(GPIOB, {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});    //背光A
  */

/*     gpio.Add(GPIOA,{GPIO_PIN_0,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //中断
    gpio.Add(GPIOA,{GPIO_PIN_5,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //SCK ：时钟信号线
    gpio.Add(GPIOA,{GPIO_PIN_6,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //MISO输入
    gpio.Add(GPIOA,{GPIO_PIN_7,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //MOSI输出
    gpio.Add(GPIOA,{GPIO_PIN_4,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH});  //CS数据
    gpio.Add(GPIOC,{GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW});  //复位 */
    
/*     SPIChannel Spi;
    Spi.hspi1.Instance = SPI1;
    Spi.hspi1.Init.Mode = SPI_MODE_MASTER;
    Spi.hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    Spi.hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    Spi.hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    Spi.hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    Spi.hspi1.Init.NSS = SPI_NSS_SOFT;  // 软件片选
    Spi.hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;  // 4MHz
    Spi.hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    Spi.hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    Spi.hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    gpio.Add(GPIOA, 
        {GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}, 
        &Spi
    );
    gpio.Add(GPIOA,
        {GPIO_PIN_6, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},
        &Spi
    );
    gpio.Add(GPIOA,
        {GPIO_PIN_7, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},
        &Spi
    );
 */
    UARTChannel Uart_c;
    Uart_c.huart1.Instance = USART1;
    Uart_c.huart1.Init.BaudRate = 9600;
    Uart_c.huart1.Init.WordLength = UART_WORDLENGTH_8B;
    Uart_c.huart1.Init.StopBits = UART_STOPBITS_1;
    Uart_c. huart1.Init.Parity = UART_PARITY_NONE;
    Uart_c. huart1.Init.Mode = UART_MODE_TX_RX;
    Uart_c. huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    Uart_c. huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},&Uart_c);   // USART1_TX  发送
    gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH},&Uart_c);    // USART1_RX  接受
    
/*     gpio.Add(GPIOC, {GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW});
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
 */
    gpio.InitAll();
    LogF.SetHuart(&Uart_c.huart1);
    initManager=true;
}
Manager manager = Manager();
