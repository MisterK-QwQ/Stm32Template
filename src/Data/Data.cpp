#include "Data.hpp"

Logger LogF;

bool IWDG_Init(){
  Data.hiwdg.Instance = IWDG;  // 指定看门狗实例（独立看门狗只有IWDG）
  Data.hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  Data.hiwdg.Init.Reload = 2343;  //Reload + 1 = (15 × 40000) ÷ 256 = 600000 ÷ 256 = 2343.75
  
  if (HAL_IWDG_Init(&Data.hiwdg) != HAL_OK) {
    return false;
  }
  return true;
}

bool SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // 校准HSI
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // 不使用PLL，避免配置复杂
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    return false;
  }

   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; // 系统时钟=HSI（8MHz）
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB=8MHz（SysTick时钟源）
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // APB1=8MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2=8MHz（GPIOC挂在APB2）
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    return false;
  }
  return true;
}
