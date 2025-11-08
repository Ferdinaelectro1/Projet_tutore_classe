#include "../include/gsm_manager.h"

gsm::SIM800::SIM800(HardwareSerial& sim800) : _sim800(sim800){
  _oldCommandTime = 0;
}

void gsm::SIM800::begin(unsigned long baudrate){
    _sim800.begin(baudrate, SERIAL_8N1, SIM800_RX, SIM800_TX);
    delay(3000);
    Serial.println("Module SIM800 prêt.");
}

void gsm::SIM800::call(const String& number){
    String cmd = "ATD+229"+number+";";
    _sim800.println(cmd);
}

void gsm::SIM800::send_msg(const String& number,const String& msg)
{
  Serial.println("Envoi du SMS...");

  _sim800.println("AT+CMGF=1"); // mode texte
  delay(1000);

  _sim800.println("AT+CMGS=\"+" + number + "\""); // ton numéro ici
  delay(1000);

  _sim800.println(msg);
  _sim800.write(26); // <-- ceci envoie le Ctrl+Z
  delay(1000);

  Serial.println("SMS envoyé !");

}
