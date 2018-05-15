// Mechatronics Project
// Author: Harrison Yan
// Author: Tristan Irvin

#include <ESP8266WiFi.h>

int motorA1 = 16;
int motorA2 = 5;
int motorApwm = 12;
int motorB1 = 4;
int motorB2 = 0;
int motorBpwm = 14;
int y = 0;
int x = 0;
int r = 0;
int dpad = 0;
int drivemode = 0;
int left;
int right;

const char* ssid = "Olin518";
const char* pw = "RaslOlin518";

IPAddress serverIP(192,168,1,102);
WiFiClient controller;

int wifiStatus;

// Author: Harrison Yan
void initializeWifi() {
    Serial.println("[WIFI] Initializing...");
    Serial.print("[WIFI] Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pw);
    while (WiFi.status()!=WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("[WIFI] Connected");
    Serial.print("[WIFI] IP Address ");
    Serial.println(WiFi.localIP());
}

void initializeClient() {
    Serial.println("[Client] Initializing...");
    controller.connect(serverIP, 80);
    Serial.print("[Client] Connected to server at ");
    Serial.println(serverIP);
}

// Author: Harrison Yan
void getXY(){
  int val = controller.parseInt();
  if (val/10000 == 1) {
    left = (val-10000)/100;
    right = (val-10000)%100;
    left = map(left, 10, 99, 0, 1024);
    right = map(right, 10, 99, 0, 1024);
    drivemode = 1;
  } else if (val == 20000) {
    // left wheel backwards
    // right wheel forward
  } else if (val == 30000) {
    // left wheel forward
    // right wheel backward
  } else {
    x = val/100;
    x = map (x,10,99,-512,512);
    y = val%100;
    y = map (y,10,99,-512,512);
    drivemode = 0;
  }
  Serial.print("y: ");
  Serial.print(y);
  Serial.print(" x: ");
  Serial.println(x);
  controller.flush();
}

// Author: Harrison Yan
// Author: Tristan Irvin
void setup() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorApwm, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(motorBpwm, OUTPUT);
  Serial.begin(9600);
  delay(200);

  initializeWifi();

  initializeClient();
}

// Author: Tristan Irvin
void loop() {
  getXY();
  if (drivemode != 1){
    if (x*x + y*y <= 2500){ // implements a deadzone where the car won't move unless the joystick
      stopMotors();              // is pushed far enough from the origin.
    } else {
      if (x >= 0 && y >= 0){ // Separate the joystick by quadrants, each with their own function. 
        forwardRight(x, y);
      }
      if (x < 0 && y >= 0){
        forwardLeft(x, y);
      }
      if (x > 0 && y < 0){
        backwardRight(x, y);
      }
      if (x <= 0 && y < 0){
        backwardLeft(x, y);
      }
    }
  } else {
    if (left < 500) {
      digitalWrite(motorA1, LOW);
      digitalWrite(motorA2, HIGH);
      analogWrite(motorApwm, left*2); 
    } else if (left > 530) {
      digitalWrite(motorA2, LOW);
      digitalWrite(motorA1, HIGH);
      analogWrite(motorApwm, (left - 512)*2);
    }
    if (right < 500) {
      digitalWrite(motorB2, LOW);
      digitalWrite(motorB1, HIGH);
      analogWrite(motorBpwm, right*2); 
    } else if (right > 530) {
      digitalWrite(motorB1, LOW);
      digitalWrite(motorB2, HIGH);
      analogWrite(motorBpwm, (right - 512)*2);
    }
  }
}
    
void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorApwm, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
  digitalWrite(motorBpwm, LOW);
}

void forwardRight(int xpos, int ypos){
  digitalWrite(motorA2, LOW);
  digitalWrite(motorA1, HIGH);
  analogWrite(motorApwm, ypos*1.6);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
  if(x < 100){
    analogWrite(motorBpwm, ypos*1.6);
  } else {
    analogWrite(motorBpwm, 819 - xpos*1.3);
  }
}

void forwardLeft(int xpos, int ypos){
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, HIGH);
  analogWrite(motorBpwm, ypos*1.6);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorA1, HIGH);
  if(x > -100){
    analogWrite(motorApwm, ypos*1.6);
  } else {
    analogWrite(motorApwm, xpos*1.3 + 819);
  }
}

void backwardRight(int xpos, int ypos){
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  analogWrite(motorApwm, ypos*1.6);
  digitalWrite(motorB2, LOW);
  digitalWrite(motorB1, HIGH);
  if (x < 150) {
    analogWrite(motorBpwm, ypos*1.6);
  } else {
    analogWrite(motorBpwm, 819 - xpos*1.3);
  }
 }


void backwardLeft(int xpos, int ypos){
  digitalWrite(motorB2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, HIGH);
  analogWrite(motorBpwm, ypos*1.6);
  if (x > -150) {
    analogWrite(motorApwm, ypos*1.6);
  } else {
    analogWrite(motorApwm, xpos*1.3 + 819);
  }
}
