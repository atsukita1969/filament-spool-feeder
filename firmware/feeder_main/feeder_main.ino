// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023-2026 atsukita1969
// https://github.com/atsukita1969/filament-spool-feeder
/*
 * feeder_main - filament spool feeder, main controller
 *
 * Target : Arduino Pro or Pro Mini, ATmega328P (5V, 16 MHz)
 * Libs   : TMCStepper 0.7.3, AccelStepper 1.64
 *
 * While the feed request (D4) is HIGH the spool is driven; after it goes LOW the motor
 * runs FEED_STEPS more and stops. Feed values are unchanged from TMC2130_spool.ino (2023);
 * the speed jump that the old code made when the request ended is removed (see config.h).
 *
 * Changes from TMC2130_spool.ino
 *   - TMC2130Stepper (deprecated) -> TMCStepper. Resulting TMC2130 register values are identical.
 *   - The driver configuration is read back and verified; the driver is enabled only after that.
 *   - The TMC2130 is checked every DRIVER_CHECK_MS. If it was reset (GSTAT.reset, e.g. after the
 *     motor supply was interrupted) or stopped responding, it is reconfigured automatically.
 *   - The feed request is filtered (INPUT_FILTER_MS).
 *   - Motion profile set explicitly: same top speed and ramps as the old firmware really had,
 *     but no speed jump when the request ends.
 *   - Backup jam timeout in software (in addition to the ATtiny85), latched until reset.
 *   - Status messages on the serial port (115200 baud, FTDI header of the Pro Mini).
 *   - Status LED on D8 (board rev.B): ON = feed requested, slow blink = motor driver not ready
 *     (motor supply cut, e.g. by the ATtiny85 jam monitor), fast blink = fault latched.
 *   - Removed: while(!Serial) (no effect on the ATmega328P), unused defines and variables.
 */
#include <SPI.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include "config.h"

// Gives access to the library's shadow registers for read-back verification.
class Tmc2130 : public TMC2130Stepper {
 public:
  Tmc2130(uint16_t cs, float rs) : TMC2130Stepper(cs, rs) {}
  uint32_t expectedGCONF() const { return GCONF_register.sr; }
  uint32_t expectedCHOPCONF() const { return CHOPCONF_register.sr; }
};

static Tmc2130 driver(PIN_CS, R_SENSE);
static AccelStepper stepper(AccelStepper::DRIVER, PIN_STEP, PIN_DIR);

static bool driverReady = false;
static bool faultLatched = false;
static bool feedRequest = false;      // filtered request
static bool rawPrev = false;
static uint32_t rawChangeMs = 0;
static uint32_t requestSinceMs = 0;
static uint32_t lastCheckMs = 0;
static volatile uint8_t *reqPort;     // fast read of the request pin (keeps the loop as short as the old one)
static uint8_t reqMask;
#if MOTION_MODE == MOTION_EXPLICIT
static bool accelUp = false;          // true while the ramp-up acceleration is set
#endif
static bool ledOn = false;

// Write the configuration and verify it by reading back. Returns true when the driver is usable.
static bool configureDriver() {
  driver.begin();                                  // GCONF, CHOPCONF, COOLCONF, PWMCONF, IHOLD_IRUN, toff=8, tbl=1
  driver.rms_current(RUN_CURRENT_MA, HOLD_MULT);
  driver.en_pwm_mode(true);                        // stealthChop  (old: stealthChop(1))
  driver.pwm_autoscale(true);                      //              (old: stealth_autoscale(1))
  driver.microsteps(MICROSTEPS);

  if (driver.version() != TMC2130_VERSION) return false;
  if (driver.GCONF() != driver.expectedGCONF()) return false;
  if (driver.CHOPCONF() != driver.expectedCHOPCONF()) return false;
  (void)driver.GSTAT();                            // clear the reset flag (read clears)
  return true;
}

static void setDriverReady(bool ready, const __FlashStringHelper *why) {
  if (ready == driverReady) return;
  driverReady = ready;
  if (ready && !faultLatched) {
    stepper.enableOutputs();
  } else if (!ready) {
    stepper.disableOutputs();
  }
  Serial.println(why);
}

static void latchFault(const __FlashStringHelper *why) {
  faultLatched = true;
  stepper.setCurrentPosition(stepper.currentPosition());   // stop at once
  stepper.disableOutputs();
  Serial.println(why);
}

static void serviceDriver(uint32_t now) {
  if (now - lastCheckMs < DRIVER_CHECK_MS) return;
  lastCheckMs = now;
  if (driverReady) {
    const uint8_t gstat = driver.GSTAT();          // bit0 reset, bit1 drv_err, bit2 uv_cp (read clears)
    if (gstat & 0x02) {
      setDriverReady(false, F("TMC2130: driver error"));
      latchFault(F("FAULT: TMC2130 overtemperature or short circuit. Latched until reset."));
    } else if (gstat & 0x05) {
      setDriverReady(false, F("TMC2130: reset / undervoltage detected (motor supply interrupted?)"));
    } else if (driver.version() != TMC2130_VERSION) {
      setDriverReady(false, F("TMC2130: not responding"));
    }
  }
  if (!driverReady && !faultLatched && configureDriver()) {
    setDriverReady(true, F("TMC2130: configured and verified"));
  }
}

static void updateRequest(uint32_t now) {
  const bool raw = (*reqPort & reqMask) != 0;
  if (raw != rawPrev) {
    rawPrev = raw;
    rawChangeMs = now;
  } else if (raw != feedRequest && (now - rawChangeMs) >= INPUT_FILTER_MS) {
    feedRequest = raw;
    if (raw) requestSinceMs = now;
  }
}

// Status LED. Bit tests on millis() keep this cheap enough for every loop pass.
static void updateLed(uint32_t now) {
  bool on;
  if (faultLatched) {
    on = (now >> 7) & 1;               // fast blink, 128 ms on / 128 ms off
  } else if (!driverReady) {
    on = (now >> 9) & 1;               // slow blink, 512 ms on / 512 ms off
  } else {
    on = feedRequest;
  }
  if (on != ledOn) {
    ledOn = on;
    digitalWrite(PIN_LED, on ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("feeder_main start"));

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  pinMode(PIN_REQUEST, INPUT);                     // external pull-down R1
  reqPort = portInputRegister(digitalPinToPort(PIN_REQUEST));
  reqMask = digitalPinToBitMask(PIN_REQUEST);
  SPI.begin();

  stepper.setMaxSpeed(MOTION_MODE == MOTION_LEGACY ? MAX_SPEED : EXPLICIT_MAX_SPEED);
  stepper.setAcceleration(MOTION_MODE == MOTION_LEGACY ? ACCELERATION : EXPLICIT_DECEL);
  stepper.setEnablePin(PIN_EN);                    // driver stays disabled here
  stepper.setPinsInverted(DIR_INVERT, false, true);
  stepper.disableOutputs();

  lastCheckMs = millis() - DRIVER_CHECK_MS;        // first check right away
  serviceDriver(millis());
}

void loop() {
  const uint32_t now = millis();
  updateRequest(now);

  if (feedRequest && !faultLatched) {
    if (BACKUP_TIMEOUT_MS && (now - requestSinceMs) >= BACKUP_TIMEOUT_MS) {
      latchFault(F("FAULT: feed request active too long (jam?). Latched until reset."));
    } else if (driverReady) {
#if MOTION_MODE == MOTION_LEGACY
      stepper.move(FEED_STEPS);                    // same call pattern as the 2023 firmware
      stepper.enableOutputs();
#else
      if (!accelUp) {
        stepper.setAcceleration(EXPLICIT_ACCEL_UP);
        accelUp = true;
      }
      if (stepper.distanceToGo() < FEED_STEPS - RETARGET_STEPS) stepper.move(FEED_STEPS);
#endif
    }
  }
#if MOTION_MODE == MOTION_EXPLICIT
  if (!feedRequest && accelUp) {                   // request ended
    if (driverReady && !faultLatched) stepper.move(FEED_STEPS);   // exactly FEED_STEPS more, as before
    stepper.setAcceleration(EXPLICIT_DECEL);       // decelerate with the original value
    accelUp = false;
  }
#endif
  stepper.run();
  serviceDriver(now);
  updateLed(now);
}
