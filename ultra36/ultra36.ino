//   _____  ___________              _______________
//   __  / / /__  /_  /_____________ __|__  /_  ___/
//   _  / / /__  /_  __/_  ___/  __ `/__/_ <_  __ \ 
//   / /_/ / _  / / /_ _  /   / /_/ /____/ // /_/ / 
//   \____/  /_/  \__/ /_/    \__,_/ /____/ \____/  
// Ultra-36 Rom Switcher for Commodore 128 - ATTiny84A Frimware
// Free for personal use.
// Commercial use or resale (in whole or part) prohibited without permission.
// (c) 2025 Lukasz Dziwosz / LukasSoft. All Rights Reserved.

#include "pin_map.h"
#include "config.h"
#include <avr/interrupt.h>
#include <EEPROM.h>

// --- Global Variables ---
volatile uint8_t lastCommand = 0;  // Stores command from ISR
uint8_t currentBank = 0;           // Current ROM bank (0–15)
bool jiffyState = false;           // JiffyDOS state
unsigned long resetHoldStart = 0;   // Reset timer

// --- Setup ---
void setup() {
  // Configure outputs (A15-A18, Jiffy control)
  pinMode(PIN_A15, OUTPUT);
  pinMode(PIN_A16, OUTPUT);
  pinMode(PIN_A17, OUTPUT);
  pinMode(PIN_A18, OUTPUT);
  digitalWrite(PIN_A18, HIGH);  // Prevent accidental reset as uses PB3
  pinMode(PIN_JIFFY_CTRL, OUTPUT);

  // Configure inputs (Reset, Trap, Data bus)
  pinMode(PIN_RESET_MON, INPUT_PULLUP);
  pinMode(PIN_TRAP_MON, INPUT_PULLUP);
  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  pinMode(PIN_D2, INPUT);
  pinMode(PIN_D3, INPUT);
  pinMode(PIN_D4, INPUT);

  // Load saved config from EEPROM
  currentBank = EEPROM.read(EEPROM_ADDR_BANK);
  jiffyState = EEPROM.read(EEPROM_ADDR_JIFFY) != 0;

  // Validate bank number
#if USE_SST39SF040
  if (currentBank > 15) currentBank = 0;  // 16 banks max (0–15)
#else
  if (currentBank > 7) currentBank = 0;   // 8 banks max (0–7)
#endif

  // Apply initial settings
  applyBank(currentBank);
  applyJiffy(jiffyState);

  // Configure interrupt for $D700 trap
  attachInterrupt(digitalPinToInterrupt(PIN_TRAP_MON), isrTrap, FALLING);
}

// --- Main Loop ---
void loop() {
  pollReset();      // Check for long-press reset
  processCommand(); // Handle any new ROM/Jiffy commands
  delay(POLL_INTERVAL_MS);
}

// --- Interrupt Service Routine ---
void isrTrap() {
  // Capture data bus (D0-D4) immediately
  lastCommand = 0;
  if (digitalRead(PIN_D0)) lastCommand |= 0x01;
  if (digitalRead(PIN_D1)) lastCommand |= 0x02;
  if (digitalRead(PIN_D2)) lastCommand |= 0x04;
  if (digitalRead(PIN_D3)) lastCommand |= 0x08;
  if (digitalRead(PIN_D4)) lastCommand |= 0x10;
}

// --- Process Command from ISR ---
void processCommand() {
  if (lastCommand == 0) return;  // No new command

  uint8_t newBank = lastCommand & ROM_BANK_MASK; 
  bool newJiffyState = (lastCommand & JIFFY_MASK) != 0;

  // Validate bank range
#if USE_SST39SF040
  if (newBank > 16) newBank = 1;  // 16 banks max (1–16)
#else
  if (newBank > 8) newBank = 1;   // 8 banks max (1–8)
#endif

  // Update bank/jiffy if changed
  if (newBank != currentBank) setBank(newBank);
  if (newJiffyState != jiffyState) setJiffy(newJiffyState);

  lastCommand = 0;  // Clear processed command
}

// --- Reset Handler (Long-press for Menu) ---
void pollReset() {
  if (digitalRead(PIN_RESET_MON) == LOW) {
    if (resetHoldStart == 0) {
      resetHoldStart = millis();
    } else if (millis() - resetHoldStart >= RESET_HOLD_MS) {
      switchToMenu();  // Long press detected
      resetHoldStart = 0;
    }
  } else {
    resetHoldStart = 0;
  }
}

// --- Bank/Jiffy Control Functions ---
void setBank(uint8_t bank) {
  if (bank == currentBank) return;  // Skip if no change
  currentBank = bank;
  applyBank(bank);
  EEPROM.update(EEPROM_ADDR_BANK, bank);  // Safer write
}

void setJiffy(bool state) {
  if (state == jiffyState) return;  // Skip if no change
  jiffyState = state;
  applyJiffy(state);
  EEPROM.update(EEPROM_ADDR_JIFFY, state ? 1 : 0);
}

void applyBank(uint8_t bank) {
  digitalWrite(PIN_A15, (bank & 0x01) ? HIGH : LOW);
  digitalWrite(PIN_A16, (bank & 0x02) ? HIGH : LOW);
  digitalWrite(PIN_A17, (bank & 0x04) ? HIGH : LOW);
#if USE_SST39SF040
  digitalWrite(PIN_A18, (bank & 0x08) ? HIGH : LOW);
#else
  digitalWrite(PIN_A18, LOW);  // Unused for SST39SF020A
#endif
}

void applyJiffy(bool state) {
  digitalWrite(PIN_JIFFY_CTRL, state ? HIGH : LOW);
}

// --- Special Menu Bank (0) ---
void switchToMenu() {
  currentBank = 0;
  applyBank(0);
  EEPROM.update(EEPROM_ADDR_BANK, 0);
}
