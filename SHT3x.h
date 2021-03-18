#include <mbed.h>

#define DEF_ADDR 0x45 << 1
typedef enum{
    MEAS_HIGHREP_STRETCH  = 0x2C06,
    MEAS_MEDREP_STRETCH   = 0x2C0D,
    MEAS_LOWREP_STRETCH   = 0x2C10,
    MEAS_HIGHREP          = 0x2400,
    MEAS_MEDREP           = 0x240B,
    MEAS_LOWREP           = 0x2416,
    READSTATUS            = 0xF32D,
    CLEARSTATUS           = 0x3041,
    SOFTRESET             = 0x30A2,
    HEATEREN              = 0x306D,
    HEATERDIS             = 0x3066
} SHT3x_Cmd_t;

typedef enum{
    LOW = 0,
    MEDIUM,
    HIGH
} SHT3x_Repeatability_t;

typedef union{
    struct{
        uint16_t alerts :1;
        uint16_t res :1;
        uint16_t heater :1;
        uint16_t res1 :1;
        uint16_t RH_alert :1;
        uint16_t T_alert :1;
        uint16_t res2 :5;
        uint16_t reset_detected :1;
        uint16_t last_cmd_fail :1;
        uint16_t crc_fail :1;
    } fields;
    uint16_t value;
    uint8_t bytes[2];
} SHT3x_Status_t;

class SHT3x {
    public:
        SHT3x(I2C* bus, uint8_t addr = DEF_ADDR);
        SHT3x(I2C* bus, PinName pin, uint8_t addr = DEF_ADDR);
        void AttachAlarmOn(Callback< void()> cb);
        void AttachAlarmOff(Callback< void()> cb);
        float GetTemp(void);
        float GetHum(void);
        bool Read(SHT3x_Repeatability_t rep);
        SHT3x_Status_t ReadStatus(bool clear = false);
        void ClearStatus(void);
    private:
        void SendCmd(SHT3x_Cmd_t cmd);
        void Reset (void);
        uint8_t crc(uint8_t* data, size_t len);
        
        InterruptIn* _irq;
        I2C* _bus;
        uint8_t _addr;
        float _temperature;
        float _humidity;
};