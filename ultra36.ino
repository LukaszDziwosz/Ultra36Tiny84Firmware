#include "pin_map.h"
#include "config.h"

uint8_t currentBank = 0;
bool jiffyState = false;
bool isROM040 = false;

unsigned long resetHoldStart = 0;

void setup() {
  // Set bank pins as output
  pinMode(PIN_A15, OUTPUT);
  pinMode(PIN_A16, OUTPUT);
  pinMode(PIN_A17, OUTPUT);
  pinMode(PIN_A18, OUTPUT);

  // Inputs
  pinMode(PIN_RESET_MON, INPUT_PULLUP);
  pinMode(PIN_TRAP_MON, INPUT_PULLUP);
  pinMode(PIN_TYPE_JUMPER, INPUT_PULLUP);

  // Output for Jiffy
  pinMode(PIN_JIFFY_CTRL, OUTPUT);

  // Determine ROM size from jumper
  isROM040 = (digitalRead(PIN_TYPE_JUMPER) == LOW);

  // Load last saved state
  currentBank = EEPROM.read(EEPROM_ADDR_BANK);
  jiffyState = EEPROM.read(EEPROM_ADDR_JIFFY) != 0;

  applyBank(currentBank);
  applyJiffy(jiffyState);
}

void loop() {
  pollReset();
  pollTrap(); // $D700 I/O commands from C128
  delay(POLL_INTERVAL_MS);
}

void pollReset() {
  bool pressed = digitalRead(PIN_RESET_MON) == LOW;

  if (pressed) {
    if (resetHoldStart == 0) {
      resetHoldStart = millis();
    } else if (millis() - resetHoldStart >= RESET_HOLD_MS) {
      // Long press detected
      switchToMenu();
      resetHoldStart = 0;
    }
  } else {
    resetHoldStart = 0;
  }
}

void pollTrap() {
  if (digitalRead(PIN_TRAP_MON) == LOW) {
    // Trap line pulled low → trigger handling
    // e.g. read command byte (over time-sliced I/O), then switch bank/jiffy
    // Custom to $D700 protocol
  }
}

void applyBank(uint8_t bank) {
  digitalWrite(PIN_A15, bank & 0x01);
  digitalWrite(PIN_A16, (bank >> 1) & 0x01);
  digitalWrite(PIN_A17, (bank >> 2) & 0x01);
  if (isROM040) {
    digitalWrite(PIN_A18, (bank >> 3) & 0x01);
  }
}

void applyJiffy(bool state) {
  digitalWrite(PIN_JIFFY_CTRL, state ? HIGH : LOW);
}

void switchToMenu() {
  currentBank = 0;
  applyBank(currentBank);
  EEPROM.write(EEPROM_ADDR_BANK, currentBank);
}

void setBank(uint8_t bank) {
  currentBank = bank;
  applyBank(bank);
  EEPROM.write(EEPROM_ADDR_BANK, bank);
}

void toggleJiffy() {
  jiffyState = !jiffyState;
  applyJiffy(jiffyState);
  EEPROM.write(EEPROM_ADDR_JIFFY, jiffyState ? 1 : 0);
}