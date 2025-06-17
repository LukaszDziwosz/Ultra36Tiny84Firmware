#ifndef CONFIG_H
#define CONFIG_H

#define EEPROM_ADDR_BANK     0
#define EEPROM_ADDR_JIFFY    1

// Set to 1 for 512KB SST39SF040 (16 banks), 0 for 256KB SST39SF020A (8 banks)
#define USE_SST39SF040       1

// Reset detection
#define RESET_HOLD_MS        3000
#define POLL_INTERVAL_MS     10

// Command masks
#define ROM_BANK_MASK        0x0F  // Bits 0–3
#define JIFFY_MASK           0x10  // Bit 4

#endif
