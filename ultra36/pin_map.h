#ifndef PIN_MAP_H
#define PIN_MAP_H

// === Bank Address Pins (Outputs) ===
// Physical pin mapping for ATtiny84A
#define PIN_A15        11   // PA0 (Physical pin 13) - Bank bit 0
#define PIN_A16         8   // PA1 (Physical pin 12) - Bank bit 1
#define PIN_A17         9   // PA2 (Physical pin 11) - Bank bit 2
#define PIN_A18        10   // PA3 (Physical pin 10) - Bank bit 3

// === Control Input Pins ===
#define PIN_RESET_MON   3   // PB3 (Physical pin 4) - Reset monitoring
#define PIN_TRAP_MON    1   // PB1 (Physical pin 3) - $D700 trap detection

// === Control Output Pins ===
#define PIN_JIFFY_CTRL  2   // PB2 (Physical pin 5) - JiffyDOS control

// === Data Bus Input Pins (from U36 socket) ===
#define PIN_D0          7   // PA7 (Physical pin 6) - Data bit 0
#define PIN_D1          6   // PA6 (Physical pin 7) - Data bit 1
#define PIN_D2          5   // PA5 (Physical pin 8) - Data bit 2
#define PIN_D3          4   // PA4 (Physical pin 9) - Data bit 3
#define PIN_D4          0   // PB0 (Physical pin 2) - Data bit 4 (JiffyDOS)

#endif
