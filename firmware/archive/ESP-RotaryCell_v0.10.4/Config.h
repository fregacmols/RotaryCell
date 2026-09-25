#pragma once
#include <Arduino.h>

// -----------------------------------------------------------------------------
// Ag1171 connections
// -----------------------------------------------------------------------------
constexpr uint8_t PIN_FR  = 15;  // Forward/Reverse
constexpr uint8_t PIN_RM  = 16;  // Ring Mode
constexpr uint8_t PIN_SHK = 37;  // Switch Hook output
constexpr bool SHK_ACTIVE_HIGH = true;

// Dial-tone PWM output. GPIO36 is part of the unused camera interface on the
// LilyGO S3-Standard board and is free when no camera is fitted.
constexpr uint8_t PIN_DIAL_TONE = 36;

// -----------------------------------------------------------------------------
// LilyGO T-A7670G-S3 Standard
// Official Standard-board mapping:
//   modem TX -> ESP32 RX GPIO5
//   modem RX <- ESP32 TX GPIO4
// -----------------------------------------------------------------------------
constexpr uint8_t PIN_MODEM_RX         = 5;
constexpr uint8_t PIN_MODEM_TX         = 4;
constexpr uint8_t PIN_MODEM_RING       = 6;
constexpr uint8_t PIN_MODEM_DTR        = 7;
constexpr uint8_t PIN_MODEM_PWRKEY     = 46;
constexpr uint8_t PIN_MODEM_POWER_SAVE = 42;

constexpr uint32_t MODEM_BAUD = 115200;
constexpr uint16_t MODEM_POWERKEY_LOW_PREP_MS = 100;
constexpr uint16_t MODEM_POWERKEY_HIGH_MS = 1000;
constexpr uint32_t MODEM_BOOT_WAIT_MS = 9000;
constexpr uint32_t MODEM_AT_TIMEOUT_MS = 1200;
constexpr uint32_t MODEM_RING_TIMEOUT_MS = 10000;

// -----------------------------------------------------------------------------
// Battery monitor
// -----------------------------------------------------------------------------
constexpr uint8_t PIN_BATTERY_ADC = 8;
constexpr uint8_t BATTERY_DIVIDER_RATIO = 2;
constexpr uint8_t BATTERY_SAMPLE_COUNT = 8;
constexpr uint32_t BATTERY_SAMPLE_INTERVAL_MS = 5000;
constexpr uint16_t BATTERY_VALID_MIN_MV = 2500;
constexpr uint16_t BATTERY_VALID_MAX_MV = 4500;

// -----------------------------------------------------------------------------
// USB serial console
// -----------------------------------------------------------------------------
constexpr uint32_t SERIAL_BAUD = 115200;

// -----------------------------------------------------------------------------
// Phone-line timing
// -----------------------------------------------------------------------------
constexpr uint16_t SHK_DEBOUNCE_MS       = 8;
constexpr uint16_t DIAL_PULSE_MIN_MS     = 25;
constexpr uint16_t DIAL_PULSE_MAX_MS     = 140;
constexpr uint16_t DIGIT_COMPLETE_MS     = 250;
constexpr uint16_t HANGUP_CONFIRM_MS     = 500;
constexpr uint32_t DIAL_NUMBER_TIMEOUT_MS = 3000;
constexpr uint8_t  DIAL_MAX_DIGITS        = 20;
constexpr uint8_t  DIAL_MIN_DIGITS        = 7;   // Reject incomplete short numbers before ATD
constexpr uint32_t OFF_HOOK_DIAL_TONE_TIMEOUT_MS = 15000;
constexpr uint32_t REORDER_BEFORE_HOWLER_MS = 10000;

// -----------------------------------------------------------------------------
// Ringing
// -----------------------------------------------------------------------------
constexpr uint16_t RING_HALF_PERIOD_MS   = 25;    // 20 Hz complete cycle
constexpr uint32_t RING_TEST_DURATION_MS = 1000;
constexpr uint32_t CADENCE_RING_ON_MS    = 2000;
constexpr uint32_t CADENCE_RING_OFF_MS   = 4000;

// -----------------------------------------------------------------------------
// v0.9 maintenance Wi-Fi / web console
// -----------------------------------------------------------------------------
constexpr const char* FIRMWARE_VERSION = "0.10.4";
constexpr const char* WIFI_AP_SSID = "ESP-RotaryCell";
// Change this before deployment if the phone will be used around untrusted people.
// WPA2 requires at least eight characters.
constexpr const char* WIFI_AP_PASSWORD = "rotarycell";
constexpr uint32_t WIFI_MAINTENANCE_TIMEOUT_MS = 10UL * 60UL * 1000UL;
constexpr uint16_t WEB_SERVER_PORT = 80;
constexpr size_t WEB_LOG_CHUNK_BYTES = 8192;

// Cellular-network clock synchronization.  Retry quickly until a valid +CCLK
// has been received, then refresh occasionally while the phone is idle.
constexpr uint32_t CLOCK_RETRY_INTERVAL_MS = 60UL * 1000UL;
constexpr uint32_t CLOCK_RESYNC_INTERVAL_MS = 6UL * 60UL * 60UL * 1000UL;
