#include <SPI.h>
#include <NativeEthernet.h>
#include <Bounce2.h>
#include <DYPlayerArduino.h>


int phoneID = 0;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED + phoneID + 11};
IPAddress ip(192, 168, 1, 222);  ////TEENSY IP

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 165);  ////TOUCH PC IP

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
EthernetClient client;

DY::Player player(&Serial1);

Bounce service1 = Bounce();
Bounce service2 = Bounce();
Bounce service3 = Bounce();

Bounce remote1 = Bounce();
Bounce remote2 = Bounce();
Bounce remote3 = Bounce();
Bounce remote4 = Bounce();

const uint8_t relayPins[] = { 6, 8, 10, 7, 9, 11};
uint8_t relayState[6];

const uint8_t buttonPins[] = { 14, 15, 16 };
uint8_t buttonState[3];

const uint8_t remotePins[] = { 2, 3, 4, 5 };
uint8_t remoteState[6];

char messageArray[6];

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)

byte ledPin = 13;

char packetBuffer[17];

String incomingMessage;

int port = 8001;

// how much data we expect before a newline
const unsigned int MAX_INPUT = 100;

void (*resetFunc)(void) = 0;  //declare reset function

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  player.begin();
  player.setVolume(25);///100%
  player.setCycleMode(DY::PlayMode::OneOff); // Play all and repeat.

  pinMode(13, OUTPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(relayPins[i], OUTPUT);
    //Serial.println("Setting pin " + String(i));
  }

service1.attach( buttonPins[0] ,  INPUT_PULLUP );
service2.attach( buttonPins[1] ,  INPUT_PULLUP );
service3.attach( buttonPins[2] ,  INPUT_PULLUP );

service1.interval(5); // interval in ms
service2.interval(5); // interval in ms
service3.interval(5); // interval in ms

remote1.attach( remotePins[0] ,  INPUT_PULLUP );
remote2.attach( remotePins[1] ,  INPUT_PULLUP );
remote3.attach( remotePins[2] ,  INPUT_PULLUP );
remote4.attach( remotePins[3] ,  INPUT_PULLUP );

remote1.interval(5); // interval in ms
remote2.interval(5); // interval in ms
remote3.interval(5); // interval in ms
remote4.interval(5);


  /////////////////////FLASH RELAYS AT STARTUP

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
    Serial.println("connected on port: " + String(port));
     digitalWrite(13, HIGH);
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    Serial.println("resetting.");
    resetFunc();  /// reset the teensy
  }
}

bool firstLoop = true;

void loop() {
service1.update();
service2.update();
service3.update();  

remote1.update();
remote2.update();
remote3.update();
remote4.update();

if(service1.changed()){
  int serviceInput1 = service1.read();
  if(serviceInput1 == LOW) {
    if (client.connected()) {
        client.print("0: 0");
        Serial.println("0: 0");
      }
  }
}

if(service2.changed()){
  int serviceInput2 = service2.read();
  if(serviceInput2 == LOW) {
    if (client.connected()) {
        client.print("0: 1");
        Serial.println("0: 1");
      }
  }
}

if(service3.changed()){
  int serviceInput3 = service3.read();
  if(serviceInput3 == LOW) {
    if (client.connected()) {
        client.print("0: 2");
        Serial.println("0: 2");
      }
  }
}

if(remote1.changed()){
  int remoteInput1 = remote1.read();
  if(remoteInput1 == LOW) {
    if (client.connected()) {
        client.print("tv1:up");
        Serial.println("tv1:up");
      }
  }
}

if(remote2.changed()){
  int remoteInput2 = remote2.read();
  if(remoteInput2 == LOW) {
    if (client.connected()) {
        client.print("tv1:down");
        Serial.println("tv1:down");
      }
  }
}

if(remote3.changed()){
  int remoteInput3 = remote3.read();
  if(remoteInput3 == LOW) {
    if (client.connected()) {
        client.print("tv2:up");
        Serial.println("tv2:up");
      }
  }
}

if(remote4.changed()){
  int remoteInput4 = remote4.read();
  if(remoteInput4 == LOW) {
    if (client.connected()) {
        client.print("tv2:down");
        Serial.println("tv2:down");
      }
  }
}

  // ///////BARTENDER SERVICE BUTTONS Press
  // for (int i = 0; i < 3; i++) {
  //   if (digitalRead(buttonPins[i]) == 0 && buttonState[i] == 0) {
  //     Serial.println(String(i) + " pressed");     
  //     delay(10);
  //     if (client.connected()) {
  //       client.print("0: " + String(i));
  //       Serial.print("sending message");
  //     }
  //     buttonState[i] = 1;
  //   }
  // }

  // ///////BARTENDER REMOTE BUTTONS Press
  // for (int i = 0; i < 4; i++) {
  //   if (digitalRead(remotePins[i]) == 0 && remoteState[i] == 0) {
  //     Serial.println(String(i) + " pressed");     
  //     delay(10);
  //     if (client.connected()) {
  //       if(i==0){client.print("tv1:up" );}
  //       if(i==1){client.print("tv1:down" );}
  //       if(i==2){client.print("tv2:up" );}
  //       if(i==3){client.print("tv2:down" );}
  //     }
  //     buttonState[i] = 1;
  //   }
  // }


// /////////////Button Release
//   for (int i = 0; i < 3; i++) {
//     if (digitalRead(buttonPins[i]) == 1 && buttonState[i] == 1) {
//       buttonState[i] = 0;
//     }
//   }

  if (relayPins[0] == 1 || relayPins[1] == 1 || relayPins[2] == 1) {
    /////BEEP and flash
    //player.playSpecified(1);
  }


  /////read incoming messages
  while (client.available() > 0) {
    processIncomingByte(client.read());
  }


  // if the server's disconnected, stop the client:
if (!client.connected()) {
  Serial.println("Disconnected. Attempting reconnect...");
  client.stop();
  delay(1000);
  if (client.connect(server, port)) {
    Serial.println("Reconnected!");
  } else {
    Serial.println("Reconnect failed.");
  }
}
}

/////////////////////////////////////////////////////////////////////PROCESS INCOMING MESSAGE
// here to process incoming data after a terminator received
void process_data(const char* data) {
  incomingMessage = String(data);

  Serial.println("FROM TOUCH: " + incomingMessage);

  if (incomingMessage == "beep"){
    player.playSpecified(1);
    player.play();
  }

  if (incomingMessage[0] == '0') {
    Serial.println("updating lights");
    incomingMessage = incomingMessage.remove(0, 3);
    incomingMessage.trim();

    //Serial.println("NEW MESSAGE" + incomingMessage);

    for (int i = 0; i < 6; i++) {

      if (incomingMessage[i] == '1') {
        digitalWrite(relayPins[i], HIGH);
        Serial.println("Setting pin high: " + String(relayPins[i]));
        relayState[i] = 1;
      } else {
        digitalWrite(relayPins[i], LOW);
        relayState[i] = 0;
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
