#include <Arduino.h>
#include <ESP8266WiFi.h>           // необхідгий для роботи WIFI
#include <WiFiClient.h>            // необхідгий для роботи серверу
#include <ESP8266WebServer.h>      // необхідгий для роботи серверу
#include <Ticker.h>

const char *ssid = "Lab124";/*"LEAGOO_KIICAA_POWER";/"YouShellNotPassRouter";   //назва точки доступу*/
const char *password = "";//"1234567890";

//IPAddress ip(192, 168, 1, 17);  //статический IP
//IPAddress gateway(192, 168, 1, 18);
//IPAddress subnet(255, 255, 255, 0);
WiFiClient client;

int endNum = 0;
int hour = 0, minute = 0, sec = 0;
int offH = 23, offM = 1;
int onH = 23, onM = 0;
int timeout = 0;
// bool logInFlag = false;
// String passwordMy = "804d41e4e902012c3441cc0a3b0ec42f";
String turnOn = "Off";

Ticker oneSec;
#define LED 2  //On board LED


ESP8266WebServer server(80);       // ініціалізація серверу на 80 порту

void function() {}

void changeSec() {

    if (++sec > 59) {
        sec = 0;
        minute++;
        ifOn();
    }
    if (minute > 59) {
        minute = 0;
        hour++;
    }
    if (hour > 23) {
        hour = 0;
    }

    ifOn();
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(sec);
}

//Command to rele
void ifOn() {
    if (hour == offH && minute == offM) {
        turnOn = "Off";
    }
    if (hour == onH && minute == onM) {
        turnOn = "On";
    }
    turnOnFun();
}

void turnOnFun() {
    if (turnOn == "On") {
        digitalWrite(LED, LOW);
    } else {
        digitalWrite(LED, HIGH);
    }
}

//SetUp
void setup() {
    digitalWrite(LED, HIGH);
    Serial.begin(115200);
    delay(1000);
    pinMode(2, OUTPUT);
    WiFi.mode(WIFI_AP);              // налаштування режимум роботи
    //WiFi.softAP(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && endNum++ <= 15) {
        delay(500);
        Serial.print(".");
    }
    //if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());// параметри створеної мережі
    server.on("/", []() {            // обробка GET запиту
        server.send(200, "text/html",
                    main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
    });

    server.on("/example_get", []() {            // обробка GET запиту
            offH = server.arg("F_name").toInt();
            offM = server.arg("S_name").toInt();
            onH = server.arg("years").toInt();
            onM = server.arg("onM").toInt();
            turnOn = server.arg("turnOn");
            Serial.println(turnOn);
            hour = server.arg("ha").toInt();
            minute = server.arg("mi").toInt();
            sec = server.arg("se").toInt();
            turnOnFun();
        Serial.println("Get responce");
        server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит

    });

    server.on("/logout", []() {            // обробка GET запиту
//        logOut();
        Serial.println("LogOut");
        server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
    });

    server.on("/changeTurnOn", HTTP_POST, []() {
          // offH = server.arg("F_name").toInt();
          // offM = server.arg("S_name").toInt();
          // onH = server.arg("years").toInt();
          // onM = server.arg("onM").toInt();
          turnOn = server.arg("turnOn");
          Serial.println(turnOn);
          hour = server.arg("ha").toInt();
          minute = server.arg("mi").toInt();
          sec = server.arg("se").toInt();
          turnOnFun();
      Serial.println("Post responce");
      server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
      });
    server.begin();                  // запуск серверу
    oneSec.attach(1, changeSec);
    // turnOnFun();
}

void loop() {
    server.handleClient();           // перевірка серверу на нові запити
    if(client.available()){
      char c = client.read();
      Serial.print(c);
    }
}

String main_page(String turnOnStatus, String hourStatus, String minStatus, String secStatus) {
    String data = "<!DOCTYPE HTML> ";
    data += "<html> ";
    data += "<head> ";
    data += "<meta charset='utf-8'> ";
    data += "<title>Welcome to ESP</title> ";
    data += "</head> ";
    data += "<body> ";

        Serial.println(turnOnStatus);
        turnOnFun();
        data += "<h1 align='center'> Welcome  </h1> ";

        data += "<table align='center'><tr><td width='40%' align='center'><p> <b>Status </b></p></td><td width='40%' align='center'><p> <b>Set </b></p></p></td></tr><tr><td width='40%'>";
        data += "<input size='1' value='" + hourStatus + "' type='text' disabled>";
        data += "<input size='1' value='" + minStatus + "' type='text' disabled>";
        data += "<input size='1' value='" + secStatus + "' type='text' disabled>";
        data += "<p> <b>Off hour : " + String(offH) + "</b></p>";
        data += "<p> <b>Off minute : " + String(offM) + "</b></p>";
        data += "<p> <b>On hour : " + String(onH) + "</b></p>";
        data += "<p> <b>On minute : " + String(onM) + "</b></p>";
        data += "<p> <b>turnOn Status : " + turnOnStatus + "</b></p>";
        data += "</td><td width='40%'>";

        data += "<form action='example_get' align='center'>";
        data += "<input size='1' name='ha' type='text' >";
        data += "<input size='1' name='mi' type='text' >";
        data += "<input size='1' name='se' type='text' >";
        data += "<p> <b> off hour : </b><input type='text' value='" + String(offH) + "' name='F_name'></p>";
        data += "<p> <b> off minute : </b><input type='text' value='" + String(offM) + "' name='S_name'></p>";
        data += "<p> <b> on hour : </b><input type='text' value='" + String(onH) + "' name='years'></p>";
        data += "<p> <b> on minute : </b><input type='text' value='" + String(onM) + "' name='onM'></p>";

        if (turnOnStatus == "On") {
            data += "<p> <b> TurnOn : </b><select name='turnOn'><option>On</option><option>Off</option></select></p>";
        } else {
            data += "<p> <b> TurnOn : </b><select name='turnOn'><option>Off</option><option>On</option></select></p>";
        }
        data += "</td></tr></table>";

        data += "<script>";
        data += "";
        data += "window.onload = function(){";
        data += "(function(){";
        data += "var d = new Date();";
        data += "var timeControlS = document.querySelector('input[name = \"se\" ]');";
        data += "var timeControlM = document.querySelector('input[name=\"mi\" ]');";
        data += "var timeControlH = document.querySelector('input[name=\"ha\" ]');";
        data += "timeControlS.value = d.getSeconds();";
        data += "timeControlM.value = d.getMinutes();";
        data += "timeControlH.value = d.getHours();";
        data += "window.setTimeout(arguments.callee, 1000);";
        data += "})();};";
        data += "</script>";

        data += "<p align='center'><button type='submit'>Send</button></p>";
        data += "</form>";
        data += "<form action='/' align='center'>";
        data += "<button>Reload</button>";
        data += "</form>";
    data += "<p>";

    return data;
}
