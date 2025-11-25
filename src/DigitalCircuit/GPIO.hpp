#pragma once
#include <memory>
#include <array>
#include <cstdint>
#include <functional>
#include "stm32f1xx_hal.h"
#include "GpioBase.hpp"
#include "PWMChannel.hpp"
#include "ADC.hpp"
#include "UARTChannel.hpp"
#include "DataChannel.hpp"
#include "DMAChannel.hpp"

enum class ClockSource {
    _GPIOA, _GPIOB, _GPIOC,
    _TIM1, _TIM2, _TIM3,
    _I2C1,
    _USART1, _USART2,
    _SPI1,
    _ADC1, _ADC2,
    _DMA1, _DMA2,
    _NONE,
    MAX
};


using GpioKey = uint64_t;

inline GpioKey make_key(GPIO_TypeDef* port, uint16_t pin) {
    return (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(port)) << 32) | pin;
}

inline void decompose_key(GpioKey key, GPIO_TypeDef*& port, uint16_t& pin) {
    uintptr_t port_ptr_val = static_cast<uintptr_t>(key >> 32);
    port = reinterpret_cast<GPIO_TypeDef*>(port_ptr_val);
    pin = static_cast<uint16_t>(key & 0xFFFF);
}

class GpioData {
public:
    bool Gpio_initialized = false;
    GPIO_TypeDef* port = nullptr;
    GPIO_InitTypeDef init_config = {0};
    GpioBase* base = nullptr;
};

class GPIO {
private:
    static constexpr size_t MAX_GPIO_PINS = 64;
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS> m_gpio_array;
    size_t m_count = 0;

public:

    std::array<std::pair<bool, size_t>, static_cast<size_t>(ClockSource::MAX)> clock{};

    GPIO() = default;
    ~GPIO() = default;
    GPIO(const GPIO&) = delete;
    GPIO& operator=(const GPIO&) = delete;

    size_t GetGpioSize() { return m_count; }
    std::array<std::unique_ptr<GpioData>, MAX_GPIO_PINS>& Get_gpio_array() { return m_gpio_array; }

    void Add(GPIO_TypeDef* port, const GPIO_InitTypeDef& init, GpioBase* hardware=nullptr) {
        if (port == nullptr || m_count >= MAX_GPIO_PINS) return;
        auto data = std::make_unique<GpioData>();
        data->port = port;
        data->init_config = init;
        data->base = hardware;
        m_gpio_array[m_count++] = std::move(data);
    }

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

    GPIO_PinState read(GPIO_TypeDef* port, uint16_t pin) {
        GpioData* data = GetData(port, pin);
        if (data && data->Gpio_initialized && data->port != nullptr) {
            return HAL_GPIO_ReadPin(data->port, pin);
        }
        return GPIO_PIN_RESET;
    }

    void ForEach(const std::function<void(GPIO_TypeDef*, uint16_t, GpioData*)>& callback) {
        for (size_t i = 0; i < m_count; ++i) {
            auto& data = m_gpio_array[i];
            if (data) {
                callback(data->port, data->init_config.Pin, data.get());
            }
        }
    }

    void ForEachInPort(GPIO_TypeDef* port, const std::function<void(uint16_t, GpioData*)>& callback) {
        if (port == nullptr) return;
        ForEach([&](GPIO_TypeDef* curr_port, uint16_t pin, GpioData* data) {
            if (curr_port == port) callback(pin, data);
        });
    }

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
     * @brief 初始化所有未初始化的GPIO引脚和关联的硬件外设
     */
    void InitAll() {
        ForEach([this](GPIO_TypeDef* port, uint16_t pin, GpioData* data) {
            if (!data->Gpio_initialized) {
                if (port == GPIOA) {
                    auto idx = (size_t)(ClockSource::_GPIOA);
                    if (!clock[(idx)].first) {
                        __HAL_RCC_GPIOA_CLK_ENABLE();
                        clock[idx].first = true;
                    }
                    clock[idx].second++;
                }
                if (port == GPIOB) {
                    auto idx = (size_t)(ClockSource::_GPIOB);
                    if (!clock[idx].first) {
                        __HAL_RCC_GPIOB_CLK_ENABLE();
                        clock[idx].first = true;
                    }
                    clock[idx].second++;
                }
                if (port == GPIOC) {
                    auto idx = (size_t)(ClockSource::_GPIOC);
                    if (!clock[idx].first) {
                        __HAL_RCC_GPIOC_CLK_ENABLE();
                        clock[idx].first = true;
                    }
                    clock[idx].second++;
                }
                
                if (data->base != nullptr) {
                    switch (data->base->get_type()) {
                        case HardwareType::PWM: {
                            PWMChannel* pwm_ptr = static_cast<PWMChannel*>(data->base);
                            if (pwm_ptr->htim.Instance == TIM1) {
                                auto idx = (size_t)(ClockSource::_TIM1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_TIM1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            if (pwm_ptr->htim.Instance == TIM2) {
                                auto idx = (size_t)(ClockSource::_TIM2);
                                if (!clock[idx].first) {
                                    __HAL_RCC_TIM2_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            if (pwm_ptr->htim.Instance == TIM3) {
                                auto idx = (size_t)(ClockSource::_TIM3);
                                if (!clock[idx].first) {
                                    __HAL_RCC_TIM3_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        case HardwareType::ADC: {
                            ADCChannel* adc_ptr = static_cast<ADCChannel*>(data->base);
                            if (adc_ptr->hadc.Instance == ADC1) {
                                auto idx = (size_t)(ClockSource::_ADC1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_ADC1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            if (adc_ptr->hadc.Instance == ADC2) {
                                auto idx = (size_t)(ClockSource::_ADC2);
                                if (!clock[idx].first) {
                                    __HAL_RCC_ADC2_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        case HardwareType::UART: {
                            UARTChannel* uart_ptr = static_cast<UARTChannel*>(data->base);
                            if (uart_ptr->huart1.Instance == USART1) {
                                auto idx = (size_t)(ClockSource::_USART1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_USART1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            if (uart_ptr->huart1.Instance == USART2) {
                                auto idx = (size_t)(ClockSource::_USART2);
                                if (!clock[idx].first) {
                                    __HAL_RCC_USART2_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        case HardwareType::SPI: {
                            SPIChannel* spi_ptr = static_cast<SPIChannel*>(data->base);
                            if (spi_ptr->hspi1.Instance == SPI1) {
                                auto idx = (size_t)(ClockSource::_SPI1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_SPI1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        case HardwareType::I2C: {
                            I2CChannel* i2c_ptr = static_cast<I2CChannel*>(data->base);
                            if (i2c_ptr->hi2c.Instance == I2C1) {
                                auto idx = (size_t)(ClockSource::_I2C1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_I2C1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        case HardwareType::DMA: {
                            DMAChannel* dma_ptr = static_cast<DMAChannel*>(data->base);
                            if (dma_ptr->hdma.Instance == DMA1_Channel1) {
                                auto idx = (size_t)(ClockSource::_DMA1);
                                if (!clock[idx].first) {
                                    __HAL_RCC_DMA1_CLK_ENABLE();
                                    clock[idx].first = true;
                                }
                                clock[idx].second++;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }

                HAL_GPIO_Init(data->port, &data->init_config);
                data->Gpio_initialized = true;
                 if (data->base != nullptr) {
                    if(!data->base->init()){
                        LogF.logF(LogLevel::DEBUG,"%dInitialization failed!",data->base->get_type());
                    };
                }
            }
        });
    }
};