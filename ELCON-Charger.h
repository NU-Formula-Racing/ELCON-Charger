#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"
#include "I-Charger.h"

class ElconCharger : public ICharger
{
public:
    ElconCharger(ICAN &can_interface) : can_interface_{can_interface} {}

    enum class FaultPositions : int
    {
        Hardware_Failure = 0,
        Temperature_of_Charger,
        Input_Voltage,
        Starting_state,
        Communication_state,

    };

    void Enable()
    {
        Control = 0;
    }
    void Disable()
    {
        Control = 1;
    }
    void SetVoltageCurrent(float voltage, float current)
    {
        Output_Voltage_High_Byte = voltage;
        Output_Current_High_Byte = current;
    }

    void Tick(uint32_t current_time)
    {
        CHG_Individual_Ctrl.GetTransmitTimer().Tick(current_time);
    }

    float GetOutputVoltage()
    {
        return Output_Voltage_High_Byte;
    }

    float GetOutputCurrent()
    {
        return Output_Current_High_Byte;
    }

    uint16_t GetFaults()
    {
        uint16_t faults = 0;
        faults |= bool(Hardware_Failure) << int(FaultPositions::Hardware_Failure);
        faults |= bool(Temperature_of_Charger) << int(FaultPositions::Temperature_of_Charger);
        faults |= bool(Input_Voltage) << int(FaultPositions::Input_Voltage);
        faults |= bool(Starting_state) << int(FaultPositions::Starting_state);
        faults |= bool(Communication_state) << int(FaultPositions::Communication_state);

        return faults;
    }

private:
    ICAN &can_interface_;

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Max_Allowable_Charging_Terminal_Voltage_High_Byte{};
    CANSignal<uint8_t, 8, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Max_Allowable_Charging_Terminal_Voltage_Low_Byte{};
    CANSignal<uint8_t, 16, 24, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Max_Allowable_Charging_Current_High_Byte{};
    CANSignal<uint8_t, 24, 32, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Max_Allowable_Charging_Current_Low_Byte{};
    CANSignal<bool, 32, 33, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Control{};
    CANTXMessage<5> Message1{can_interface, 0x1806E5F4, 8, 500, Max_Allowable_Charging_Terminal_Voltage_High_Byte, Max_Allowable_Charging_Terminal_Voltage_Low_Byte,
                             Max_Allowable_Charging_Current_High_Byte, Max_Allowable_Charging_Current_Low_Byte, Control};

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Output_Voltage_High_Byte{};
    CANSignal<uint8_t, 8, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Output_Voltage_Low_Byte{};
    CANSignal<uint8_t, 16, 24, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Output_Current_High_Byte{};
    CANSignal<uint8_t, 24, 32, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0)> Output_Current_Low_Byte{};
    CANRXMessage<4> Message2{can_interface, 0x18FF50E5, Output_Voltage_High_Byte, Output_Voltage_Low_Byte, Output_Current_High_Byte,
                             Output_Current_Low_Byte};

    CANSignal<bool, 0, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Hardware_Failure{};
    CANSignal<bool, 1, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Temperature_of_Charger{};
    CANSignal<bool, 2, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Input_Voltage{};
    CANSignal<bool, 3, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Starting_state{};
    CANSignal<bool, 4, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> Communication_state{};
    CANRXMessage<4> Status{can_interface, 0x18FF50E5, Hardware_Failure, Temperature_of_Charger, Input_Voltage,
                           Starting_state, Communication_state};
};