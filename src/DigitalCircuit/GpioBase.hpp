#pragma once
enum class HardwareType {
    NONE,
    PWM,
    ADC,
    UART,
    SPI,
    I2C,
    DMA
};

class GpioBase {
private:
    bool isInitialized;
    HardwareType type;

protected:
    GpioBase(HardwareType t) : isInitialized(false), type(t) {}

public:
    virtual ~GpioBase() {}
    virtual bool init() = 0;
    HardwareType get_type() const {
        return type;
    }
};