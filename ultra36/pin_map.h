#ifndef PIN_MAP_H
#define PIN_MAP_H

// === Bank Address Pins (Outputs) ===
#define PIN_A15        PIN_PB0  // PB0 (Physical pin 2)  - Bit0
#define PIN_A16        PIN_PB1  // PB1 (Physical pin 3)  - Bit1
#define PIN_A17        PIN_PA2  // PA2 (Physical pin 11) - Bit2
#define PIN_A18        PIN_PB3  // PB3 (Physical pin 4)  - Bit3

// === Control Input Pins ===
#define PIN_RESET_MON  PIN_PA1  // PA1 (Physical pin 12) - 3s reset monitor
#define PIN_TRAP_MON   PIN_PA3  // PA3 (Physical pin 10) - $D700 trap

// === Control Output Pins ===
#define PIN_JIFFY_CTRL PIN_PA0  // PA0 (Physical pin 13) - Jiffy control - ground

// === Data Bus Input Pins (from U36 socket) ===
#define PIN_D0         PIN_PA7  // PA7 (Physical pin 6)  - Bit0
#define PIN_D1         PIN_PA6  // PA6 (Physical pin 7)  - Bit1
#define PIN_D2         PIN_PB2  // PB2 (Physical pin 5)  - Bit2
#define PIN_D3         PIN_PA4  // PA4 (Physical pin 9)  - Bit3
#define PIN_D4         PIN_PA5  // PA5 (Physical pin 8)  - Bit4 - Jiffy Dos command

#endif
