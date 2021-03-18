#include <mbed.h>
#include "SHT3x.h"

I2C bus(I2C_SDA, I2C_SCL);

SHT3x sensor(&bus);

int main() {

  // put your setup code here, to run once:

  while(1) {
    sensor.Read(HIGH);
    printf("Temperature: %d.%02d\r\n", (int)sensor.GetTemp(), (int)(sensor.GetTemp()*100) % 100);
    printf("Humidity: %d.%02d\r\n\r\n", (int)sensor.GetHum(), (int)(sensor.GetHum()*100) % 100);
    ThisThread::sleep_for(1s);
  }
}