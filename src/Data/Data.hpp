#pragma once
#include "stm32f1xx_hal.h"
#include "Utils/Logger.hpp"
#include "../DigitalCircuit/DataChannel.hpp"
#include "../DigitalCircuit/UARTChannel.hpp"
extern Logger LogF;

struct DATA{
  IWDG_HandleTypeDef hiwdg;
  SPIChannel Spi;
  UARTChannel Uart_c;
};
inline DATA Data;

bool IWDG_Init();
bool SystemClock_Config(void);