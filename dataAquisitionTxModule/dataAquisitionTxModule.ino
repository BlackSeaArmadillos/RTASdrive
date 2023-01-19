/*
 * Tx Module transmits aquired data to the Rx Module via LoRa
*/

// Include libraries
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the LoRa Tx module
#define ss 5
#define rst 17
#define dio0 2

// Potentiometer is connected to GPIO 33 (Analog ADC1_CH5) 
const int potPin = 33;

// variable for storing the potentiometer value
int potValue = 0;

int counter = 0;

void setup() 
{
  Serial.begin(115200);  
  while (!Serial);
  Serial.println("Data Aquisition Tx Module initialising...");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) 
  {
    Serial.println(".");
    delay(500);
  }

  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!"); 
  
  delay(1000);
}

void loop() 
{
  // Reading potentiometer value
  potValue = analogRead(potPin);
  Serial.println(potValue);
  Serial.print("Counter = ");
  Serial.println(counter);
  counter++;
  Serial.print("Sending packet: ");
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("Analog read: ");
  LoRa.print(potValue);
  LoRa.endPacket();

  delay(1000);
}
