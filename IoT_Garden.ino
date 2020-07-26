#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
 
const char* ssid = "Azhari Home";
const char* password = "papamama";

const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

int ledPin = 5; // GPIO5
int statusPin = 13;

WiFiServer server(80);
const int analog_ip = A0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(115200);
  delay(10);
 
  pinMode(ledPin, OUTPUT);
  pinMode(statusPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);

  Serial.print("not connected: ");
  Serial.println(WiFi.status());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(statusPin, HIGH);
  delay(1000);
  digitalWrite(statusPin, LOW);
  delay(1000);
  digitalWrite(statusPin, HIGH);
  delay(1000);
  digitalWrite(statusPin, LOW);
  delay(1000);
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  timeClient.begin();  
}
 
void loop() {
  // Check if a client has connected
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(statusPin, HIGH);
  }
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  timeClient.update();
  String day = daysOfTheWeek[timeClient.getDay()];
  int hour = timeClient.getHours()+7;
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();
  
  int inputVal = analogRead (analog_ip); // Analog Values 0 to 1023
  int humidity = map(inputVal, 800, 350, 0, 100);
  
  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    value = HIGH;
    digitalWrite(ledPin, not(value));
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    value = LOW;
    digitalWrite(ledPin, not(value));
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println(
  "<head>"
    "<title>IoT Garden</title>"
    "<style>"
      ".button {"
        "background-color:white;"
        "border: 2px solid red;"
        "color: red;"
        "padding: 15px 32px;"
        "text-align: center;"
        "text-decoration: none;"
        "display: inline-block;"
        "font-size: 16px;"
        "margin: 4px 2px;"
        "cursor: pointer;"
        "border-radius: 8px;"
        "width: 150px;"
      "}"
    "</style>"
  "</head>");

  client.print("Updated at: ");
  client.print(day);
  client.print(", ");
  client.print(hour);
  client.print(":");
  client.print(minute);
  client.print(":");
  client.println(second);
  client.println("<br>");
  
  client.print("Humidity: ");
  client.print(humidity);
  client.print("%");
  client.println("<br>");

  client.print("Water pump is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button class='button'>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button class='button'>Turn Off </button></a><br />");  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
