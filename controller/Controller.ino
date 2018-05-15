// Mechatronics Project
// Author: Harrison Yan

#include <SoftwareSerial.h> // Include software serial library, ESP8266 library dependency
#include <SparkFunESP8266WiFi.h> // Include the ESP8266 AT library
#include <WiFi.h>

// WiFi
const char ssid[] = "Olin518";
const char pw[] = "RaslOlin518";

// Must use ESP8266Server, not WifiServer
// <Wifi.h> does not work with Wifi Shield firmware
ESP8266Server server(80);
ESP8266Client client;

// Static IP
IPAddress ip(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Input channels
int left_A1 = A4;
int left_A2 = A5;
int left_D1 = 2;
int right_A1 = A0;
int right_A2 = A1;
int right_D1 = 7;

// Input channel variables to initiate based on the drive mode
int input_joystick1_select;
int input_joystick1_x;
int input_joystick1_y;
int input_joystick2_select;
int input_joystick2_x;
int input_joystick2_y;
int input_btn;

// Default values
int btn_default = 0;
int joystick_default = 520;

// mode = 1 -> 1 joystick -> classic drive
// mode = 2 -> 2 joysticks -> tank drive
int input_mode;

// Input values
int x_val;
int y_val;
int btn_val;
int left_joystick;
int right_joystick;
int left_select;
int right_select;

// D-pad threshold values for recognizing button press
int btn_vals[] = {340,511,1020,278,0,613,234,306,438};
int btn_threshold = 10;

// Initialize communication between Wifi shield & Arduino
void initializeShield() {
    if (esp8266.begin()){
        Serial.println("[SHIELD] Communication initialized");
    } else {
        Serial.println("[SHIELD] Communication initialization failed");
    }
}

// Connect to Wifi
void initializeWifi() {
    Serial.println("[WIFI] Initializing...");
    int retVal = esp8266.getMode();
    if (retVal != ESP8266_MODE_STAAP) {
        retVal = esp8266.setMode(ESP8266_MODE_STAAP);
        if (retVal < 0) {
            Serial.println(F("Error setting mode."));
        }
    }
    Serial.println("[WIFI] Mode set to access point");
    Serial.print("[WIFI] Connecting to ");
    Serial.print(ssid);
    //esp8266.config(ip,gateway,subnet);
    retVal = esp8266.connect(ssid, pw);
    while (esp8266.status()!=1) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("[WIFI] Connected");
    Serial.print("[WIFI] IP address ");
    Serial.println(esp8266.localIP());
}

// Start a server on the router
void initializeServer() {
    Serial.println("[SERVER] Initializing...");
    server.begin();
    Serial.print("[SERVER] Started at ");
    Serial.println(esp8266.localIP());
}

// Find first client available and connects to it
void initializeClient() {
    client = server.available();    
}

// Module recognition based on analog reading of Pin 
void initializeInputMode() {
    pinMode(left_A1,INPUT);
    pinMode(left_A2,INPUT);
    pinMode(left_D1,INPUT_PULLUP);
    pinMode(right_A1,INPUT);
    pinMode(right_A2,INPUT);
    pinMode(right_D1,INPUT_PULLUP);

    int btn = 0;
    int joystick = 1;
    int left;
    int right;

    // Left module
    if (abs(analogRead(left_A1)-joystick_default) < 50) {
        left = joystick;
        Serial.println("[INPUT] Left Side: Joystick");
    } else {
        left = btn;
        Serial.println("[INPUT] Left side: D-pad");
    }

    // Right module
    if (abs(analogRead(right_A1)-joystick_default) < 50) {
        right = joystick;
        Serial.println("[INPUT] Right side: Joystick");
    } else {
        right = btn;
        Serial.println("[INPUT] Right side: D-pad");
    }

    if ((left+right)==2){
        input_mode = 2;
        Serial.println("[INPUT] Mode 2 Tank");
        input_joystick1_select = left_D1;
        input_joystick1_y = left_A1;
        input_joystick1_x = left_A2;
        input_joystick2_select = right_D1;
        input_joystick2_y = right_A1;
        input_joystick2_x = right_A2;
    } else {
        input_mode = 1;
        Serial.println("[INPUT] Mode 1 Forza");
        if (left) {
            input_joystick1_select = left_D1;
            input_joystick1_y = left_A1;
            input_joystick1_x = left_A2;
            input_btn = right_A1;
        } else {
            input_joystick1_select = right_D1;
            input_joystick1_y = righpct_A1;
            input_joystick1_x = right_A2;
            input_btn = left_A1;
        }
    }
}

void readInput_mode1() {
    x_val = analogRead(input_joystick1_x);
    y_val = analogRead(input_joystick1_y);
    btn_val = analogRead(input_btn);
}

void parseInput_mode1() {
    int btn_pressed = 0;
    if (btn_val > 150) {
        for (int i = 0; i < 9; i++) {
            if (abs(btn_val - btn_vals[i]) < btn_threshold) {
                btn_pressed = i+1;
            }
        }
    }
    if (btn_pressed==1 || btn_pressed==2 || btn_pressed==3) {
        x_val = 55;
        y_val = 99;
        for (int i = 0; i < 5; i++) {
            client.println(5599);
        }
    } else if (btn_pressed==4) {
        client.println(20000);
    } else if (btn_pressed==6) {
        client.println(30000);
    } else {
        x_val = map(x_val, 0, 1023, 99, 10);
        y_val = map(y_val, 0, 1023, 84, 25);
        client.println(x_val*100+y_val);
    }
}

void readInput_mode2() {
    left_joystick = analogRead(input_joystick1_y);
    Serial.print("left: ");
    Serial.print(left_joystick);
    right_joystick = analogRead(input_joystick2_y);
    Serial.print("right: ");
    Serial.println(right_joystick);
    left_select = digitalRead(input_joystick1_select);
    right_select = digitalRead(input_joystick2_select);
}

void parseInput_mode2() {
    if (left_select==0 || right_select==0) {
        for (int i = 0; i < 5; i++) {
            client.println(19999);
        }
    } else {
        left_joystick = map(left_joystick, 0, 1023, 84, 25);
        right_joystick = map(right_joystick, 0, 1023, 84, 25);
        client.println(10000+left_joystick*100+right_joystick);
    }
}

void setup() {

    Serial.begin(9600);
    delay(200);

    initializeShield();

    initializeWifi();    
    
    initializeServer();

    initializeInputMode();

    initializeClient();    
}

void loop() {

    if (!client){
        initializeClient();
    }

    if (input_mode == 1) {
        readInput_mode1();
        parseInput_mode1();
    } else if (input_mode == 2) {
        readInput_mode2();
        parseInput_mode2();
    } else {
        // do nothing
    } 
      
}