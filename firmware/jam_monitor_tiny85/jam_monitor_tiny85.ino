// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023-2026 atsukita1969
// https://github.com/atsukita1969/filament-spool-feeder
/*
 * jam_monitor_tiny85 - independent jam watchdog for the filament spool feeder
 *
 * Target : ATtiny85, ATTinyCore (board "ATtiny25/45/85 (No bootloader)"),
 *          Clock 8 MHz internal, B.O.D. 2.7 V. Run "Burn Bootloader" once to set the fuses.
 *
 * Function
 *   PB1 (pin 6) : feed request, same net as Pro Mini D4 (limit switch X1 / sensor X9).
 *                 HIGH = filament pulled = feed requested. External 10k pull-down (R1).
 *   PB3 (pin 2) : EXT_IN. HIGH -> T1 -> relay K1 closes -> motor supply VM on.
 *                 External 10k pull-down (R3) keeps the relay OFF whenever this pin is not driven.
 *   PB4 (pin 3) : external sensor input (board rev.B only, EXT_SENSE_ENABLED = 1).
 *                 X5 pin 2 with 10k pull-down. HIGH = sensor OK, LOW or open wire = fault.
 *                 Jumper JP (X5 signal to +5V) = external sensor not used.
 *   PB5 (pin 1) : RESET, shared with the Pro Mini reset (X3). X3 also clears a latched fault.
 *
 *   The motor supply is cut and the state is latched until reset / power cycle when
 *     - the feed request stays HIGH continuously for JAM_TIMEOUT_MS (jam), or
 *     - (rev.B) the external sensor reports a fault for EXT_FAULT_FILTER_MS, or is not OK
 *       within EXT_STARTUP_GRACE_MS after power-up.
 *
 * Changes from motor_emergency_stop_tiny85.ino (2023)
 *   - The timer is reset deterministically when the request goes LOW (the old code reset it
 *     only if the input fell during a 10 ms window, so about 1/3 of releases were missed).
 *   - Timeout measured in real time with millis() instead of counting loop passes.
 *     48 s = the effective timeout of the old code at 8 MHz (3000 passes of 10 ms delay
 *     plus Serial output time, measured in simulation: 47.9 s).
 *   - Serial removed: on ATTinyCore its RX shares PB1 and its TX (PB0) is not connected.
 *   - Short LOW glitches (contact chatter) do not reset the timer.
 *   - Fault output is high impedance, so on rev.A boards a fitted jumper on EXT_IN or a signal
 *     on X5 cannot short the pin.
 *   - Watchdog: a hang resets the MCU; a watchdog reset is treated as a fault (fail safe).
 *   - External sensor input for board rev.B.
 */
#include <avr/io.h>
#include <avr/wdt.h>

// ---- Pins (ATTinyCore numbering = PBn) ----
constexpr uint8_t PIN_REQUEST   = 1;   // PB1
constexpr uint8_t PIN_RELAY     = 3;   // PB3
constexpr uint8_t PIN_EXT_SENSE = 4;   // PB4 (rev.B)

// ---- Parameters ----
constexpr uint32_t JAM_TIMEOUT_MS    = 48000UL;  // continuous feed request that is treated as a jam
constexpr uint32_t RELEASE_FILTER_MS = 20UL;     // request must be LOW this long to end an episode

// External sensor (board rev.B). 0 on rev.A boards: PB4 is not connected there.
#ifndef EXT_SENSE_ENABLED                 // can be overridden with -DEXT_SENSE_ENABLED=1
#define EXT_SENSE_ENABLED 0
#endif
constexpr uint32_t EXT_FAULT_FILTER_MS  = 50UL;    // LOW this long while running = sensor fault
constexpr uint32_t EXT_STARTUP_GRACE_MS = 1000UL;  // sensor must report OK within this time

static bool     relayIsOn = false;
static bool     episodeActive = false;
static uint32_t episodeStartMs = 0;
static uint32_t lastHighMs = 0;
#if EXT_SENSE_ENABLED
static bool     extLowTiming = false;
static uint32_t extLowSinceMs = 0;
#endif

static void relayOn() {
  digitalWrite(PIN_RELAY, HIGH);
  pinMode(PIN_RELAY, OUTPUT);
  relayIsOn = true;
}

// Cut the motor supply and stay here until reset / power cycle.
static void latchFault() __attribute__((noreturn));
static void latchFault() {
  digitalWrite(PIN_RELAY, LOW);    // relay off at once
  pinMode(PIN_RELAY, INPUT);       // then high impedance (PORT bit is 0, so no pull-up); R3 holds it LOW
  for (;;) {
    wdt_reset();
  }
}

void setup() {
  const uint8_t resetCause = MCUSR;
  MCUSR = 0;
  wdt_disable();

  pinMode(PIN_REQUEST, INPUT);     // external pull-down R1, no internal pull-up
#if EXT_SENSE_ENABLED
  pinMode(PIN_EXT_SENSE, INPUT);   // external pull-down on rev.B, no internal pull-up
#endif

  wdt_enable(WDTO_1S);
  if (resetCause & _BV(WDRF)) {
    latchFault();                  // previous run hung: do not restart the motor by itself
  }
#if !EXT_SENSE_ENABLED
  relayOn();
#endif
}

void loop() {
  wdt_reset();
  const uint32_t now = millis();

#if EXT_SENSE_ENABLED
  if (digitalRead(PIN_EXT_SENSE) == HIGH) {
    extLowTiming = false;
    if (!relayIsOn) relayOn();     // motor supply only after the sensor has reported OK
  } else {
    if (!extLowTiming) {
      extLowTiming = true;
      extLowSinceMs = now;
    }
    if (relayIsOn && (now - extLowSinceMs) >= EXT_FAULT_FILTER_MS) latchFault();
    if (!relayIsOn && now >= EXT_STARTUP_GRACE_MS) latchFault();
  }
#endif

  if (digitalRead(PIN_REQUEST) == HIGH) {
    lastHighMs = now;
    if (!episodeActive) {
      episodeActive = true;
      episodeStartMs = now;
    } else if (now - episodeStartMs >= JAM_TIMEOUT_MS) {
      latchFault();
    }
  } else if (episodeActive && (now - lastHighMs >= RELEASE_FILTER_MS)) {
    episodeActive = false;         // request released: timer is reset
  }
}
