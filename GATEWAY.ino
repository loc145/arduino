#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(13, 15);  //(RX, TX);

const byte M0 = 16;
const byte M1 = 5;

const char* ssid = "Samsung";
const char* password = "77777777";
WiFiClient  client;

//Point[0]: do_am, Point[1]: Point_Set
byte Point1[2];
byte Point2[2];
byte Point3[2];
byte Point4[2];

byte i = 0, capacity = 0;
int statusCode = 0;
int x;
                           
void setup(){
  pinMode(M0, OUTPUT); digitalWrite(M0, LOW);
  pinMode(M1, OUTPUT); digitalWrite(M1, LOW);

  Serial.begin(9600);
  espSerial.begin(9600);
  //Kết nối Wifi
  Serial.println("Bat dau ket noi WiFi");
  Serial.print("Loading");
  WiFi.begin(ssid,password);
  while(!(WiFi.status() == WL_CONNECTED)){
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to this Wifi with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(client);  
}

void loop(){
  Serial.println("Ready!!!");
  readChannelPoints();
  RxTx_vs_STM32();
  writeChannelPoints();
  writeChannelPump();
  RxTx_PumpControl();
}

void readChannelPoints()
{
  //Read Point_Sets
  Point1[1] = ThingSpeak.readIntField(1246638, 5, "PVHH009EBTBDL9UG");  
  Point2[1] = ThingSpeak.readIntField(1246638, 6, "PVHH009EBTBDL9UG");
  Point3[1] = ThingSpeak.readIntField(1246638, 7, "PVHH009EBTBDL9UG");
  Point4[1] = ThingSpeak.readIntField(1246638, 8, "PVHH009EBTBDL9UG");
  // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    Serial.println("Channel Point_Set reading successful.");
    
    Serial.println("Point1[1] = " + String(Point1[1]));
    Serial.println("Point2[1] = " + String(Point2[1]));
    Serial.println("Point3[1] = " + String(Point3[1]));
    Serial.println("Point4[1] = " + String(Point4[1]));
  }
  else{
    Serial.println("Problem reading channel Points. HTTP error code " + String(statusCode)); 
  }
  delay(5000);
}
void RxTx_vs_STM32()
{
    //Diem 1
    espSerial.print("3" + String((char)Point1[1]));
    ////////////////////////////////////
    delay(500);                 ////////
    espSerial.print("50");      //////// 
    ////////////////////////////////////
    if (espSerial.available()) Point1[0] = espSerial.read();
    Serial.println("Do am Point 1: " + String(Point1[0]) + "%");
    delay(5000);

    //Diem 2
    espSerial.print("2" + String((char)Point2[1]));
    ////////////////////////////////////
    delay(500);                 ////////
    espSerial.print("50");      //////// 
    ////////////////////////////////////
    if (espSerial.available()) Point2[0] = espSerial.read();
    Serial.println("Do am Point 2: " + String(Point2[0]) + "%");
    delay(5000);

    //Diem 3
    espSerial.print("1" + String((char)Point3[1]));
    ////////////////////////////////////
    delay(500);                 ////////
    espSerial.print("50");      //////// 
    ////////////////////////////////////
    if(espSerial.available()) Point3[0] = espSerial.read();
    Serial.println("Do am Point 3: " + String(Point3[0]) + "%");
    delay(5000);
    
    //Diem 4
    ////////////////////////////////////
    espSerial.print("50");     /////////
    delay(500);                /////////
    ////////////////////////////////////
    espSerial.print("4" + String((char)Point4[1]));
    ////////////////////////////////////
    delay(500);                 ////////
    espSerial.print("50");      ////////
    ////////////////////////////////////
    if (espSerial.available()) Point4[0] = espSerial.read();
    Serial.println("Do am Point 4: " + String(Point4[0]) + "%");
    delay(5000);      
}
void writeChannelPoints()
{
  ThingSpeak.setField(1, Point1[0]);
  ThingSpeak.setField(2, Point2[0]);
  ThingSpeak.setField(3, Point3[0]);
  ThingSpeak.setField(4, Point4[0]);
  ThingSpeak.setField(5, Point1[1]);
  ThingSpeak.setField(6, Point2[1]);
  ThingSpeak.setField(7, Point3[1]);
  ThingSpeak.setField(8, Point4[1]);
  
  x = ThingSpeak.writeFields(1246638, "2XINH7EJNQ438STB");
  if(x == 200){
    Serial.println("Channel Points update successful.");
  }
  else{
    Serial.println("Problem updating channel Points. HTTP error code " + String(x));
  }
  delay(5000);
}

void writeChannelPump()
{
  if(3 <= Point1[0] < Point1[1]) i++;
  if(3 <= Point2[0] < Point2[1]) i++;
  if(3 <= Point3[0] < Point3[1]) i++;
  if(3 <= Point4[0] < Point4[1]) i++;
  capacity = 25*i;
  
  x = ThingSpeak.writeField(1246624, 1, capacity, "8C9G1R545YCXVK5S");
  if(x == 200){
    Serial.println("Channel Pump update successful.");
  }
  else{
    Serial.println("Problem updating channel Pump. HTTP error code " + String(x));
  }
  i = 0;
  delay(5000);
}
void RxTx_PumpControl()
{
  espSerial.print("P" + String((char)capacity));
  delay(5000);
}
