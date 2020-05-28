#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Blynk.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <math.h>
//(gps->tx,rx)(motor->8digital,4analog)(acce-2 Analog)(communication-tx,rx)
//(10 Analog, 1 txrx)    (esp->gps)(nano->motor,acce)
// return to fixed block or place where started; add return to menu in Blynk
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 4800;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
char auth[]="AuthToken";
char ssid[]="ssid";
char pass[]="123456789";
double destlat=0.000000;
double destlon=0.000000; 
float heading=0.000;
BLYNK_WRITE(V6){
  String s=param.asStr();
  destlat=s.toFloat();
  s=param.asStr();
  destlon=s.toFloat();
}
BLYNK_WRITE(V4){
  int Y=param[1].asInt();
  if(Y>512)
    Serial.print('e');
  if(Y<512)
    Serial.print('r');
}
BLYNK_WRITE(V3){ //Yaw Control
  int x = param[0].asInt();
  if(x>512)
    Serial.print('w');
  if(x<512)
    Serial.print('q');
}
BLYNK_WRITE(V2){
  switch(param.asInt()){
    case 1:{
      destlat=13.000000;
      destlon=80.000000;  
    }
    case 2:{
      destlat=13.000020;
      destlon=80.000020;  
    }
  }  
}
double distance(double currentlat,double currentlon){
  return 3963.0*1000*1.609344*acos( (sin(currentlat/57.29577951)*sin(destlat/57.29577951)) +( cos(currentlat/57.29577951)*cos(destlat/57.29577951)*cos(destlon/57.29577951-currentlon/57.29577951) ) );  
}
void angle(){
  double latitude;
  double longitude;
  if(ss.available()){
    gps.encode(ss.read());
    latitude=gps.location.lat();
    longitude=gps.location.lng();
  }
  double dellon;
  (destlon>=longitude)?(dellon=destlon-longitude):(dellon=longitude-destlon);
  double x=cos(destlon)*sin(dellon);
  double y=cos(latitude)*sin(destlat)-sin(latitude)*cos(destlat)*cos(dellon);
  heading=atan2(x,y);
}
void adjust(int h,char c){
  if(c=='t'){
    Serial.print(c);
    Serial.print(h);    
  }
  else
    Serial.print(c);
}
BLYNK_WRITE(V1){
  switch(param.asInt()) {
    case 1:{      //Manual      
      break;
    }
    case 2:{      //Automatic
      angle();
      adjust(heading,'p');
      for(;;){
        double currentlat=0.000000;
        double currentlon=0.000000;
        if(ss.available()){
          gps.encode(ss.read());
          currentlat=gps.location.lat();
          currentlon=gps.location.lng();
        }
        if(distance(currentlat,currentlon)<1){
          Blynk.virtualWrite(V5,"www.google.com");
          Blynk.virtualWrite(V6,"www.google.com");
          break;
        }
        else{
          adjust(heading,'t');
          adjust(heading,'f');
        }

      } 
      adjust(heading,'l');      
      break;
    }
  }
}
void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Blynk.begin(auth,ssid,pass);
  
}
void loop() {
  Blynk.run();
}
