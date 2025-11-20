#include <Arduino.h>
#include "../include/gsm_manager.h"
#include <HardwareSerial.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define SWITCH 4
#define LED 2
#define ECHO 5
#define DETECTION_TIME_OUT 200
#define INIT_WAIT 5000 //Attendre un peu à l'initialisation pour ignorer les évènements parasites du PIR
#define UPDATE_FIREBASE_INTERVAL 5000  // Intervalle de mise à jour Firebase en ms
#define LED_INTERVALLE_TOOGLE 500 // Intervalle de clignotement de la LED en ms


// Firebase
const char* apiKey = "AIzaSyC7YhVNnmsI_tD-6mM1p887gbcIbmPzBC4";
const char* databaseURL = "https://intruderalert-2a5d3-default-rtdb.firebaseio.com/";

//Wifi infos
const char* ssid = "PALLAS_LINE";
const char* password = "XK-B05-JoJo";

HardwareSerial SIM800(2); // Use UART2
gsm::SIM800 gsmModule(SIM800);
unsigned long startDetectionTime;
unsigned long lastFireBaseUpdateTime = 0;
unsigned long lastLedToggleTime = 0;
bool detected = false;
bool oldPirValue = false;
bool isActive = false;

// Eléments réseau utiles
FirebaseData fbdo;     // objet principal pour Realtime Database
FirebaseAuth auth;     // auth si nécessaire
FirebaseConfig config; // configuration


void handleDetect()
{
    Serial.println("Envoie du status vers Firebase ...");
    Firebase.RTDB.setBool(&fbdo,"/surveillance/statut/intrusion_detectee", true);
    Serial.println("Détection");
    gsm::Task task;
    task.number = "0147813474";
    task.message = "SMS Intrue";
    task.taskType = gsm::TaskType::TASK_MAKE_CALL;
    gsmModule.addTask(task);
    detected = false;
}

void setup() {
    delay(INIT_WAIT);
    Serial.begin(115200); 
    pinMode(SWITCH, INPUT_PULLUP);
    pinMode(ECHO, INPUT);
    gsmModule.begin(); 
    pinMode(LED,OUTPUT);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("Initialisation ...");

    // Configuration Firebase
    config.api_key = apiKey;
    config.database_url = databaseURL;

    //Authentification
    auth.user.email = "atiferdinand31@gmail.com";
    auth.user.password = "A1A2A3A4";
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println("Initialisation Firebase terminée.");
    // Initialisation de l'état dans Firebase
    Firebase.RTDB.setBool(&fbdo,"/surveillance/statut/intrusion_detectee", false);
}

void loop() {

  if(isActive) {
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
        //vérifier si l'état du fin de course est toujours maintenu
        if(digitalRead(SWITCH) == HIGH)
          detected = false;
      }
      else
      {
        handleDetect();
      }
    }

    //Si on détecte un changement d'état sur la broche echo du capteur pir
    if(oldPirValue != digitalRead(ECHO))
    {
      //Si c'est un front descendant (si l'ancienne valeur était un signal High)
      if(oldPirValue == 1)
        handleDetect();
    }
    oldPirValue = digitalRead(ECHO);

    gsmModule.update();
    //Toogle de la LED toutes les 500ms 
      if(millis() - lastLedToggleTime >= LED_INTERVALLE_TOOGLE)
      {
        digitalWrite(LED, !digitalRead(LED));
        lastLedToggleTime = millis();
      }
 }

 //Mise à jour de l'état actif/inactif toutes les 5 secondes
 if(millis() - lastFireBaseUpdateTime >= UPDATE_FIREBASE_INTERVAL)
 {
    if(Firebase.RTDB.getString(&fbdo,"/surveillance/systeme/arme"))
    {
      String dataBaseData = fbdo.stringData();
      Serial.print("Etat de arme lu dans Firebase : ");
      Serial.println(dataBaseData);
      if(dataBaseData == "true")
      {
        isActive = true;
      }
      else
      {
        isActive = false;
      }
    }
    else
    {
      Serial.print("Erreur de lecture Firebase : ");
      Serial.println(fbdo.errorReason());
    }
    lastFireBaseUpdateTime = millis();
 }
 
 delay(1);
}