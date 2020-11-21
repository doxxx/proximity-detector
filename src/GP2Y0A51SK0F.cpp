#include "GP2Y0A51SK0F.h"

struct LerpEntry
{
  double volts;
  double distance;
};

const static int DISTANCE_TABLE_LEN = 5;
constexpr static LerpEntry DISTANCE_TABLE[DISTANCE_TABLE_LEN] = {
    // clang-format off
    {0.30, 20}, 
    {0.39, 15}, 
    {0.58, 10}, 
    {1.07, 5}, 
    {2.09, 2},
    // clang-format on
};

namespace GP2Y0A51SK0F
{
double lookupDistance(double volts)
{
  if (volts <= DISTANCE_TABLE[0].volts)
  {
    return DISTANCE_TABLE[0].distance;
  }

  if (volts >= DISTANCE_TABLE[DISTANCE_TABLE_LEN - 1].volts)
  {
    return DISTANCE_TABLE[DISTANCE_TABLE_LEN - 1].distance;
  }

  for (int i = 0; i < DISTANCE_TABLE_LEN; i++)
  {
    auto &e = DISTANCE_TABLE[i];
    if (e.volts > volts)
    {
      auto &p = DISTANCE_TABLE[i - 1];
      auto volts_diff = e.volts - p.volts;
      auto f = 1.0 - (volts - p.volts) / volts_diff;
      auto distance_diff = p.distance - e.distance;
      return f * distance_diff + e.distance;
    }
  }
  return 0;
}

} // namespace GP2Y0A51SK0F
