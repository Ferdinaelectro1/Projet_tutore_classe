#include <Arduino.h>
#include "../include/gsm_manager.h"
#include <HardwareSerial.h>

#define SWITCH 5
#define LED 2
#define DETECTION_TIME_OUT 200


HardwareSerial SIM800(2); // Use UART2
gsm::SIM800 gsmModule(SIM800);
bool detecteShief = false;

void setup() {
     Serial.begin(115200); 
    pinMode(SWITCH, INPUT);
    gsmModule.begin(); 
    pinMode(LED,OUTPUT);
}

void loop() {
  gsmModule.update();
   if (digitalRead(SWITCH) == HIGH) {
    Serial.println("Switch is ON");
    gsmModule.call("0153005087");
  } else {
    Serial.println("Switch is OFF");
  }
  digitalWrite(LED,!digitalRead(LED));
  delay(1);
}
