#include <Arduino.h>
#include "../include/gsm_manager.h"
#include <HardwareSerial.h>

#define SWITCH 5
#define LED 2
#define DETECTION_TIME_OUT 200


HardwareSerial SIM800(2); // Use UART2
gsm::SIM800 gsmModule(SIM800);
unsigned long startDetectionTime;
bool detected = false;

void setup() {
     Serial.begin(115200); 
    pinMode(SWITCH, INPUT_PULLUP);
    gsmModule.begin(); 
    pinMode(LED,OUTPUT);
}

void loop() {
  unsigned long now =  millis();
  if (digitalRead(SWITCH) == LOW) {
    if(!detected)
      startDetectionTime = now;
    detected = true;
  } 
  if(detected)
  {
    if(now - startDetectionTime <= DETECTION_TIME_OUT )
    {
      if(digitalRead(SWITCH) == HIGH)
        detected = false;
    }
    else
    {
      Serial.println("Détection");
      gsm::Task task;
      task.number = "0147813474";
      task.message = "SMS Intrue";
      task.taskType = gsm::TaskType::TASK_SEND_SMS;
      gsmModule.addTask(task);
      detected = false;
    }
  }
  gsmModule.update();
  delay(1);
}

