#include <SPI.h>
#include "RF24.h"

// Define constants
#define MIN_PULSE_WIDTH 650
#define MAX_PULSE_WIDTH 2350
#define FREQUENCY 50

// RF24 setup
RF24 rf24(7, 8); // CE, CSN pins
const byte addr[] = "2Node";
const char msg[] = "Happy Hacking!";

// Potentiometer pin assignments
int hand = A7;
int potWrist = A6;
int potElbow = A5;
int potShoulder = A4;
int potBase = A3;

// Array to store potentiometer readings
int test[7];

// Joystick pin assignments
int xPin = 2;
int yPin = 1;
int zPin = 0;

// Variables to store joystick readings
int xval=0;
int yval=0;
char dir[10]="stop";

void setup() {
    // RF24 setup
    rf24.begin();
    rf24.setChannel(83);
    rf24.openWritingPipe(addr);
    rf24.setPALevel(RF24_PA_MIN);
    rf24.setDataRate(RF24_250KBPS);
    rf24.stopListening();

    // Start serial communication
    Serial.begin(115200);
}

void loop() {
    // Read potentiometer values
    test[0]= analogRead(hand);
    test[1]= analogRead(potWrist);
    test[2]= analogRead(potElbow);
    test[3]= analogRead(potShoulder);
    test[4]= analogRead(potBase);

    // Check hand value
    if(test[0] > 100){
        test[0] = 1;
    }

    // Read joystick values
    xval = analogRead(xPin);
    yval = analogRead(yPin);
    int zval = analogRead(zPin);
    test[5]= xval;
    test[6]= yval;

    // Send data
    rf24.write(&test, sizeof(test));

    // Print potentiometer values
    Serial.println("Base:" + String(test[4])+" Shoulder:"+String(test[3])+" Elbow:"+String(test[2])+" Wrist:"+String(test[1])+" Hand:"+String(test[0]));

    // Determine joystick direction
    String dir = "";
    if (xval > 800 && yval < 600 && yval > 400) {
        dir = "forward";
    } else if (xval > 800 && yval > 800) {
        dir = "forward right";
    } else if (xval > 800 && yval < 350) {
        dir = "forward left";
    } else if (xval < 200 && yval > 800) {
        dir = "backward right";
    } else if (xval < 200 && yval < 350) {
        dir = "backward left";
    } else if (xval < 200 && yval < 800) {
        dir = "backward";
    } else if (xval > 350 && xval < 600 && yval < 150) {
        dir = "left";
    } else if (xval > 350 && xval < 800 && yval > 800) {
        dir = "right";
    } else {
        dir = "stop";
    }

    // Print joystick values and direction
    char buf[100];
    sprintf(buf, "VRx=%d, VRy=%d, Direction=%s", xval, yval, dir.c_str());
    Serial.println(buf);
}