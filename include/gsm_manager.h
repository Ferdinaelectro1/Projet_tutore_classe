#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#define SIM800_TX 17  // ESP32 TX2
#define SIM800_RX 16  // ESP32 RX2

namespace gsm
{
    class SIM800
    {
        public:
          SIM800(HardwareSerial& sim800);
          void begin(unsigned long baudrate = 9600);
          void call(const String& number);
          void send_msg(const String& number,const String& msg);
          void update();

        private:
          HardwareSerial &_sim800;
    };
};