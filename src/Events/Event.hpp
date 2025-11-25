#pragma once
#include "../include/Events.hpp"
#include "stm32f1xx_hal.h"
#include "DigitalCircuit/GPIO.hpp"

struct GpioEvent:EmbeddedEvent::Event{
    uint16_t pin;
    GPIO_TypeDef* Port;
    GPIO_PinState state;    
    GpioData* Data;
    GpioEvent(uint16_t p, GPIO_TypeDef* pt, GPIO_PinState s,GpioData* data)
        : pin(p), Port(pt),state(s), Data(data){}
};


struct SpiEvent:EmbeddedEvent::Event{
    SPI_HandleTypeDef hspi;  //SPI句柄
    HAL_StatusTypeDef status; // 传输状态
    uint8_t* tx_data;         // 发送数据
    uint8_t* rx_data;         // 接收数据
    size_t len;               // 数据长度
    GpioData* Data;

    SpiEvent(SPI_HandleTypeDef h, HAL_StatusTypeDef s,GpioData* data, uint8_t* tx, uint8_t* rx, size_t l)
        : hspi(h), status(s),Data(data),tx_data(tx), rx_data(rx), len(l) {}
};