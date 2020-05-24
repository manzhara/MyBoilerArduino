#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LED 2

const char *ssid = "YouShellNotPassRouter"; //name of WiFi
const char *password = ""; //password of WiFi

String turnOn = "On";
int hour = 0, minute = 0, sec = 0;
int offH = 0, offM = 1;
int onH = 0, onM = 0;
int endNum = 0;

Ticker oneSec;
WiFiClient client;
ESP8266WebServer server(80);

void function() {}

void turnOnFunction() {
  if (turnOn == "On"){
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }
}

void ifOn() {
  if(hour == offH && minute == offM) {
    turnOn = "Off";
  }
  if(hour == onH && minute == onM) {
    turnOn = "On";
  }
  turnOnFunction();
}

void changeSec() {
  if(++sec > 59) {
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
  // Serial.println(":");
}

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Start");

  WiFi.mode(WIFI_AP);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED && endNum++ < 15){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    server.on("/", []() {
        server.send(200, "text/html",
                    main_page(turnOn,
                    String(hour),
                    String(minute),
                    String(sec)));
    });
        server.on("/example_get", []() {            // обробка GET запиту
            offH = server.arg("F_name").toInt();
            offM = server.arg("S_name").toInt();
            onH = server.arg("years").toInt();
            onM = server.arg("onM").toInt();
            turnOn = server.arg("turnOn");
            hour = server.arg("ha").toInt();
            minute = server.arg("mi").toInt();
            sec = server.arg("se").toInt();
            Serial.println("Get responce");
            turnOnFunction();
        server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
    });

    server.on("/set_time", []() {            // обробка GET запиту
            offH = server.arg("F_name").toInt();
            offM = server.arg("S_name").toInt();
            onH = server.arg("years").toInt();
            onM = server.arg("onM").toInt();
            // turnOn = server.arg("turnOn");
            // hour = server.arg("ha").toInt();
            // minute = server.arg("mi").toInt();
            // sec = server.arg("se").toInt();
            Serial.println("Post responce");
            turnOnFunction();
        // server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
    });

    server.on("/getInfo", []() {            // обробка GET запиту
            Serial.println("Get Info responce");
        server.send(200, "text/html", infoResponse(turnOn, String(onH), String(onM), String(offH), String(offM))); // відповідь на запит
    });

//     server.on("/logout", []() {            // обробка GET запиту
// //        logOut();
//         Serial.println("LogOut");
//         server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
//     });

    server.on("/changeTurnOn", HTTP_POST, []() {
          turnOn = server.arg("turnOn");
          Serial.println(turnOn);
          hour = server.arg("ha").toInt();
          minute = server.arg("mi").toInt();
          sec = server.arg("se").toInt();
          turnOnFunction();
      Serial.println("Post responce");
      server.send(200, "text/html", main_page(turnOn, String(hour), String(minute), String(sec))); // відповідь на запит
      });

  server.begin();

  oneSec.attach(1, changeSec);

}

void loop() {
  server.handleClient();           // перевірка серверу на нові запити
  if(client.available()){
    char c = client.read();
          Serial.print(c);
    }
  }

String infoResponse(String turnOnStatus, String hourOnStatus, String minOnStatus, String hourOffStatus, String minOffStatus) {
  String data = turnOnStatus + "." + hourOnStatus + "." + minOnStatus + "." + hourOffStatus + "." + minOffStatus;
  return data;
  // delay(1000);s
}

String main_page(String turnOnStatus, String hourStatus, String minStatus, String secStatus) {
    String data = "<!DOCTYPE HTML> ";
    data += "<html> ";
    data += "<head> ";
    data += "<meta charset='utf-8'> ";
    data += "<title>Welcome to ESP</title> ";
    data += "</head> ";
    data += "<body> ";
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
