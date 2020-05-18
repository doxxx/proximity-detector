#pragma once

#include <Arduino.h>
#include <STM32RTC.h>

enum LP_Mode : uint8_t {
  IDLE_MODE,
  SLEEP_MODE,
  DEEP_SLEEP_MODE,
  SHUTDOWN_MODE
};

typedef void (*voidFuncPtrVoid)(void) ;

class STM32LowPower {
  public:
    STM32LowPower();

    void begin(void);

    void idle(uint32_t millis = 0);
    void idle(int millis)
    {
      idle((uint32_t)millis);
    }

    void sleep(uint32_t millis = 0);
    void sleep(int millis)
    {
      sleep((uint32_t)millis);
    }

    void deepSleep(uint32_t millis = 0);
    void deepSleep(int millis)
    {
      deepSleep((uint32_t)millis);
    }

    void shutdown(uint32_t millis = 0);
    void shutdown(int millis)
    {
      shutdown((uint32_t)millis);
    }

    void attachInterruptWakeup(uint32_t pin, voidFuncPtrVoid callback, uint32_t mode, LP_Mode LowPowerMode = SHUTDOWN_MODE);

    void enableWakeupFrom(HardwareSerial *serial, voidFuncPtrVoid callback);
    void enableWakeupFrom(STM32RTC *rtc, voidFuncPtr callback, void *data = NULL);

  private:
    bool _configured;     // Low Power mode initialization status
    serial_t *_serial;    // Serial for wakeup from deep sleep
    bool _rtc_wakeup;     // Is RTC wakeup?
    void programRtcWakeUp(uint32_t millis, LP_Mode lp_mode);
};

extern STM32LowPower LowPower;
