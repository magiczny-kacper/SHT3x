#include "SHT3x.h"

SHT3x::SHT3x(I2C* bus, uint8_t addr){
    _bus = bus;
    _addr = addr;
    _temperature = 0.0f;
    _humidity = 0.0f;
    _irq = NULL;
    Reset();
}

SHT3x::SHT3x(I2C* bus, PinName pin, uint8_t addr){
    _bus = bus;
    _addr = addr;
    _temperature = 0.0f;
    _humidity = 0.0f;
    _irq = new InterruptIn(pin);
    Reset();
}

void SHT3x::AttachAlarmOn(Callback< void()> cb){
    if(NULL != _irq) _irq->rise(cb);
}

void SHT3x::AttachAlarmOff(Callback< void()> cb){
    if(NULL != _irq) _irq->fall(cb);
}

float SHT3x::GetTemp(void){
    return _temperature;
}

float SHT3x::GetHum(void){
    return _humidity;
}

bool SHT3x::Read(SHT3x_Repeatability_t rep) {
    char readbuffer[6];
    
    switch (rep){
        case HIGH:
            SendCmd(MEAS_HIGHREP);
            ThisThread::sleep_for(20ms);
            break;
        case MEDIUM:
            SendCmd(MEAS_MEDREP);
            ThisThread::sleep_for(10ms);
            break;
        case LOW:
            SendCmd(MEAS_LOWREP);
            ThisThread::sleep_for(8ms);
            break;
        default:
            return false;
            break;
    }

    _bus->read(_addr, readbuffer, 6);
    
    uint16_t hum_raw, temp_raw;
    temp_raw = ((uint16_t)readbuffer[0] << 8) | readbuffer[1];
    if (readbuffer[2] != crc((uint8_t *) readbuffer, 2)) {
        return false;
    }
    
    hum_raw = ((uint16_t)readbuffer[3] << 8) | readbuffer[4];
    if (readbuffer[5] != crc((uint8_t *) readbuffer+3, 2)) {
        return false;
    }
 
    float temp = temp_raw;
    temp *= 175.0f;
    temp /= 0xFFFF;
    temp = -45.0f + temp;
    _temperature = temp;
 
    float hum = hum_raw;
    hum *= 100.0f;
    hum /= 0xFFFF;
 
    _humidity = hum;
 
    return true;
}

SHT3x_Status_t SHT3x::ReadStatus(bool clear){
    char readbuffer[3];
    SHT3x_Status_t status;
    SendCmd(READSTATUS);
    _bus->read(_addr, &readbuffer[0], 3);
    if (readbuffer[2] != crc((uint8_t *) readbuffer, 2)) {
        status.value = 0xFFFF;
        return status;
    }

    status.bytes[0] = readbuffer[1];
    status.bytes[1] = readbuffer[0];
    if(clear) ClearStatus();
    return status;
}

void SHT3x::ClearStatus(void){
    SendCmd(CLEARSTATUS);
}

void SHT3x::SendCmd(SHT3x_Cmd_t cmd){
    char toSend[2];
    toSend[0] = (cmd >> 8) & 0xFF;
    toSend[1] = (cmd & 0xFF);

    _bus->write(_addr, &toSend[0], 2);
}

void SHT3x::Reset(void){
    SendCmd(SOFTRESET);
    ThisThread::sleep_for(10ms);
}

uint8_t SHT3x::crc(uint8_t* data, size_t len){
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc1 = 0xFF;
 
    for (size_t j = len; j > 0; --j ) {
        crc1 ^= *data++;
        for (int i = 8; i > 0; --i ) {
            crc1 = (crc1 & 0x80) ? (crc1 << 1) ^ POLYNOMIAL : (crc1 << 1);
        }
    }
    return crc1;
}