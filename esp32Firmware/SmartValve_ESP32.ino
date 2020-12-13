#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

WebServer Server; // Create a webServer object that listens for HTTP request on port 80
AutoConnect Portal(Server); //Captive portal for provisioning

// function prototypes for HTTP handlers
void handleRoot();              
void handleRequest();
void handleNotFound();

//GPIO definition for controlling the smart valve
int pin[9] = {32,33,25,26,27,14,12,13,16};

void setup(void){
  delay(100);

  //Initialize the GPIOs as output pins and 
  //write inital output  to 1 (reverse logic due to BJT gate drive)
  for(int i=0;i<9;i++){
    pinMode(pin[i], OUTPUT);
    digitalWrite(pin[i], 1);
  }

  //Sample implementation for PWM, current not in use
  //Follows normal logic
  analogWriteRange(100);
  analogWrite(16, 0);

 // Start the Serial communication to send debug messages to PC
  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');

  // Call the 'handleRoot' function when a client requests URI "/"
  Server.on("/", HTTP_GET, handleRoot); 
         
  // Call the 'handRequest' function when a POST request is made to URI "/command"
  Server.on("/command", HTTP_POST, handleRequest); 
  Server.on("/inline", []() {
    Server.send(200, "text/plain", "This works as well");
  });

  // When a client requests an unknown URI 
  Server.onNotFound(handleNotFound); 

  //Begin as a captive portal in case not provisioning or else
  //Acts as the valve control server
  if(Portal.begin()){ 
    Serial.println("HTTP Server started "+ WiFi.localIP().toString());
  }
  return;
}

void loop(void){
  Portal.handleClient(); // Listen for HTTP requests from clients
}

void handleRoot() {  // When URI / is requested, send a standard web page 
  Server.send(200, "text/html", "Wi-Fi SmartValve Connected");
  return;
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += Server.uri();
  message += "\nMethod: ";
  message += (Server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += Server.args();
  message += "\n";
  for (uint8_t i = 0; i < Server.args(); i++) {
    message += " " + Server.argName(i) + ": " + Server.arg(i) + "\n";
  }
  Server.send(404, "text/plain", message);
}

void handleRequest() { // If a POST request is made to URI /command
  // Validate parameters
  if(!Server.hasArg("pin") || !Server.hasArg("value") || Server.arg("pin") == NULL || Server.arg("value") == NULL) { 
    Server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  
  // Get the parameters: pin and value
  String temp = "";
  temp = Server.arg("pin"); 
  int pin = temp.toInt();
  temp = Server.arg("value"); 
  int value = temp.toInt();

  Serial.println(pin);
  Serial.println(value);
  if (pin >= 12 && pin < 34 && value >= 0 && value <= 100) {
    digitalWrite(pin, value);
    if (pin == 16) {
      analogWrite(pin, value);
    } else {
      digitalWrite(pin, !value);
    }
  }
  Server.send(200, "text/html", "Wi-Fi SmartValve operation success");
  return;
}
