#pragma once
#include "stm32f1xx_hal.h"
#include "Utils/Logger.hpp"
extern Logger LogF;

struct DATA{
  UART_HandleTypeDef huart1;
  IWDG_HandleTypeDef hiwdg;
};
inline DATA Data;

bool USART1_UART_Init(void);
bool IWDG_Init();
bool SystemClock_Config(void);