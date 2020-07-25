#include <ESP8266WiFi.h>
 
const char* ssid = "Azhari Home";
const char* password = "papamama";
 
int ledPin = 5; // GPIO5
int statusPin = 13;

WiFiServer server(80);
const int analog_ip = A0;

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
 
  int inputVal = analogRead (analog_ip); // Analog Values 0 to 1023
  int humidity = map(inputVal, 800, 350, 0, 100);
  
  // Match the request
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
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

  client.print("Humidity: ");
  client.print(humidity);
  client.print("%");
  client.println("<br>");

  client.print("Led pin is now: ");
 
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
