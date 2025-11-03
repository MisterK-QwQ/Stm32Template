#pragma once
#include "stm32f1xx_hal.h"

class Flash{
public:
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    Flash(FLASH_EraseInitTypeDef eraseInitstruct) : EraseInitStruct(eraseInitstruct) {};

    Flash()=default;
    Flash(const Flash&) = delete;
    Flash& operator=(const Flash&) = delete;
    /**
     * @brief 写入多个字到Flash
     * @param StartAddress 起始地址（必须是4字节对齐）
     * @param pData 指向数据缓冲区的指针
     * @param Length 写入的字数
     * @return bool 写入是否成功
     * @details 解锁Flash→逐字写入→锁定Flash
     */
    bool FlashWriteMulti(uint32_t StartAddress, uint32_t* pData, uint32_t Length) {
        HAL_FLASH_Unlock();
        bool success = true;
        for (uint32_t i = 0; i < Length; i++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartAddress + i * 4, pData[i]) != HAL_OK) {
                success = false;
                break;
            }
        }
        HAL_FLASH_Lock();
        return success;
    }
    /**
     * @brief 读取多个字从Flash
     * @param StartAddress 起始地址（必须是4字节对齐）
     * @param pData 指向数据缓冲区的指针
     * @param Length 读取的字数
     * @details 逐字读取Flash数据到缓冲区
     */
    void FlashReadMulti(uint32_t StartAddress, uint32_t* pData, uint32_t Length) {
        for (uint32_t i = 0; i < Length; i++) {
            pData[i] = *(__IO uint32_t*)(StartAddress + i * 4);
        }
    }
    /**
     * @brief 擦除Flash页面
     * @details 解锁Flash→擦除指定页面→锁定Flash
     */
    void FlashErase() {
        HAL_FLASH_Unlock();
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        }
        HAL_FLASH_Lock();
    }
};