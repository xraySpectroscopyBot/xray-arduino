#include <ArduinoJson.h>

#define stp 23
#define dir 22

long pos = 0;
long target = 0;
int velocity = 2000;

unsigned long start = 0;

void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);

  digitalWrite(dir, false);
  digitalWrite(stp, false);

  Serial.begin(9600);
  while (!Serial) {}
}

void loop() {
  digitalWrite(stp, false);

  if (Serial.available()) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, Serial);
    String cmd = doc["command"];
    
    if (cmd == "goto") {
      int doc_steps = doc["steps"].as<int>();
      if (doc_steps != 0) {
        target += doc_steps;
        int doc_velocity = doc["velocity"].as<int>();
        if (doc_velocity >= 1000 and doc_velocity <= 10000) {
          velocity = doc_velocity;
        } else {
          velocity = 2000;
        }
      }
    } else if (cmd == "move" and not doc["direction"].isNull()) {
      if (doc["direction"] == "up") {
        target += 19200;
      } else {
        target -= 19200;
      }
      int doc_velocity = doc["velocity"].as<int>();
      if (doc_velocity >= 1000 and doc_velocity <= 10000) {
        velocity = doc_velocity;
      } else {
        velocity = 2000;
      }
    } else if (cmd == "stop") {
      target = pos;
    } else if (cmd == "position") {
      StaticJsonDocument<200> doc;
      doc["position"] = pos;
      serializeJson(doc, Serial);
      Serial.println();
    } else if (cmd == "sethome") {
      target = 0;
      pos = 0;
    } else if (cmd == "ping") {
      Serial.println("pong");
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

    delayMicroseconds(500);
  }
}
