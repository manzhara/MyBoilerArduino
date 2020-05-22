  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>

  #define LED 2

  String turnOn = "Off";

  void turnOnFunction() {
    if(turnOn == "On") {
      digitalWrite(LED, LOW);
    } else {
      digitalWrite(LED, HIGH);
    }
  }

  void setup() {
    pinMode(2, OUTPUT);
    digitalWrite(LED, HIGH);
    Serial.begin(115200);
    Serial.println("Start");
  }

  void loop() {
    turnOnFunction();
    delay(1000);
  }
