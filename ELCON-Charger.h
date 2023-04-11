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

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0.1, 0) Max_Allowable_Charging_Terminal_Voltage_High_Byte{};
    <uint16_t, 0, 16, 0.1, 0> Max_Allowable_Charging_Terminal_Voltage_High_Byte{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0.1, 0) Max_Allowable_Charging_Current_High_Byte{};
    MakeUnsignedCANSignal(bool, 32, 1, 1, 0) Control{};
    CANTXMessage<3> Message1{can_interface, 0x1806E5F4, 8, 500, Max_Allowable_Charging_Terminal_Voltage_High_Byte, Max_Allowable_Charging_Terminal_Voltage_Low_Byte,
                             Max_Allowable_Charging_Current_High_Byte, Max_Allowable_Charging_Current_Low_Byte, Control};

    MakeUnsignedCANSignal(uint16_t, 0, 16, 0, 0) Output_Voltage_High_Byte{};
    MakeUnsignedCANSignal(uint16_t, 16, 16, 0, 0) Output_Voltage_High_Byte{};
    MakeUnsignedCANSignal(uint8_t, 32, 8, 0, 0) Status_Flags{};

    CANRXMessage<3> Message2{can_interface, 0x18FF50E5, Output_Voltage_High_Byte, Output_Current_High_Byte,
                             Status_Flags};
};