#pragma once

#include <algorithm>

template <typename T, int N>
class SmoothedValue
{
 public:
  void add(T v)
  {
    T old = values[index];
    values[index] = v;
    index = (index + 1) % N;
    total -= old;
    total += v;
    count++;
  }

  T avg()
  {
    return total / std::min(N, count);
  }

 private:
  T values[N];
  int index;
  T total;
  int count;
};
