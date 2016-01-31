/*
 * ESP 8266 Garage Door Opener Server
 * Mark Bundgus 2016
 */

// todo: add door position sensing
// todo: add status page
// todo: add state machine for explicit close/open commands
// todo: add mDNS
// todo: change default DNS name
// todo: add SSID/PASSWORD programability via soft AP or UART
// todo: add reset button to return to 'factory state'
// todo: enable command authentication and session level encryption
// todo: add admin interface
// todo: add automatic push notifications (twitter?)

#include <ESP8266WiFi.h>

#define SSID "3C9NH"
#define PASSWORD "enter WIFI password here"
#define PORT 80
#define OPENERTRIGGER 4

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(PORT);

void triggeropener(){
    digitalWrite(OPENERTRIGGER, 1);
    delay(500);
    digitalWrite(OPENERTRIGGER, 0);
}

void setup() {
  // prepare GPIO2
  //pinMode(2, INPUT_PULLUP);
  pinMode(OPENERTRIGGER, OUTPUT);
  digitalWrite(OPENERTRIGGER, 0);
  
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
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
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  String state;
  if (req.indexOf("/open") != -1){
    triggeropener();
    state = "opened";
  }
  else if (req.indexOf("/close") != -1){
    triggeropener();
    state = "closed";
  }
  else {
    //client.stop();
    //return;
    yield();
  }

  Serial.println(state);

  // Set GPIO2 according to the request
  //digitalWrite(2, val);
  
  client.flush();

  // for invalid requests like closing a door that is alredy closed, use the 409 Conflict response

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\n\r\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
