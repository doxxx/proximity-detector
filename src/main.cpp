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

#include "GP2Y0A51SK0F.h"
#include "STM32LowPower.h"
#include <Arduino.h>
#include <RFM69.h>

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

// Proximity power switch pin
#define SENSOR_POWER_PIN PA2

// Proximity sensor data pin
#define SENSOR_DATA_PIN PA0

// Distance (cm) under which the proximity state becomes active
#define DISTANCE_THRESHOLD 8

// Time in milliseconds between proximity checks
#define PROXIMITY_CHECK_INTERVAL 5000

RFM69 radio(PA4, PA3, true);

void setup()
{
#ifdef PD_DEBUG
  Serial.begin(9600);
#endif

  // Set up the heartbeat LED
  pinMode(HEARTBEAT_LED, OUTPUT);
  digitalWrite(HEARTBEAT_LED, HIGH); // LOW==ON!!!

  // Setup IR power switch pin
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Setup IR sensor pin
  pinMode(SENSOR_DATA_PIN, INPUT_ANALOG);

  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MY_NODE_ID, NETWORK_ID);
  radio.setHighPower();

  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);

#ifdef PD_DEBUG
  Serial.print("RFM initialized with node #");
  Serial.print(MY_NODE_ID, DEC);
  Serial.println();
#endif

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

double checkProximity()
{
  using namespace GP2Y0A51SK0F;

  const int sampleCount = 5;
  static auto sensor = ProximitySensor<sampleCount>(SENSOR_DATA_PIN);
  for (int i = 0; i < sampleCount; i++)
  {
    delay(30);       // wait for next measurement to become available
    sensor.sample(); // sample sensor
  }
  return sensor.getDistance();
}

void sendProximityStateChange(bool active)
{
  uint8_t messageBuffer[16];
  uint8_t messageLen = makeMsg_ProximityStateChange(messageBuffer, active);

  radio.sendWithRetry(HUB_NODE_ID, messageBuffer, messageLen);
}

bool updateProximityState()
{
  digitalWrite(SENSOR_POWER_PIN, HIGH); // turn on sensor
  delay(100);                            // wait for sensor to stabilize
  auto distance = checkProximity();
  digitalWrite(SENSOR_POWER_PIN, LOW); // turn off sensor

  bool active = distance < DISTANCE_THRESHOLD;
  
#ifdef PD_DEBUG
  Serial.print("distance=");
  Serial.print(distance);
  Serial.print(", active=");
  Serial.print(active);
  Serial.println();
  Serial.flush();
#endif

  sendProximityStateChange(active);

  return active;
}

void dumpPacket()
{
  static int packetCount = 0;
  Serial.print("#[");
  Serial.print(++packetCount);
  Serial.print(']');
  Serial.print('[');
  Serial.print(radio.SENDERID, DEC);
  Serial.print("] ");

  for (int i = 0; i < radio.DATALEN; i++)
  {
    char c = radio.DATA[i];
    if (isprint(c))
    {
      Serial.print(c);
    }
    else
    {
      Serial.print('<');
      Serial.print((int)c);
      Serial.print('>');
    }
  }

  Serial.print("  [RSSI:");
  Serial.print(radio.RSSI);
  Serial.print("]");
  Serial.println();
}

void loop()
{
  digitalWrite(HEARTBEAT_LED, LOW); // turn on heartbeat LED

#ifdef PD_DEBUG
    Serial.println("updating proximity state");
    Serial.flush();
#endif

  updateProximityState();
  
  // ensure radio is asleep
  radio.sleep();

  digitalWrite(HEARTBEAT_LED, HIGH); // turn off heartbeat LED

#ifdef PD_DEBUG
    Serial.println("sleeping");
    Serial.flush();
#endif

  LowPower.deepSleep(PROXIMITY_CHECK_INTERVAL);
}
