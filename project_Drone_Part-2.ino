#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Servo.h>
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
int inl11=1;
int inl12=2;
int inl21=5;
int inl22=6;
int inr11=7;
int inr12=8;
int inr21=9;
int inr22=10;
int enl1=18;
int enl2=19;
int enr1=20;
int enr2=21;
int trig=11;
int echo=12;
float sensorval(){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  int duration = pulseIn(echo, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}
void yawRight(int flag){
  if(flag==1){
    analogWrite(enl1,240);
    analogWrite(enr1,255);
    analogWrite(enl2,255);
    analogWrite(enr2,240);
  }
  else if(flag==0){
    analogWrite(enl1,255);
    analogWrite(enr1,245);
    analogWrite(enl2,245);
    analogWrite(enr2,255);
  }
  digitalWrite(inl11,HIGH);
  digitalWrite(inl12,LOW);
  digitalWrite(inr21,HIGH);
  digitalWrite(inr22,LOW);  
  digitalWrite(inr11,LOW);
  digitalWrite(inr12,HIGH);
  digitalWrite(inl21,LOW);
  digitalWrite(inl22,HIGH);  
}
void moveForward(int flag){
  if(flag==1){
    if(sensorval>100){
      analogWrite(enl1,245);
      analogWrite(enr1,245);
      analogWrite(enl2,255);
      analogWrite(enr2,255);
    }
  }
  else if(flag==0){
    analogWrite(enl1,255);
    analogWrite(enr1,255);
    analogWrite(enl2,245);
    analogWrite(enr2,245);
  }
  else if(flag==2){
    analogWrite(enl1,255);
    analogWrite(enr1,255);
    analogWrite(enl2,255);
    analogWrite(enr2,255);
  }
  digitalWrite(inl11,HIGH);
  digitalWrite(inl12,LOW);
  digitalWrite(inr21,HIGH);
  digitalWrite(inr22,LOW);  
  digitalWrite(inr11,LOW);
  digitalWrite(inr12,HIGH);
  digitalWrite(inl21,LOW);
  digitalWrite(inl22,HIGH);
  delay(2000);
}
void land(){
  for(int i=255;i>=0;i=i-10){
    analogWrite(enl1,i);
    analogWrite(enr1,i);
    analogWrite(enl2,i);
    analogWrite(enr2,i);
  }
}
float currentHeading(){
  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  float declinationAngle = 0.0221075;
  heading+=declinationAngle;
  if(heading < 0)
    heading += 2*PI;
  if(heading > 2*PI)
    heading -= 2*PI;
  heading=heading * 180/M_PI; 
  return heading;
}
void setup() {
  Serial.begin(9600);
  pinMode(enl1,OUTPUT);
  pinMode(enr1,OUTPUT);
  pinMode(enl2,OUTPUT);
  pinMode(enr2,OUTPUT);
  pinMode(inl11,OUTPUT);
  pinMode(inl12,OUTPUT);
  pinMode(inl21,OUTPUT);
  pinMode(inl22,OUTPUT);
  pinMode(inr11,OUTPUT);
  pinMode(inr12,OUTPUT);
  pinMode(inr21,OUTPUT);
  pinMode(inr22,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(trig,OUTPUT);
}

void loop() {
  while(Serial.available()){
    char c=Serial.read();
    if(c=='l')
      land();
    else if(c=='p')
      moveForward(2);
    else if(c=='f')
      moveForward(1);
    else if(c=='t'){
      String s=Serial.readString();
      int h=s.toInt();
      int ch=currentHeading();
      while((h-ch)>3 && (h-ch)<-3){
        ch=currentHeading();
        if((ch-h)<-3)
          yawRight(1);
        else if((ch-h)>3)
          yawRight(0);            
      }      
    }
    else if(c=='q')   //yawleft
      yawRight(0);
    else if(c=='w')   //yawright
      yawRight(1);
    else if(c=='e')   //move Forward
      moveForward(1);
    else if(c=='r')   //move backward
      moveForward(0);
  }
}
