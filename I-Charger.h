#pragma once
#include <array>
#include <vector>

class ICharger
{
public:
    virtual void Enable() = 0;

    virtual void Disable() = 0;

    virtual void SetVoltageCurrentAndPower(float voltage, float current, float power) = 0;

    virtual void Tick(uint32_t current_time) = 0;

    virtual float GetOutputVoltage() = 0;

    virtual float GetOutputCurrent() = 0;

    virtual uint16_t GetFaults() = 0;
};
