#include "CTBot.h"
#include <WiFiUdp.h>

CTBot myBot;
String ssid = "your_ssid";
String pass = "your_pass";
String token = "telegram_bot_token";
const int id = 123;

const int analog_ip = A0;
#include <NTPClient.h>

const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int minute_prev = -1;
int pump = 13;
bool pump_state = LOW;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting TelegramBot...");
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);

  pinMode(pump, OUTPUT);
   
  if (myBot.testConnection()) {
    Serial.println("Connected");
  } else {
    Serial.println("Not Connected");
  }

  myBot.sendMessage(id, "Hello, I'm Tombolotutu Garden\nType 'on' to turn on pump\nType 'off to turn it off");
  Serial.println("Message Sent");

  timeClient.begin();  
}

void loop() {
  TBMessage msg;
  
  int inputVal = analogRead (analog_ip); // Analog Values 0 to 1023
  int humidity = map(inputVal, 800, 300, 0, 100);

  timeClient.update();
  String day = daysOfTheWeek[timeClient.getDay()];
  int hour = (timeClient.getHours()+7) % 24;
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  String message = "Time: ";
  message += day;
  message += String(", ");
  message += String(hour);
  message += String(":");
  message += String(minute);
  message += String(":");
  message += String(second);
  
  message += String("\nSoil Humidity: ");
  message += String(humidity);
  message += String("%");
  
  if (humidity < 40){
    if (minute - minute_prev > 30 || minute - minute_prev < 0){
      minute_prev = minute - 1;
      message += String("\nLow Humidity! Please water your plant! ");
      myBot.sendMessage(id, message);
      delay(500);
    }
  }
  
  if (myBot.getNewMessage(msg)) {
    if(msg.sender.id == id){
      if (msg.text.equalsIgnoreCase("on")) {
        pump_state = LOW;
        digitalWrite(pump, pump_state);
        message += String("\nThe pump is ON");
        Serial.println(message);
        myBot.sendMessage(id, message);
      }
  
      else if (msg.text.equalsIgnoreCase("off")) {
        digitalWrite(pump, HIGH);
        message += String("\nThe pump is OFF");
        Serial.println(message);
        myBot.sendMessage(id, message);
      }
      else{
        myBot.sendMessage(msg.sender.id, "Message is not recognized\nType 'On' to turn pump on\nType 'Off to turn it off");
      }
    }
  }
}
