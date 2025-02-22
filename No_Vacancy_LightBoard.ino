#include <SPI.h>
#include <NativeEthernet.h>
#include <Bounce2.h>
#include <DYPlayerArduino.h>

DY::Player player;

int phoneID = 0;

#define BUTTON_PIN 1
Bounce2::Button button = Bounce2::Button();

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 200 + phoneID);  ////TEENSY IP

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 0, 10);  ////TOUCH PC IP

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
EthernetClient client;


// int service1 = 9;   // Service 1
// int dnd1 = 10;  //DND1
// int service2 = 11;
// int dnd2 = 12;
// int service3 = 13;
// int dnd3 = 14;

const uint8_t relayPins[] = { 2, 3, 4, 5, 6, 7 };
uint8_t relayState[6];
char messageArray[6];

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)

byte ledPin = 13;

char packetBuffer[17];

String incomingMessage;

int port = 7000;

// how much data we expect before a newline
const unsigned int MAX_INPUT = 100;

void (*resetFunc)(void) = 0;  //declare reset function

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(13, OUTPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(relayPins[i], OUTPUT);
    Serial.println("Setting pin " + String(i));
  }


  for (int i = 0; i < 6; i++) {
    digitalWrite(relayPins[i], HIGH);
  }

  delay(2000);

  for (int i = 0; i < 6; i++) {
    digitalWrite(relayPins[i], LOW);
  }
  digitalWrite(13, HIGH);


  // start the Ethernet connection:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);  // do nothing, no point running without Ethernet hardware
    }
  }
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(500);
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    Serial.println("resetting.");
    resetFunc();  /// reset the teensy
  }
}

bool firstLoop = true;

void loop() {
  /////read incoming messages
  while (client.available() > 0) {
    processIncomingByte(client.read());
  }


  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    //delay(1000);
    Serial.println("resetting.");
    resetFunc();  /// reset the teensy
    //Ethernet.begin(mac, ip);
  }
}

/////////////////////////////////////////////////////////////////////PROCESS INCOMING MESSAGE
// here to process incoming data after a terminator received
void process_data(const char* data) {
  incomingMessage = String(data);

  Serial.println("FROM TOUCH: " + incomingMessage);

  if (incomingMessage[0] == '0') {
    Serial.println("updating lights");
    incomingMessage = incomingMessage.remove(0, 3);
    incomingMessage.trim();

    //Serial.println("NEW MESSAGE" + incomingMessage);
    
    for (int i = 0; i < 6; i++) {

      if (incomingMessage[i] == '1') {
        digitalWrite(relayPins[i], HIGH);
        Serial.println("Setting pin high: " + String(relayPins[i]));
      } else {
        digitalWrite(relayPins[i], LOW);
        
      }
      incomingMessage = incomingMessage.remove(i, 2);
    }
  }
}  // end of process_data

static char input_line[MAX_INPUT];
static unsigned int input_pos = 0;

void processIncomingByte(const byte inByte) {
  switch (inByte) {
    case '\n':                    // end of text
      input_line[input_pos] = 0;  // terminating null byte

      // terminator reached! process input_line here ...
      process_data(input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':  // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line[input_pos++] = inByte;
      break;

  }  // end of switch

}  // end of processIncomingByte
