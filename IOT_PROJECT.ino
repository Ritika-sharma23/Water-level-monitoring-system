#define BLYNK_TEMPLATE_ID "TMPL55F_hIwUQ"
#define BLYNK_TEMPLATE_NAME "WATER LEVEL MONITORING SYSTEM"
#define BLYNK_AUTH_TOKEN "U5l3SF9VG6KG7Nph4c8_eTKt2F1TUzBI"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define TRIG_PIN D1
#define ECHO_PIN D2
#define GREEN_LED D3
#define RED_LED D4
#define RELAY_PIN D7

char ssid[] = "internet";
char pass[] = "12345678";

float distance;
bool motorOn = false;

const float EMPTY_DISTANCE = 8.0;
const float FULL_DISTANCE  = 3.0;

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  Serial.println("System Started...");
}

float getDistance() {
  long duration;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return (duration * 0.0343) / 2;
}

void loop() {
  Blynk.run();
  distance = getDistance();
  float levelPercent = ((EMPTY_DISTANCE - distance) / (EMPTY_DISTANCE - FULL_DISTANCE)) * 100.0;
  if (levelPercent < 0) levelPercent = 0;
  if (levelPercent > 100) levelPercent = 100;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Level: ");
  Serial.print(levelPercent);
  Serial.println(" %");
  Blynk.virtualWrite(V1, distance);
  Blynk.virtualWrite(V3, levelPercent);
  if (distance > 0 && distance <= FULL_DISTANCE) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    if (motorOn) {
      motorOn = false;
      Blynk.virtualWrite(V2, "MOTOR OFF");
      Blynk.logEvent("motor_status", "Motor turned OFF (Tank Full)");
      Serial.println("MOTOR OFF (Tank Full)");
    }
  } else if (distance >= EMPTY_DISTANCE) {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    if (!motorOn) {
      motorOn = true;
      Blynk.virtualWrite(V2, "MOTOR ON");
      Blynk.logEvent("motor_status", "Motor turned ON (Tank Empty)");
      Serial.println("MOTOR ON (Tank Empty)");
    }
  } else {
    if (motorOn) {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
  }
  delay(1000);
}
