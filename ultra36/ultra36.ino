#include "pin_map.h"
#include "config.h"

uint8_t currentBank = 0;
bool jiffyState = false;
unsigned long resetHoldStart = 0;
bool trapLastState = HIGH;

void setup() {
  // Outputs
  pinMode(PIN_A15, OUTPUT);
  pinMode(PIN_A16, OUTPUT);
  pinMode(PIN_A17, OUTPUT);
  pinMode(PIN_A18, OUTPUT);
  pinMode(PIN_JIFFY_CTRL, OUTPUT);

  // Inputs
  pinMode(PIN_RESET_MON, INPUT_PULLUP);
  pinMode(PIN_TRAP_MON, INPUT_PULLUP);
  pinMode(PIN_D0, INPUT);
  pinMode(PIN_D1, INPUT);
  pinMode(PIN_D2, INPUT);
  pinMode(PIN_D3, INPUT);
  pinMode(PIN_D4, INPUT);

  // Load saved config
  currentBank = EEPROM.read(EEPROM_ADDR_BANK);
  jiffyState = EEPROM.read(EEPROM_ADDR_JIFFY) != 0;

 // Validate bank number
  #if USE_SST39SF040
    if (currentBank > 15) currentBank = 0;  // 16 banks (0-15)
  #else
    if (currentBank > 7) currentBank = 0;   // 8 banks (0-7)
  #endif

  applyBank(currentBank);
  applyJiffy(jiffyState);

  trapLastState = digitalRead(PIN_TRAP_MON);
}

void loop() {
  pollReset();
  pollTrap();
  delay(POLL_INTERVAL_MS);
}

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

void pollTrap() {
  bool trapCurrent = digitalRead(PIN_TRAP_MON);
  if (trapLastState == HIGH && trapCurrent == LOW) {
    handleTrapCommand();
  }
  trapLastState = trapCurrent;
}

void handleTrapCommand() {
  uint8_t command = readCommandByte();
  uint8_t newBank = command & ROM_BANK_MASK;
  bool newJiffyState = (command & JIFFY_MASK) != 0;

   // Validate bank range
  #if USE_SST39SF040
    if (newBank > 15) newBank = 0;  // 16 banks max
  #else
    if (newBank > 7) newBank = 0;   // 8 banks max  
  #endif

  if (newBank != currentBank) setBank(newBank);
  if (newJiffyState != jiffyState) setJiffy(newJiffyState);
}

uint8_t readCommandByte() {
  uint8_t cmd = 0;
  if (digitalRead(PIN_D0)) cmd |= 0x01;
  if (digitalRead(PIN_D1)) cmd |= 0x02;
  if (digitalRead(PIN_D2)) cmd |= 0x04;
  if (digitalRead(PIN_D3)) cmd |= 0x08;
  if (digitalRead(PIN_D4)) cmd |= 0x10;
  return cmd;
}

void setBank(uint8_t bank) {
  currentBank = bank;
  applyBank(bank);
  EEPROM.write(EEPROM_ADDR_BANK, bank);
}

void setJiffy(bool state) {
  jiffyState = state;
  applyJiffy(state);
  EEPROM.write(EEPROM_ADDR_JIFFY, state ? 1 : 0);
}

void applyBank(uint8_t bank) {
  digitalWrite(PIN_A15, (bank & 0x01) ? HIGH : LOW);
  digitalWrite(PIN_A16, (bank & 0x02) ? HIGH : LOW);
  digitalWrite(PIN_A17, (bank & 0x04) ? HIGH : LOW);

#if USE_SST39SF040
  digitalWrite(PIN_A18, (bank & 0x08) ? HIGH : LOW);
#else
  digitalWrite(PIN_A18, LOW);
#endif
}

void applyJiffy(bool state) {
  digitalWrite(PIN_JIFFY_CTRL, state ? HIGH : LOW);
}

void switchToMenu() {
  setBank(0);
}