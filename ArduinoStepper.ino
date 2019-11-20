#include <ArduinoJson.h>

#define stp 10
#define dir 16

long pos = 0;
long target = 0;
int velocity = 1000;

unsigned long start = 0;

void setup() {
  Serial.begin(9600);
  //while (!Serial) {}
  
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
}

void loop() {
  digitalWrite(stp, false);

  if (Serial.available()) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, Serial);
    String cmd = doc["command"];
    
    if (cmd == "goto") {
      target += doc["steps"].as<int>();
      velocity = doc["velocity"];
    } else if (cmd == "move") {
      if (doc["direction"] == "up") {
        target += 19200;
      } else {
        target -= 19200;
      }
      velocity = doc["velocity"];
    } else if (cmd == "stop") {
      target = pos;
    } else if (cmd == "position") {
      StaticJsonDocument<200> doc;
      doc["position"] = pos;
      serializeJson(doc, Serial);
      Serial.println();
    }
  }
  
  if (micros() > start) {
    start = micros() + velocity;

    if (target < pos) {
      digitalWrite(dir, false);
      digitalWrite(stp, true);
      pos--;
    } else if (target > pos) {
      digitalWrite(dir, true);
      digitalWrite(stp, true);
      pos++;
    }
  }
}
