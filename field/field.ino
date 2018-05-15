// Mechatronics Project
// Author: Harrison Yan

int yellowFSR=A0;
int redFSR=A1;
int yellowLEDs[] = {2,3,4,5,6};
int redLEDs[]= {9,10,11,12,13};
int yellowScore;
int redScore;
int FSRpower = 52;

void setup() {
    Serial.begin(9600);
    yellowScore = 0;
    redScore = 0;
    pinMode(FSRpower, OUTPUT);
    digitalWrite(FSRpower, HIGH);
    for (int i = 0; i < 5; i++) {
        pinMode(yellowLEDs[i], OUTPUT);
        digitalWrite(yellowLEDs[i], LOW);
        pinMode(redLEDs[i], OUTPUT);
        digitalWrite(redLEDs[i], LOW);
    }
    pinMode(yellowFSR, INPUT);
    pinMode(redFSR, INPUT);
    for (int i = 5; i >= 0; i--) {
        digitalWrite(redLEDs[i],HIGH);
        digitalWrite(yellowLEDs[i],HIGH);
        delay(200);
        digitalWrite(redLEDs[i],LOW);
        digitalWrite(yellowLEDs[i],LOW);
    }
    for (int i = 0; i < 5; i++) {
        digitalWrite(redLEDs[i],HIGH);
        digitalWrite(yellowLEDs[i],HIGH);
        delay(200);
        digitalWrite(redLEDs[i],LOW);
        digitalWrite(yellowLEDs[i],LOW);
    }
}

void loop() {
    delay(100);
    int redVal = analogRead(redFSR);
    int yellowVal = analogRead(yellowFSR);
    Serial.print("red: ");
    Serial.print(redVal);
    if (redVal<100) {
        redScore++;
        for (int i = 0; i < redScore; i++) {
            digitalWrite (redLEDs[i],HIGH);
        }
    }
    Serial.print(" yellow: ");
    Serial.println(yellowVal);
    if (yellowVal>1000) {
        yellowScore++;
        for (int i = 0; i < yellowScore; i++) {
            digitalWrite (yellowLEDs[i],HIGH);
        }
        delay(10000);
    }
}