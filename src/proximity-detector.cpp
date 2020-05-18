// RMF69 Proximity Detector
//
// Uses a Sharp GP2Y0A51SK0F IR LED proximity sensor to detect a nearby object
// and transmits a notification via RFM69.
//
// RFM69 hook-up:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide
//
// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69

#include <Arduino.h>
#include "GP2Y0A51SK0F.h"
#include <RFM69.h>
#include "STM32Power.h"

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORK_ID 1  // Must be the same for all nodes
#define MY_NODE_ID 2  // Node ID of this detector
#define HUB_NODE_ID 1 // Node ID of the hub which receives the notification

// RFM69 frequency, uncomment the frequency of your module:
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY RF69_915MHZ

// AES encryption (or not):
#define ENCRYPT false                 // Set to "true" to use encryption
#define ENCRYPTKEY "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Heartbeat LED
#define HEARTBEAT_LED PC13

// Packet sent/received indicator LED:
#define PACKET_LED PA8

// Sensor active indicator LED
#define ACTIVE_LED PA12

// Proximity sensor data pin
#define SENSOR_PIN PA0

RFM69 radio(PA4, PA3, true);

void setup()
{
  Serial.begin(9600);
  // while (!Serial.available()) {}

  delay(1000);

  // Set up the indicator LEDs
  pinMode(HEARTBEAT_LED, OUTPUT);
  digitalWrite(HEARTBEAT_LED, HIGH); // LOW==ON!!!
  pinMode(PACKET_LED, OUTPUT);
  digitalWrite(PACKET_LED, LOW);
  pinMode(ACTIVE_LED, OUTPUT);
  digitalWrite(ACTIVE_LED, LOW);

  // Setup IR sensor pin
  pinMode(SENSOR_PIN, INPUT_ANALOG);
  delay(22); // allow IR sensor to stabilize

  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MY_NODE_ID, NETWORK_ID);
  radio.setHighPower();
  if (ENCRYPT) radio.encrypt(ENCRYPTKEY);

  Serial.print("RFM initialized with node #");
  Serial.print(MY_NODE_ID, DEC);
  Serial.println();

  LowPower.begin();
}

// Message format:
//   "GHT" type<u8> [data...]
// Types:
//   0x01: Proximity State Change
//         Data: active<u8>
//           active: 0x00=false, 0x01=true

uint8_t makeMsg_ProximityStateChange(uint8_t *buf, bool active)
{
  uint8_t i = 0;
  buf[i++] = 'G';
  buf[i++] = 'H';
  buf[i++] = 'T';
  buf[i++] = 0x01;
  buf[i++] = active ? 0x01 : 0x00;
  return i;
}

void checkProximity(bool &active, int &value)
{
  static auto sensor = ProximitySensor<1>(SENSOR_PIN);
  sensor.sample();
  value = sensor.getDistance();
  active = value < 5;
}

void updateProximity()
{
  static uint8_t messageBuffer[16];
  static uint8_t messageLen;
  static bool active = false;

  bool newActive;
  int value;

  checkProximity(newActive, value);

  if (newActive != active) {
    active = newActive;
    Serial.print("active: ");
    Serial.println(active);
    if (active)
    {
      digitalWrite(ACTIVE_LED, HIGH);
    }
    else
    {
      digitalWrite(ACTIVE_LED, LOW);
    }
    messageLen = makeMsg_ProximityStateChange(messageBuffer, active);
    digitalWrite(PACKET_LED, HIGH);
    radio.receiveDone(); // wake-up radio
    radio.sendWithRetry(HUB_NODE_ID, messageBuffer, messageLen);
    radio.sleep();
    digitalWrite(PACKET_LED, LOW);
  }
}

void loop()
{
  digitalWrite(HEARTBEAT_LED, LOW); // turn on heartbeat LED
  updateProximity();
  digitalWrite(HEARTBEAT_LED, HIGH); // turn off heartbeat LED

  LowPower.sleep(1000); // sleep for a second
  
  delay(100); // wait for sensor to stabilize
}
