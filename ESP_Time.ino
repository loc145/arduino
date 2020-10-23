#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include<SoftwareSerial.h>
#include <time.h>

FirebaseData dulieufirebase;
SoftwareSerial espSerial(13, 15);  //(RX, TX);

const char* ssid = "Samsung";
const char* password = "77777777";
int do_am, do_am_ed;
int timezone = 7 * 3600;
int dst = 0;
String updated_time;
///////////////////
const int vout = 0;                              
//////////////////

                                 
void setup(){
  Serial.begin(9600);
  espSerial.begin(9600);
  Serial.println("Bat dau ket noi WiFi");
  Serial.print("Loading");
  WiFi.begin(ssid,password);
  while(!(WiFi.status() == WL_CONNECTED)){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nKet noi thanh cong");
  Serial.print("Connected to this Wifi with IP: ");
  Serial.println(WiFi.localIP());

  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");


  while(time(nullptr)<100){
     Serial.print("*");
     delay(1000);
  }
  Serial.println("\nTime response....OK"); 
  
  Firebase.begin("loczing9c.firebaseio.com","olHKMSOaPG7ac2DHIneUt5d8CClPvoQxmzUlfpxv"); 
  do_am = analogRead(vout);
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop(){
  while(do_am==do_am_ed){
    do_am = analogRead(vout);
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);                    
  digitalWrite(LED_BUILTIN, HIGH);
/***************************Truyền dữ liệu******************************/
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  String _year = String(p_tm->tm_year + 1900);
  String _mon = String(p_tm->tm_mon + 1);
  String _day = String(p_tm->tm_mday);
  String _hour = String(p_tm->tm_hour);
  String _min = String(p_tm->tm_min);
  String _sec = String(p_tm->tm_sec);
  updated_time = _year+"-"+_mon+"-"+_day+" "+_hour+":"+_min+":"+_sec;
  
  
  Firebase.setInt(dulieufirebase, "Humidly/Sensor1/Value", do_am);
  Firebase.setString(dulieufirebase, "Humidly/Sensor1/Updated time", updated_time);
  /*
  if(espSerial.available()){
    do_am = espSerial.read();
  }
  Firebase.setInt(dulieufirebase, "Humidly/Sensor1/Value", do_am);
  */
  
  Serial.print(updated_time);
  Serial.print(" -->The current humid value is: ");
  Serial.println(do_am);
//  Serial.print(p_tm->tm_mday);
//  Serial.print("/");
//  Serial.print(p_tm->tm_mon + 1);
//  Serial.print("/");
//  Serial.print(p_tm->tm_year + 1900);
//  
//  Serial.print(" ");
//  
//  Serial.print(p_tm->tm_hour);
//  Serial.print(":");
//  Serial.print(p_tm->tm_min);
//  Serial.print(":");
//  Serial.println(p_tm->tm_sec);
/***************************Nhận dữ liệu*********************************/
//  Firebase.getString(dulieufirebase, "trang_thai_bom");
//  str = dulieufirebase.stringData();
//  Firebase.getInt(dulieufirebase, "do_am");
//  do_am = dulieufirebase.intData();
//  delay(200);
  do_am_ed = do_am;
  delay(3000);
}
