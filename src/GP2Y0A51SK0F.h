#include <Arduino.h>

#include "smoothed-value.h"

namespace GP2Y0A51SK0F
{

double lookupDistance(double volts);

template <int Smoothing> class ProximitySensor
{
public:
  ProximitySensor(int pin) : m_pin(pin)
  {
  }

  double getDistance()
  {
    return m_data.avg();
  }

  void sample()
  {
    auto value = analogRead(m_pin);
    auto volts = (value * 3.3) / 1023;
    auto distance = lookupDistance(volts);
    m_data.add(distance);
  }

private:
  int m_pin;
  SmoothedValue<double, Smoothing> m_data;
};

} // namespace GP2Y0A51SK0F
