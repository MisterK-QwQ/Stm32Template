#pragma once
#include "stm32f1xx_hal.h"
#include "DataChannel.hpp"

class OLED {
private:
    I2CChannel& i2c;       // I2C通道引用
    uint8_t dev_addr;      // OLED I2C地址（通常为0x3C或0x3D，GME12864-49多为0x3C）

    void writeCommand(uint8_t cmd) {
        uint8_t data[2] = {0x00, cmd};  // 0x00=命令标志
        i2c.I2C_Write(dev_addr, 0x00, data, 2);  // 寄存器地址占位（OLED忽略）
    }

    void writeData(uint8_t data) {
        uint8_t buf[2] = {0x40, data};  // 0x40=数据标志
        i2c.I2C_Write(dev_addr, 0x00, buf, 2);
    }

public:
    OLED(I2CChannel& i2c_channel, uint8_t addr = 0x3C) 
        : i2c(i2c_channel), dev_addr(addr) {}

    void init() {
        HAL_Delay(100);  // 上电延时

        writeCommand(0xAE);  // 关闭显示
        writeCommand(0x00);  // 设置列地址低4位
        writeCommand(0x10);  // 设置列地址高4位
        writeCommand(0x40);  // 设置起始行
        writeCommand(0xB0);  // 设置页地址（0-7）
        writeCommand(0x81);  // 对比度设置
        writeCommand(0xFF);  // 最大对比度
        writeCommand(0xA1);  // 左右反置（根据屏幕方向调整）
        writeCommand(0xA6);  // 正常显示（0xA7为反显）
        writeCommand(0xA8);  // 设置多路复用率
        writeCommand(0x3F);  // 64行
        writeCommand(0xA4);  // 显示全部点亮关闭
        writeCommand(0xD3);  // 设置显示偏移
        writeCommand(0x00);  // 无偏移
        writeCommand(0xD5);  // 设置振荡频率
        writeCommand(0x80);  // 默认值
        writeCommand(0xD9);  // 设置预充电周期
        writeCommand(0xF1);  // 
        writeCommand(0xDA);  // 设置COM引脚配置
        writeCommand(0x12);  // 
        writeCommand(0xDB);  // 设置VCOMH
        writeCommand(0x40);  // 
        writeCommand(0xAF);  // 开启显示
    }

    void clear() {
        for (uint8_t page = 0; page < 8; page++) {
            writeCommand(0xB0 + page);  // 页地址
            writeCommand(0x00);         // 列地址低4位
            writeCommand(0x10);         // 列地址高4位
            for (uint8_t col = 0; col < 128; col++) {
                writeData(0x00);        // 写入0清空
            }
        }
    }

    void showChar(uint8_t x, uint8_t y, char c) {

        // 实际应用需添加字库，此处仅示意逻辑
        uint8_t i, j;
        c -= ' ';  // 计算偏移（假设字库从空格开始）
        writeCommand(0xB0 + y);                  // 页地址（y=0~7）
        writeCommand(0x00 + (x & 0x0F));         // 列地址低4位
        writeCommand(0x10 + ((x >> 4) & 0x0F));  // 列地址高4位
        for (i = 0; i < 8; i++) {
            writeData(0x00);  // 替换为实际字库数据font8x16[c*16 + i]
        }
        writeCommand(0xB0 + y + 1);
        writeCommand(0x00 + (x & 0x0F));
        writeCommand(0x10 + ((x >> 4) & 0x0F));
        for (i = 0; i < 8; i++) {
            writeData(0x00);  // 替换为实际字库数据font8x16[c*16 + i + 8]
        }
    }
};