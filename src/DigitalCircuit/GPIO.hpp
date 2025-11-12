#pragma once
#include <memory>
#include <unordered_map>
#include <cstdint>
#include <functional>  
#include "stm32f1xx_hal.h"

#include "DataChannel.hpp"
#include "PWMChannel.hpp"
#include "ADC.hpp"
#include "UARTChannel.hpp"
#include "DMAChannel.hpp"

using GpioKey = uint64_t;

inline GpioKey make_key(GPIO_TypeDef* port, uint16_t pin) {
    return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(port)) << 32) | pin;
}

inline void decompose_key(GpioKey key, GPIO_TypeDef*& port, uint16_t& pin) {
    uintptr_t port_ptr_val = static_cast<uintptr_t>(key >> 32);  // 高32位是端口指针
    port = reinterpret_cast<GPIO_TypeDef*>(port_ptr_val);
    pin = static_cast<uint16_t>(key & 0xFFFF);  // 低32位的低16位是引脚号
}

class Hardware {
public:
    PWMChannel pwm_channel;
    SPIChannel spi_channel;
    I2CChannel i2c_channel;
    DMAChannel dma_channel;
    ADCChannel adc_channel;
    UARTChannel uart_channel;
    
    Hardware() = default;

    Hardware(UARTChannel uart){
        uart_channel = uart;
    }
     Hardware(PWMChannel pwm){
        pwm_channel = pwm;
    }
    Hardware(SPIChannel spi) {
        spi_channel = spi;
    }
    Hardware(I2CChannel i2c){
        i2c_channel = i2c;
    }
    Hardware(DMAChannel dma){
        dma_channel = dma;
    }
    Hardware(ADCChannel adc){
        adc_channel = adc;
    }
};

class GpioData {
public:
    bool Data_initialized;
    bool Gpio_initialized;               // 初始化状态标记
    GPIO_TypeDef* port = nullptr;           // GPIO端口指针
    GPIO_InitTypeDef init_config;           // GPIO初始化配置
    Hardware hardware_info;                 // 硬件相关信息
};

class GPIO {
private:
    static constexpr size_t MAX_GPIO_PINS = 64;  // 最大支持64个引脚
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS> m_gpio_array;
    size_t m_count = 0;  // 当前已添加的引脚数量
public:

    std::array<std::pair<bool, size_t>, 16> clock{};
    /**
     * [0] GPIOA
     * [1] GPIOB
     * [2] GPIOC
     * [3] TIM1
     * [4] TIM2
     * [5] TIM3
     * [6] I2C1
     */


    GPIO() = default;
    ~GPIO() = default;

    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;

    size_t GetGpioSize(){
        return m_count;
    }
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS>& Get_gpio_array(){
        return m_gpio_array;
    }
    /**
     * @brief 添加GPIO引脚配置
     */
      void Add(GPIO_TypeDef* port, const GPIO_InitTypeDef& init, 
             const Hardware& hardware = Hardware()) {
        if (port == nullptr || m_count >= MAX_GPIO_PINS) return;  // 检查容量
        
        auto data = std::make_unique<GpioData>();
        data->port = port;
        data->init_config = init;
        data->hardware_info = hardware;
        data->Gpio_initialized = false;
        data->Data_initialized = false;
        m_gpio_array[m_count++] = std::move(data);  // 存入数组
    }

    /**
     * @brief 获取指定引脚的配置数据
     * @return 找到返回GpioData指针，否则返回nullptr
     */
     GpioData* GetData(GPIO_TypeDef* port, uint16_t pin) {
        if (port == nullptr || pin == 0) return nullptr;
        for (size_t i = 0; i < m_count; ++i) {
            auto& data = m_gpio_array[i];
            if (data && data->port == port && data->init_config.Pin == pin) {
                return data.get();
            }
        }
        return nullptr;
    }

    /**
     * @brief 读取指定引脚电平
     */
    GPIO_PinState read(GPIO_TypeDef* port, uint16_t pin) {
        GpioData* data = GetData(port, pin);
        if (data && data->Gpio_initialized && data->port != nullptr) {
            return HAL_GPIO_ReadPin(data->port, pin);
        }
        return GPIO_PIN_RESET;
    }
    
    /**
     * @brief 遍历所有已添加的GPIO引脚
     * @param callback 回调函数，参数为：端口、引脚号、GpioData指针
     */
    void ForEach(const std::function<void(GPIO_TypeDef*, uint16_t, GpioData*)>& callback) {
        for (size_t i = 0; i < m_count; ++i) {
            auto& data = m_gpio_array[i];
            if (data) {
                callback(data->port, data->init_config.Pin, data.get());
            }
        }
    }

    /**
     * @brief 遍历指定端口的所有引脚
     * @param port 目标端口（如GPIOA）
     * @param callback 回调函数，参数为：引脚号、GpioData指针
     */
    void ForEachInPort(GPIO_TypeDef* port, const std::function<void(uint16_t, GpioData*)>& callback) {
        if (port == nullptr) return;
        ForEach([&](GPIO_TypeDef* curr_port, uint16_t pin, GpioData* data) {
            if (curr_port == port) {
                callback(pin, data);
            }
        });
    }

    /**
     * @brief 按条件查找GPIO引脚
     * @param condition 条件函数（返回true表示匹配）
     * @return 第一个匹配的引脚数据（port, pin, data），无匹配则返回空
     */
    std::tuple<GPIO_TypeDef*, uint16_t, GpioData*> FindIf(
        const std::function<bool(GPIO_TypeDef*, uint16_t, GpioData*)>& condition
    ) {
        for (size_t i = 0; i < m_count; ++i) {
            const auto& data_ptr = m_gpio_array[i];
            if (!data_ptr) continue;
            GPIO_TypeDef* port = data_ptr->port;
            uint16_t pin = data_ptr->init_config.Pin;
            if (condition(port, pin, data_ptr.get())) {
                return {port, pin, data_ptr.get()};
            }
        }
        return {nullptr, 0, nullptr};
    }

    /**
     * @brief 初始化所有未初始化的GPIO引脚
     */
    void InitAll() {
        ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
            if (!data->Gpio_initialized) {
                if(data->port==GPIOA){
                    if(!clock[0].first){
                        __HAL_RCC_GPIOA_CLK_ENABLE();
                        clock[0].first=true;
                    }
                    ++clock[0].second;
                }
                
                if(data->port==GPIOB){
                      if(!clock[1].first){
                        __HAL_RCC_GPIOB_CLK_ENABLE();
                        clock[1].first=true;
                    }
                    ++clock[1].second;
                }
                
                if(data->port==GPIOC){
                    if(!clock[2].first){
                        __HAL_RCC_GPIOC_CLK_ENABLE();
                        clock[2].first=true;
                    }
                    ++clock[2].second;
                }

                //Hardware init.
                if(data->hardware_info.pwm_channel.htim.Instance==TIM1){
                    if(!clock[3].first){
                        __HAL_RCC_TIM1_CLK_ENABLE();
                        clock[3].first=true;
                    }
                    ++clock[3].second;
                }
                if(data->hardware_info.pwm_channel.htim.Instance==TIM2){
                    if(!clock[4].first){
                        __HAL_RCC_TIM2_CLK_ENABLE();
                        clock[4].first=true;
                    }
                    ++clock[4].second;
                }

                if(data->hardware_info.pwm_channel.htim.Instance==TIM3){
                    if(!clock[5].first){
                        __HAL_RCC_TIM3_CLK_ENABLE();
                        clock[5].first=true;
                    }
                    ++clock[5].second;
                }
                if (data->hardware_info.i2c_channel.hi2c.Instance == I2C1) {
                    if (!clock[6].first) {
                        __HAL_RCC_I2C1_CLK_ENABLE();
                        clock[6].first = true;
                    }
                    ++clock[6].second;
                }
                if(data->hardware_info.uart_channel.huart1.Instance==USART1){
                    if (!clock[7].first) {
                        __HAL_RCC_USART1_CLK_ENABLE();
                        clock[7].first = true;
                    }
                    ++clock[7].second;

                }
                 if(data->hardware_info.uart_channel.huart1.Instance==USART2){
                    if (!clock[8].first) {
                        __HAL_RCC_USART2_CLK_ENABLE();
                        clock[8].first = true;
                    }
                    ++clock[8].second;
                }
                 if(data->hardware_info.spi_channel.hspi1.Instance==SPI1){
                    if (!clock[9].first) {
                        __HAL_RCC_SPI1_CLK_ENABLE();
                        clock[9].first = true;
                    }
                    ++clock[9].second;
                }
                if(data->hardware_info.adc_channel.hadc.Instance==ADC1){
                    if (!clock[10].first) {
                        __HAL_RCC_ADC1_CLK_ENABLE();
                        clock[10].first = true;
                    }
                    ++clock[10].second;
                }
                if(data->hardware_info.adc_channel.hadc.Instance==ADC2){
                    if (!clock[11].first) {
                        __HAL_RCC_ADC2_CLK_ENABLE();
                        clock[11].first = true;
                    }
                    ++clock[11].second;
                }

                HAL_GPIO_Init(data->port, &data->init_config);

                data->Gpio_initialized = true;
            }
        });
    }
};