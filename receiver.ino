#include <SPI.h>
#include "RF24.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
 


#define MIN_PULSE_WIDTH       650
#define MAX_PULSE_WIDTH       2350
#define FREQUENCY             50
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

RF24 rf24(7, 8); // CE腳, CSN腳

const byte addr[] = "2Node";
const byte pipe = 1;  // 指定通道編號
int hand = 3;
int wrist = 8;
int elbow = 9;                           //Assign Motors to pins on Servo Driver Board
int shoulder = 10;
int base = 11;

const int In1_A = 2;
const int In2_A = 3;
const int In1_B = 4;
const int In2_B = 5;

void setup() {
  delay(5000);                            // <-- So I have time to get controller to starting position
  
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  pwm.setPWM(3, 0, 90);                  //Set Gripper to 90 degrees (Close Gripper)
  
  pinMode(In1_A, OUTPUT);
  pinMode(In2_A, OUTPUT);
  pinMode(In1_B, OUTPUT);
  pinMode(In2_B, OUTPUT);

  pinMode(9,INPUT_PULLUP);
  //pinMode(8,INPUT_PULLUP);
  Serial.begin(9600);
  rf24.begin();
  rf24.setChannel(83);  // 設定頻道編號
  rf24.setPALevel(RF24_PA_MIN);
  rf24.setDataRate(RF24_250KBPS);
  rf24.openReadingPipe(pipe, addr);  // 開啟通道和位址
  rf24.startListening();  // 開始監聽無線廣播

  

  Serial.println("nRF24L01 ready!"); 
}

void moveMotor(int controlIn, int motorOut)
{
  int pulse_wide, pulse_width, potVal;
  
  potVal = controlIn;                                                   //Read value of Potentiometer
  //Serial.println(potVal);
  if (motorOut == 8){
    //Serial.println('a');
    pulse_wide = map(potVal, 800,240 ,MAX_PULSE_WIDTH, MIN_PULSE_WIDTH);
  }
  else{
    pulse_wide = map(potVal, 800, 240, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);                                                                                                                                                                                                                         
  }
  //pulse_wide = map(potVal, 800, 240, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);                //Map Po tentiometer position to Motor
  //Serial.println(pulse_width);
  pwm.setPWM(motorOut, 0, pulse_width);
 
}

void loop() {
  // pwm.setPWM(hand,0,240);
  // delay(1000);
  // pwm.setPWM(hand,0,90);
  // delay(1000);
  //for (int i = 400; i <= 500; i++) {
    //moveMotor(i,elbow);
  //}
  //for (int j = 500; j >= 400; j--) {
  //  moveMotor(j,elbow);
  //}
  if (rf24.available(&pipe)) {
    int CONTROL[7] ;
    
    rf24.read(&CONTROL, sizeof(CONTROL));
    //analogWrite(wrist,CONTROL[0]/3.9);
    Serial.print("hand:"+String(CONTROL[0])+" wrist:"+String(CONTROL[1])+" elbow:"+String(CONTROL[2])+" shoulder:"+String(CONTROL[3])+" base:"+String(CONTROL[4]) + " "); // 顯示訊息內容 
    
    moveMotor(CONTROL[1], wrist);
  
    moveMotor(CONTROL[2], elbow);
                                                            //Assign Motors to corresponding Potentiometers
    moveMotor(CONTROL[3], shoulder);
  
    moveMotor(CONTROL[4], base);
  String dir = "";  // 用來儲存方向的變數
  int xval = CONTROL[5];
  int yval = CONTROL[6];
  if (xval > 800 && yval < 600 && yval > 400) {  // forward

    digitalWrite(In1_A, HIGH);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, HIGH);
    dir = "forward";
  } else if (xval > 800 && yval > 800) {  // forward right
    digitalWrite(In1_A, HIGH);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, LOW);
    dir = "forward right";
  } else if (xval > 800 && yval < 350) {  // forward left
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, HIGH);
    dir = "forward left";
  } else if (xval < 200 && yval > 800) {  // backward right
    dir = "backward right";
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, HIGH);
    digitalWrite(In2_B, LOW);
  } else if (xval < 200 && yval < 350) {  // backward left
    dir = "backward left";
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, HIGH);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, LOW);
  } else if (xval < 200 && yval < 800) {  // backward
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, HIGH);
    digitalWrite(In1_B, HIGH);
    digitalWrite(In2_B, LOW);
    dir = "backward";
  } else if (xval > 350 && xval < 600 && yval < 150) {  // left
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, HIGH);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, HIGH);
    dir = "left";
  } else if (xval > 350 && xval < 800 && yval > 800) {  // right
    digitalWrite(In1_A, HIGH);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, HIGH);
    digitalWrite(In2_B, LOW);
    dir = "right";
  } else {  // stop
    digitalWrite(In1_A, LOW);
    digitalWrite(In2_A, LOW);
    digitalWrite(In1_B, LOW);
    digitalWrite(In2_B, LOW);
    dir = "stop";
  }
  Serial.print(" xval:"+String(CONTROL[5])+" yval:"+String(CONTROL[6])+ " ");
  Serial.println(dir);
  // delay(50);
    int pushButton = CONTROL[0];
    if(pushButton == LOW)
    {
     pwm.setPWM(hand, 0, 90);                             //Keep Gripper closed when button is not pressed
     //Serial.println("Grab");
    }
    else
    {
     pwm.setPWM(hand, 0, 200);                              //Open Gripper when button is pressed
     Serial.println("Release");
    }
  }
}
