#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <config.h>

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;
int dataNumber = 0;             // new for this version
int toggle = 0;
String toggleState = "Low";

Servo servo;

void setup() {
  Serial.begin(9600);

  // pepare servo - set initial angle to 15
  servo.attach(2); //D4
  servo.write(80);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();  
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
   
    if (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewNumber() {
    if (newData == true) {
        dataNumber = 0;             // new for this version
        dataNumber = atoi(receivedChars);   // new for this version
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        Serial.print("Data as Number ... ");    // new for this version
        Serial.println(dataNumber);     // new for this version
        servo.write(dataNumber);
        newData = false;
    }
}

int pos = 0;
void loop() {
  WiFiClient client = server.available(); 
 if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // toggle mechanism
            if (header.indexOf("GET /5/toggle") >= 0) {
              if (toggle == 0) {
                servo.write(15);
                toggle = 1;
                toggleState = "Low";
              } else {
                toggle = 0;
                servo.write(80);
                toggleState = "High";
              }                        
            }

            // small jerk
            if (header.indexOf("GET /5/40") >= 0) {
              Serial.println("Small feeding");
              outputState = "on";
              servo.write(40);
              delay(700);
              servo.write(80);
              outputState = "off";
            } 
            // small jerk - long delay
            if (header.indexOf("GET /5/40l") >= 0) {
              Serial.println("Small feeding");
              outputState = "on";
              servo.write(40);
              delay(1500);
              servo.write(80);
              outputState = "off";
            } 
            // large jerk
            if (header.indexOf("GET /5/15") >= 0) {
              Serial.println("Large feeding");
              outputState = "on";
              servo.write(15);
              delay(700);
              servo.write(80);
              outputState = "off";
            } 
            // large jerk - long delay
            if (header.indexOf("GET /5/15l") >= 0) {
              Serial.println("Large feeding long");
              outputState = "on";
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);
              servo.write(15);
              delay(100);
              servo.write(80);
              delay(100);

              outputState = "off";
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Feeder</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>State " + toggleState + "</p>");
            // If the output5State is off, it displays the ON button       
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }  
  recvWithEndMarker();
  showNewNumber();
}
