/*                     myGarden
 *            Created by agnath18@gmail.com
 *  GPIO PINS
 *  PUMP               : D1
 *  BUZZER             : D2
 *  Water Level Sensor : D5
 *  Status LED         : D6
 *  NTP OFFSET         : 19800
 * 
 *            MONITOR
 *  Current_Level : Current Water Level
 *  Duration : Duration for activating PUMP
 *  Last_Status : Last Successful Watering Time
 *  Tank_Status : Water Level Of Tank ("Normal" OR "Low Level")
 *  Threshold_Level : Minimum Water Level Required
 *  
 *           SCHEDULE
 *  Time1 : Set Watering Time1
 *  Time2 : Set Watering Time2
 *  
 *             CMD
 *  CMD :  "restart" For Reset.
 */

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#define WIFI_SSID "X18"
#define WIFI_PASSWORD "rty8p20wb@x18"
#define FIREBASE_HOST "covidquiz23.firebaseio.com"
#define FIREBASE_AUTH "aT6z8TYb7HtRUTp1a0kTmsatC7bOzsb6ydJ66jVy"
#define sensorPin A0

FirebaseData firebaseData;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800,60000);

const int pump_r = D1;
const int buzzer = D2;
const int w_level = D5;
const int led_stat = D6;

String cmd;
String current_time = "00:00";
String Time1 = "06:00";
String Time2 = "18:00";
int w_count = 1;
int duration = 30000;
int level;
int t_level = 100;

int Start_pump() {
  Serial.println("Watering Started");
  digitalWrite(pump_r,HIGH);
  String Stat = "Watering Successful at : " + current_time;
  if(Firebase.setString(firebaseData,"/myGarden/Monitor/Last_Status",Stat))
  {
    //Success
     Serial.println("Set Last Status Success");

  }else{
    //Failed

    Serial.print("Error in updating last_status");
    Serial.println(firebaseData.errorReason());
  }
  
  if(Firebase.getInt(firebaseData, "/myGarden/Monitor/Watering_Count"))
  {
    //Success
    Serial.println("Recieved count value");
    w_count = firebaseData.intData();
    w_count +=1;
    Firebase.setInt(firebaseData,"/myGarden/Monitor/Watering_Count",w_count);
    Serial.println("Incremented count");

  }else{
    //Failed
    Serial.println(firebaseData.errorReason());
    Firebase.setInt(firebaseData,"/myGarden/Monitor/Watering_Count",w_count);
  }
}

int water_level()
{
  tone(buzzer, 4000, 100);
  if(Firebase.getInt(firebaseData, "/myGarden/Monitor/Threshold_Level"))
  {
    //Success
    t_level = firebaseData.intData();
    Serial.println("Recieved Threshold Level");

  }else{
    //Failed
    Serial.println(firebaseData.errorReason());
    Firebase.setInt(firebaseData,"/myGarden/Monitor/Threshold_Level",t_level);
  }
  digitalWrite(w_level,HIGH);
  delay(5000);
  level = analogRead(sensorPin);
  if(level<t_level)
  Firebase.setString(firebaseData,"/myGarden/Monitor/Tank_Status","Low Level");
  else
  Firebase.setString(firebaseData,"/myGarden/Monitor/Tank_Status","Normal");
  Firebase.setInt(firebaseData,"/myGarden/Monitor/Current_Level",level);
  digitalWrite(w_level,LOW);
}


void setup() {
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  timeClient.begin();

  pinMode(pump_r, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(w_level, OUTPUT);
  pinMode(led_stat, OUTPUT);
  

  digitalWrite(pump_r,LOW);
  digitalWrite(w_level,LOW);
  digitalWrite(led_stat,LOW);
  delay(5000);
  Firebase.setString(firebaseData,"/myGarden/Monitor/Last_Status","idle");
  Firebase.setString(firebaseData,"/myGarden/cmd","Online");
}

void loop() {
  
  tone(buzzer, 4000, 100);
  timeClient.update();
  current_time = timeClient.getFormattedTime();
  current_time.remove(5,3);

  if(Firebase.getString(firebaseData, "/myGarden/Schedule/Time1"))
  {
    //Success
    Serial.println("Updating Database");
    Time1 = firebaseData.stringData();
    Serial.println("Recieved Schedule Time1");

  }else{
    //Failed
    Serial.println(firebaseData.errorReason());
    Firebase.setString(firebaseData,"/myGarden/Schedule/Time1",Time1);
  }

    if(Firebase.getString(firebaseData, "/myGarden/Schedule/Time2"))
  {
    //Success
    Time2 = firebaseData.stringData();
    Serial.println("Recieved Schedule Time2");

  }else{
    //Failed
    Serial.println(firebaseData.errorReason());
    Firebase.setString(firebaseData,"/myGarden/Schedule/Time2",Time2);
  }

  if(Firebase.getInt(firebaseData, "/myGarden/Monitor/Duration"))
  {
    //Success
    duration = firebaseData.intData();
    Serial.println("Recieved Duration");

  }else{
    //Failed
    Serial.println(firebaseData.errorReason());
    Firebase.setInt(firebaseData,"/myGarden/Monitor/Duration",duration);
  }

    if(Firebase.getString(firebaseData, "/myGarden/cmd"))
    {
    //Success
      cmd = firebaseData.stringData();
      Serial.println("CMD Recieved");
      if(cmd=="restart") {
      Serial.println("Reset..");
      ESP.restart(); }}

  if(current_time == Time1 || current_time == Time2){

    Serial.println("Time Match");
    tone(buzzer, 2000, 10000);
    digitalWrite(led_stat, HIGH);
    Start_pump();
    delay(duration);
    digitalWrite(pump_r,LOW);
    digitalWrite(led_stat, LOW);
    Serial.println("Watering Successful");
    Serial.println("Delay For 60 Seconds");
    delay(60000); }
    
    water_level();
    Serial.println("Configuration Updation In Next 10Sec");
    delay(10000); 
    
}
