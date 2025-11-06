#pragma once
#include "../Events/Event.hpp"
#include "DigitalCircuit/GPIO.hpp"

class Manager{
public:
    GPIO gpio;
   // UART_HandleTypeDef huart1;
    std::unique_ptr<EmbeddedEvent::Dispatcher> mDispatcher;


    void init();
    void read();
    void read(GPIO_TypeDef port); 

    Manager(/* args */);
    ~Manager();
};
extern Manager* manager;
