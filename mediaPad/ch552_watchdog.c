#include "Arduino.h"

// Some SFRs can be written only in safe mode, while they can be read only in unsafe mode, such as:
// GLOBAL_CFG, CLOCK_CFG, WAKE_CTRL.
//
// Some SFRs can only be written in safe mode, while they are always read-only in non-safe mode. Steps for
// entering safe mode:
// (1). Write 55h into this register.
// (2). And then write AAh into this register.
// (3). After that, they are in safe mode for about 13 to 23 system clock cycles, and one or more safe class
// SFR or ordinary SFR can be rewritten in such validity period.
// (4). Automatically terminate the safe mode after the expiration of the above validity period.
// (5). Alternatively, write any value to the register to prematurely terminate safe mode. 
//

void wdt_enable(uint8_t timeout) {
	WDOG_COUNT  = timeout;
	SAFE_MOD    = 0x55;	// Safe mode control register (write only)
	SAFE_MOD    = 0xAA;
	GLOBAL_CFG |= bWDOG_EN;
	SAFE_MOD    = 0;
}

void wdt_disable() {
	WDOG_COUNT  = 0;
	SAFE_MOD    = 0x55;	
	SAFE_MOD    = 0xAA;
	GLOBAL_CFG &= ~bWDOG_EN;
	SAFE_MOD    = 0;
}

