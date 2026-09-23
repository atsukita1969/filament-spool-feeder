// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023-2026 atsukita1969
// https://github.com/atsukita1969/filament-spool-feeder
// config.h - settings for feeder_main (Arduino Pro Mini 5V 16 MHz + TMC2130 SPI)
#pragma once
#include <stdint.h>

// ---- Pin assignment (schematic spool_tmc2130_b) ----
constexpr uint8_t PIN_REQUEST = 4;    // D4: feed request (X1 limit switch / X9 sensor), HIGH = feed, ext. pull-down R1
constexpr uint8_t PIN_DIR     = 5;    // D5: TMC2130 DIR
constexpr uint8_t PIN_STEP    = 6;    // D6: TMC2130 STEP
constexpr uint8_t PIN_EN      = 7;    // D7: TMC2130 EN (active LOW)
constexpr uint8_t PIN_CS      = 10;   // D10: TMC2130 CS  (D11 MOSI, D12 MISO, D13 SCK = hardware SPI)
constexpr uint8_t PIN_LED     = 8;    // D8: status LED D3 via R4 (board rev.B; not connected on rev.A)

// ---- Feed control: same values as the 2023 firmware (TMC2130_spool.ino) ----
constexpr long  FEED_STEPS   = 4000;  // 50 * 80: steps fed after the request ends
constexpr float MAX_SPEED    = 4000;  // steps/s  (setMaxSpeed)
constexpr float ACCELERATION = 2000;  // steps/s^2 (setAcceleration)
constexpr bool  DIR_INVERT   = true;  // setPinsInverted(true, false, true)

// Motion call pattern.
//   MOTION_EXPLICIT : (default) the motion profile is set explicitly. The values below reproduce what
//                     the 2023 firmware actually did (measured in simulation), without its speed jump
//                     at the moment the request ends.
//   MOTION_LEGACY   : identical call pattern to the 2023 firmware (move() and enableOutputs() on every
//                     loop pass). Kept for comparison only. The real speed is then limited by loop time
//                     (about 2390 steps/s) and jumps to about 3900 steps/s when the request ends.
#define MOTION_LEGACY   1
#define MOTION_EXPLICIT 2
#ifndef MOTION_MODE                   // can be overridden with -DMOTION_MODE=1
#define MOTION_MODE     MOTION_EXPLICIT
#endif
constexpr float EXPLICIT_MAX_SPEED = 2400;               // steps/s   (2023 firmware: about 2390 while requested)
constexpr float EXPLICIT_ACCEL_UP  = 2 * ACCELERATION;   // steps/s^2 while requested (2023 firmware: about 3800)
constexpr float EXPLICIT_DECEL     = ACCELERATION;       // steps/s^2 after the request ends (2023 firmware: about 1960)
constexpr long  RETARGET_STEPS     = 32;                 // re-extend the target when it has shrunk by this much

// ---- TMC2130: same values as the 2023 firmware ----
constexpr float    R_SENSE        = 0.11f;  // ohm (library default of the old code)
constexpr uint16_t RUN_CURRENT_MA = 600;    // RMS
constexpr float    HOLD_MULT      = 0.5f;   // hold current = run current * 0.5
constexpr uint16_t MICROSTEPS     = 16;
constexpr uint8_t  TMC2130_VERSION = 0x11;  // IOIN.VERSION of the TMC2130

// ---- Robustness (new) ----
constexpr uint16_t INPUT_FILTER_MS   = 5;       // request must be stable this long
constexpr uint16_t DRIVER_CHECK_MS   = 500;     // TMC2130 health check period
constexpr uint32_t BACKUP_TIMEOUT_MS = 53000UL; // software jam timeout (backup of the ATtiny85, 48 s); 0 = off
constexpr uint32_t SERIAL_BAUD       = 115200;
