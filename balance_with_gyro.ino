#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define OUTPUT_READABLE_ACCELGYRO
#define LED_PIN 13

const int In1_right = 3;
const int In2_right = 4;
const int In3_right = 5;      
const int In4_right = 6;       
const int In1_left = 8;
const int In2_left = 9;
const int In3_left = 10;      
const int In4_left = 11;

MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
bool blinkState = false;

void setup() {
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(38400);
    pinMode(In1_right, OUTPUT);
    pinMode(In2_right, OUTPUT);
    pinMode(In3_right, OUTPUT);
    pinMode(In4_right, OUTPUT);    
    pinMode(In1_left, OUTPUT);
    pinMode(In2_left, OUTPUT);
    pinMode(In3_left, OUTPUT);
    pinMode(In4_left, OUTPUT);  

    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if(ay >= 2000){
      Serial.print("Balance toward right\t");
      balanceTowardRight();
    }else if(ay <= -2000){
      Serial.print("Balance toward left\t");
      balanceTowardLeft();
    }else{
      Serial.print("Nothing happen\t");
      motorReset();
    }

    #ifdef OUTPUT_READABLE_ACCELGYRO
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
    #endif

    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}

void motorReset(){
  digitalWrite(In1_right, LOW);
  digitalWrite(In2_right, LOW); 
  digitalWrite(In3_right, LOW);
  digitalWrite(In4_right, LOW);
  digitalWrite(In1_left, LOW);
  digitalWrite(In2_left, LOW);
  digitalWrite(In3_left, LOW);
  digitalWrite(In4_left, LOW);
}

void balanceTowardRight(){
  flowInRight();
  flowOutLeft();
}

void balanceTowardLeft(){
  flowInLeft();
  flowOutRight();
}

void flowInRight(){
  digitalWrite(In1_right, HIGH);
  digitalWrite(In2_right, LOW);
}

void flowOutRight(){
  digitalWrite(In3_right, HIGH);
  digitalWrite(In4_right, LOW);
}

void flowInLeft(){
  digitalWrite(In1_left, HIGH);
  digitalWrite(In2_left, LOW);
}

void flowOutLeft(){
  digitalWrite(In3_left, HIGH);
  digitalWrite(In4_left, LOW);
}