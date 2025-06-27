#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
//DY::Player player;

// Alternatively initialise on another serial port.
 DY::Player player(&Serial1);


int relayPin1 = 9;   // ringer
int relayPin2 = 10;  //led
int phoneSwitch = 11;

void setup() {
   Serial.begin(9600);
   Serial.println("hi");
  pinMode(13, OUTPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(phoneSwitch, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(relayPin1, HIGH);  // turn the LED on (HIGH is the voltage level)
  digitalWrite(relayPin2, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
   digitalWrite(relayPin2, LOW);  // turn the LED on (HIGH is the voltage level)
   digitalWrite(relayPin1, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(1000); 
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
     digitalWrite(relayPin2, HIGH);  // turn the LED on (HIGH is the voltage level)
     digitalWrite(relayPin1, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);        

  player.begin();
  player.setVolume(15);  // 50% Volume
  // Pick one of these..
  // player.setCycleMode(DY::PlayMode::Sequence); // Play all and stop.
  player.setCycleMode(DY::PlayMode::Repeat);  // Play all and repeat.
  player.play();
}

void loop() {
 
  /* Nothing to do.. */

    int switchState = digitalRead(phoneSwitch);
  // print out the state of the button:
  Serial.println(switchState);
  delay(1);  // delay in between reads for stability


  if (switchState == 0){              // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
   digitalWrite(relayPin1, LOW);  // turn the LED on (HIGH is the voltage level)
   digitalWrite(relayPin2, LOW);  // turn the LED on (HIGH is the voltage level)
  }else{
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
     digitalWrite(relayPin1, HIGH);  // turn the LED on (HIGH is the voltage level)
     digitalWrite(relayPin2, HIGH);  // turn the LED on (HIGH is the voltage level)
  }
  //delay(5000);
}
