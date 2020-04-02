#include "smoothed-value.h"

template <int Smoothing>
class ProximitySensor
{
public:
  ProximitySensor(int pin)
      : m_pin(pin)
  {
  }

  int getValue()
  {
    return m_data.avg();
  }

  void sample()
  {
    m_data.add(analogRead(m_pin));
  }

private:
  int m_pin;
  SmoothedValue<int, Smoothing> m_data;
};
