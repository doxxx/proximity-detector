#include <iterator>

#include "smoothed-value.h"

struct LerpEntry
{
  double volts;
  double distance;
};

const static int DISTANCE_TABLE_LEN = 5;
constexpr static LerpEntry DISTANCE_TABLE[DISTANCE_TABLE_LEN] {
    {0.30, 20},
    {0.39, 15},
    {0.58, 10},
    {1.07, 5},
    {2.09, 2},
};

static double lookupDistance(double volts)
{
  if (volts <= DISTANCE_TABLE[0].volts) {
    return DISTANCE_TABLE[0].distance;
  }
  if (volts >= DISTANCE_TABLE[DISTANCE_TABLE_LEN-1].volts) {
    return DISTANCE_TABLE[DISTANCE_TABLE_LEN-1].distance;
  }
  for (int i = 0; i < DISTANCE_TABLE_LEN; i++) {
    auto &e = DISTANCE_TABLE[i];
    if (e.volts > volts) {
      auto &p = DISTANCE_TABLE[i-1];
      auto volts_diff = e.volts - p.volts;
      auto f = 1.0 - (volts - p.volts) / volts_diff;
      auto distance_diff = p.distance - e.distance;
      return f * distance_diff + e.distance;
    }
  }
}

template <int Smoothing>
class ProximitySensor
{
public:
  ProximitySensor(int pin)
      : m_pin(pin)
  {
  }

  int getDistance()
  {
    return m_data.avg();
  }

  void sample()
  {
    auto value = analogRead(m_pin);
    auto volts = (value * 3.3) / 1023;
    auto distance = lookupDistance(volts);
    Serial.print("sensor: value=");
    Serial.print(value);
    Serial.print(", volts=");
    Serial.print(volts, 2);
    Serial.print(", distance=");
    Serial.print(distance, 2);
    Serial.println();
    m_data.add(distance);
  }

private:
  int m_pin;
  SmoothedValue<int, Smoothing> m_data;
};
