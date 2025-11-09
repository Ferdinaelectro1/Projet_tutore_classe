#include "../include/gsm_manager.h"

gsm::SIM800::SIM800(HardwareSerial& sim800) : _sim800(sim800){
  _oldCommandTime = 0;
  _current_task_index = 0;
  _number_of_task = 0;

}

void gsm::SIM800::begin(unsigned long baudrate){
    _sim800.begin(baudrate, SERIAL_8N1, SIM800_RX, SIM800_TX);
    delay(3000);
    Serial.println("Module SIM800 prêt.");
}

void gsm::SIM800::addTask(const Task& task)
{
  if(_number_of_task < MAX_TACHE)
  {
    _tasks[_number_of_task] = task;
    _number_of_task++;
  }
}

void gsm::SIM800::update()
{
  unsigned long now = millis();
  switch(_currentState)
  {
    case State::IDLE :
      if(_number_of_task) //si il y a de tache 
      {
        _currentState = State::SEND_NEXT_INTRUCTION; //executer la prochaine tache
        _currentTask = _tasks[_current_task_index];
      }
    break;

    case State::MAKING_CALL :
      if(now - _oldCommandTime >= COMMAND_INTERVALLE)
      {
        MakeCall();
        makeCall_step ++;
        _oldCommandTime = now;
      }
    break;
    
    case State::SENDING_SMS :
      if(now - _oldCommandTime >= COMMAND_INTERVALLE)
      {
        SendSms();
        sendSMS_step ++;
        _oldCommandTime = now;
      }
    break;
    
    case State::SEND_NEXT_INTRUCTION :
       //si on atteint le nombre de tache ajouté, c'est la fin des taches
       if(_current_task_index >=  _number_of_task)
       {
          endOfTasks();
          break;
       }

      //En fonction du type de tache à executer
       switch(_currentTask.taskType)
       {
          case TaskType::TASK_NONE : 
            break;

          case TaskType::TASK_MAKE_CALL :
            _currentState = State::MAKING_CALL;
            makeCall_step = 0;
          break;

          case TaskType::TASK_SEND_SMS :
            _currentState = State::SENDING_SMS;
            sendSMS_step = 0;
          break;

          default:
          break;
       }
       //on passe à la tache suivante
       _current_task_index++;
    break;
    
    default : 
    break;
  }
}

void gsm::SIM800::MakeCall()
{
  String cmd = "ATD+229"+_currentTask.number+";";
  _sim800.println(cmd);
  _currentState = State::SEND_NEXT_INTRUCTION;
}

void gsm::SIM800::SendSms()
{
    switch (sendSMS_step)
    {
    case 0 :
      Serial.println("Envoi du SMS...");
      _sim800.println("AT+CMGF=1"); // mode texte
      digitalWrite(2,!digitalRead(2));
      break;

    case 1 :
      _sim800.println("AT+CMGS=\"+" + _currentTask.number + "\""); // ton numéro ici
      digitalWrite(2,!digitalRead(2));
      break;

    case 2 :
      _sim800.println(_currentTask.message);
      digitalWrite(2,!digitalRead(2));
      break;

    case 3 :
      _sim800.write(26); // <-- ceci envoie le Ctrl+Z
      digitalWrite(2,!digitalRead(2));
      break;

    //Fin de l'envoie des messages , on retourne l'état du système
    case 4 :
      Serial.println("SMS envoyé !");
      _currentState = State::SEND_NEXT_INTRUCTION;
      digitalWrite(2,!digitalRead(2));
      break;

    default:
      break;
    }
}

void gsm::SIM800::endOfTasks()
{
  _number_of_task  = 0;
  _current_task_index = 0;
  _currentState = State::IDLE;
}