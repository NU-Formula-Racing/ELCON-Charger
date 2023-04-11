#pragma once
#include <array>
#include "can_interface.h"
#include "virtualTimer.h"
#include "I-Charger.h"

class ElconCharger : public ICharger
{
public:
    ElconCharger(ICAN &can_interface) : can_interface_{can_interface} {}

    union Status
    {
        uint8_t all_flags = 0;
        struct flags
        {
            bool HardwareFailure : 1;
            bool OverTemperature : 1;
            bool InputVoltageFault : 1;
            bool StartingState : 1;
            bool CommunicationTimeout : 1;
            bool res1 : 1;
            bool res2 : 1;
            bool res3 : 1;
        } flags;
    };

    uint16_t GetFaults()
    {
        uint16_t faults = 0;
        faults |= bool(HardwareFailure) << int(FaultPositions::HardwareFailure);
        faults |= bool(OverTemperature) << int(FaultPositions::OverTemperature);
        faults |= bool(InputVoltageFault) << int(FaultPositions::InputVoltageFault);
        faults |= bool(StartingState) << int(FaultPositions::StartingState);
        faults |= bool(CommunicationTimeout) << int(FaultPositions::CommunicationTimeout);

        return faults;
    }

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