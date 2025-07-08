#include <SPI.h>
#include <NativeEthernet.h>
#include <Bounce2.h>
#include <DYPlayerArduino.h>

int phoneID = 2;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, (200 + phoneID));  ////TEENSY IP

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 165);  ////TOUCH PC IP

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 23 is default for telnet;
// if you're using Processing's ChatServer, use port 10002):
EthernetClient client;

//DY::Player player;
DY::Player player(&Serial1);

////////REMOTE PINS
const uint8_t remotePins[] = { 31, 30, 29, 28 };
Bounce remote1 = Bounce();
Bounce remote2 = Bounce();
Bounce remote3 = Bounce();
Bounce remote4 = Bounce();



/////KEYPAD

#include <Keypad.h>

#include <Wire.h>
#include <Adafruit_GFX.h>

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPins[ROWS] = { 5, 4, 3, 2 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8 };     //connect to the column pinouts of the keypad

int relayPin1 = 9;   // ringer
int relayPin2 = 10;  //led
int phoneSwitch = 11;

bool relay1State = LOW;
bool relay2State = LOW;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store

//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     00000000..
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)


//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

byte ledPin = 13;

char packetBuffer[17];

boolean blink = false;
boolean ledPin_state;

String incomingMessage;

int port = 7000 + phoneID;
int localPort = 50000 + phoneID;

// how much data we expect before a newline
const unsigned int MAX_INPUT = 100;

void (*resetFunc)(void) = 0;  //declare reset function

char message[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  ///buffer for message to Touch
int count = 0;

bool fiveminWarning = false;
bool dialing = false;
bool satanMode = false;

void setup() {

  pinMode(13, OUTPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(phoneSwitch, INPUT_PULLUP);

  remote1.attach(remotePins[0], INPUT_PULLUP);
  remote2.attach(remotePins[1], INPUT_PULLUP);
  remote3.attach(remotePins[2], INPUT_PULLUP);
  remote4.attach(remotePins[3], INPUT_PULLUP);
  remote1.interval(5);  // interval in ms
  remote2.interval(5);  // interval in ms
  remote3.interval(5);  // interval in ms
  remote4.interval(5);

  player.begin();
  player.setVolume(15);

  digitalWrite(13, HIGH);
  keypad.addEventListener(keypadEvent);  // Add an event listener for this keypad

  // Open serial communications and wait for port to open:
  Serial.begin(9600);

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
  if (client.connect(server, port, localPort)) {
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

  remote1.update();
  remote2.update();
  remote3.update();
  remote4.update();

  if (remote1.changed()) {
    int remoteInput1 = remote1.read();
    if (remoteInput1 == LOW) {
      if (client.connected()) {
        client.print("ch:up");
        Serial.println("ch:up");
      }
    }
  }

  if (remote2.changed()) {
    int remoteInput2 = remote2.read();
    if (remoteInput2 == LOW) {
      if (client.connected()) {
        client.print("ch:down");
        Serial.println("ch:down");
      }
    }
  }

  if (remote3.changed()) {
    int remoteInput3 = remote3.read();
    if (remoteInput3 == LOW) {
      if (client.connected()) {
        client.print("vol:up");
        Serial.println("vol:up");
      }
    }
  }

  if (remote4.changed()) {
    int remoteInput4 = remote4.read();
    if (remoteInput4 == LOW) {
      if (client.connected()) {
        client.print("vol:down");
        Serial.println("vol:down");
      }
    }
  }

  if (digitalRead(phoneSwitch) == LOW) {
    //Serial.println("playing audio file");

    if (fiveminWarning == true) {
      player.playSpecified(4);  ///5 min warning message
      fiveminWarning = false;
    } else {
      player.playSpecified(3);  /// help message
    }
  }

  char customKey = keypad.getKey();

  /*
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char received = client.read();
    Serial.print(received);
    }
*/

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

// If key gets pressed, do the thing
void keypadEvent(KeypadEvent customKey) {
  switch (keypad.getState()) {
    case PRESSED:
      Serial.println(customKey);

      if (customKey == '0' && dialing == false) {  ///  SERVICE
        count++;
        //Serial.print(count);
        if (count == 1) {  /// if 4 characters send password attempt
          //delay(400);
          if (client.connected()) {
            client.print(String(phoneID) + ":service");
            Serial.println("sending " + String(phoneID) + ":service");
            ////Set light to flashing
            count = 0;
            break;
          }
        }
        if (phoneSwitch == LOW) {
          //play confirmation message if handset if off the hook
          player.playSpecified(8);
        }
      }

      if (customKey == '#' && dialing == false) {  /// DO NOT DISTURB
        count++;
        //Serial.print(count);

        if (count == 1) {  /// if 1 characters send password attempt
          //delay(400);
          if (client.connected()) {
            client.print(String(phoneID) + ":dnd");
            Serial.println("sending " + String(phoneID) + ":dnd");
            count = 0;
            break;
          }
        }
        if (phoneSwitch == LOW) {
          //play confirmation message if handset if off the hook
          player.playSpecified(9);
        }
      }

      if (customKey == '1' && dialing == false) {  ///External call
        dialing = true;                            ////enter 7 digit mode
        count = 0;
        memset(message, 0, sizeof(message));
        player.playSpecified(1);  // DIAL TONE
      }

      if (dialing == true) {  ///External call
        message[count - 1] = customKey;
        count++;
        Serial.println("MAKING A CALL");
        Serial.print(message);
      }

      /*
      if (count == 3 && (message[0] == "6" && message[1] == "6" && message[2] == "6")) {
        Serial.println("calling satan")
        count = 0;
        //dialing = false;
        }*/

      if (count == 8) {  /// if 7 characters send password attempt
        //delay(400);
        if (dialing == true) {
          if (client.connected()) {
            client.print(String(phoneID) + message);
            count = 0;
            Serial.println();
            Serial.println("dialing: " + String(message));
            dialing = false;
            //memset(message, 0, sizeof(message));
            break;
          } else {
            count = 0;
          }
        }
      }

      if ((customKey == '2' || customKey == '3' || customKey == '4' || customKey == '5' || customKey == '7' || customKey == '8' || customKey == '9') && dialing == false) {  /// HELP MESSAGE
        count = 0;
        if (phoneSwitch == LOW) {
          //play confirmation message if handset if off the hook
          player.playSpecified(5);
        }
      }

      if ((customKey == '6' && dialing == false)) {
        count++;
      }
  }
}


/////////////////////////////////////////////////////////////////////PROCESS INCOMING MESSAGE
// here to process incoming data after a terminator received
void process_data(const char* data) {
  incomingMessage = String(data);

  // for now just display it
  // (but you could compare it to some value, convert to an integer, etc.)
  Serial.println("FROM TOUCH: " + incomingMessage);

  if (incomingMessage == String(phoneID) + ":nope") {
    ///play error audio file
    player.playSpecified(5);
    //Serial.println("nope");
  }

  //////////////////////////////////////////////////////////////////////////5 min warning
  if (incomingMessage == "warning") {
    fiveminWarning = true;

    /////////////////////RING PHONE
    while (digitalRead(phoneSwitch) == HIGH) {
      digitalWrite(relayPin1, HIGH);
      digitalWrite(relayPin2, HIGH);
      delay(1000);
      digitalWrite(relayPin1, LOW);
      digitalWrite(relayPin2, LOW);
      delay(1000);

      ////play audio upon pickup
    }
  }

  ///////////////////////////////////////////////////////////////PHONE CALLS
  if (incomingMessage.substring(0, 5) == "play") {
    /////////////////////RING PHONE
    while (digitalRead(phoneSwitch) == HIGH) {
      digitalWrite(relayPin1, HIGH);
      digitalWrite(relayPin2, HIGH);
      delay(1000);
      digitalWrite(relayPin1, LOW);
      digitalWrite(relayPin2, LOW);
      delay(1000);
    }
    player.playSpecified(data[6]);
  }
  ///////////////////////////////////////////////////////////////TEST RING
  if (incomingMessage == "ring") {
    Serial.println("RING!");
    /////////////////////RING PHONE
    while (digitalRead(phoneSwitch) == HIGH) {
      digitalWrite(relayPin1, HIGH);
      digitalWrite(relayPin2, HIGH);
      delay(1000);
      digitalWrite(relayPin1, LOW);
      digitalWrite(relayPin2, LOW);
      delay(1000);
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