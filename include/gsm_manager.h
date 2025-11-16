#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

#define SIM800_TX 17  // ESP32 TX2
#define SIM800_RX 16  // ESP32 RX2

#define MAX_TACHE 5
#define COMMAND_INTERVALLE 1000
#define CALL_INTERVALLE 5000

namespace gsm
{
  enum class State {
    IDLE,
    SENDING_SMS,
    MAKING_CALL,
    SEND_NEXT_INTRUCTION
  };

  enum class TaskType
  {
    TASK_MAKE_CALL,
    TASK_SEND_SMS,
    TASK_NONE
  };

  struct Task
  {
    TaskType taskType;
    String number;
    String message;
  };

  class SIM800
  {
        public:
          SIM800(HardwareSerial& sim800);
          void begin(unsigned long baudrate = 9600);
          void addTask(const Task& task);
          void update();

        private:
          HardwareSerial& _sim800;
          Task _tasks[MAX_TACHE];
          u_int8_t _current_task_index;
          u_int8_t _number_of_task;
          State _currentState;
          Task _currentTask;
          unsigned long _oldCommandTime;
          u_int8_t sendSMS_step;
          u_int8_t makeCall_step;

        private:
          void endOfTasks();
          void MakeCall();
          void SendSms();
    };
};

