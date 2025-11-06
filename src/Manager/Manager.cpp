#include "Manager.hpp"
Manager* manager =new Manager();

Manager::Manager(/* args */){
    mDispatcher = std::make_unique<EmbeddedEvent::Dispatcher>(); 
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
    delete manager;
}
void Manager::read() {
    gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->initialized){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher->trigger(event);
        }
    });
}

void Manager::read(GPIO_TypeDef port) {
     gpio.ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
        if(data->initialized&&data->port==port){
            GPIO_PinState state = gpio.read(port, pin);
            GpioEvent event(pin, port, state, data);
            mDispatcher->trigger(event);
        }
    });
}

void Manager::init() {
   /*  gpio.Add(GPIOA, 
         {GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}, 
         Hardware( 
             PWMChannel(
                 TIM_HandleTypeDef{
                     .Instance=TIM2,
                     {
                         .Prescaler=71, 
                         .CounterMode=TIM_COUNTERMODE_UP, 
                         .Period=100,
                         .ClockDivision=TIM_CLOCKDIVISION_DIV1,
                         .RepetitionCounter= 0,
                         .AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_DISABLE
                     },
                     .Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED,
                     .hdma={nullptr},
                     .Lock=HAL_UNLOCKED,
                     .State=HAL_TIM_STATE_RESET
                 },
                 TIM_OC_InitTypeDef{
                     .OCMode=TIM_OCMODE_PWM1,
                     .Pulse=0,
                     .OCPolarity=TIM_OCPOLARITY_HIGH,
                     .OCNPolarity=0, 
                     .OCFastMode=TIM_OCFAST_DISABLE,
                 },
                 TIM_CHANNEL_1
             ))); */
    

    gpio.Add(GPIOC,{GPIO_PIN_13,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_LOW});

   // gpio.Add(GPIOA, {GPIO_PIN_0, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // 按键输入
    gpio.Add(GPIOA, {GPIO_PIN_9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_RX
    gpio.Add(GPIOA, {GPIO_PIN_10, GPIO_MODE_AF_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH});  // USART1_TX
    gpio.InitAll();
    initManager=true;
}
