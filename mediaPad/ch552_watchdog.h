// ch552_watchdog.h
// ------------------------------------------------------------
// CH552 Watchdog Timer
// ------------------------------------------------------------
//
// The watchdog timeout period is determined by the following formula:
//
//      wdt_sec = 65536 * (256 - WDOG_COUNT) / Fsys
//
// Rearranging the formula to obtain WDOG_COUNT from a desired
// timeout in milliseconds:
//
//      WDOG_COUNT = 256 - ((Fsys / 1000) * wdt_msec) / 65536
//
// When the watchdog timer overflows, the CPU resets after
// Trdl (thermal reset delay, 45usec Typ.).
//
// ------------------------------------------------------------
// Precomputed values
// ------------------------------------------------------------
//
// To avoid runtime division or multiplication, the WDOG_COUNT
// value is precomputed for a watchdog timeout of 500 ms.
//
//      WDOG_COUNT = 256 - ((F_CPU / 1000) * 500) / 65536
//
// ------------------------------------------------------------

// Predefined WDOG_COUNT values for a 500 ms timeout
#if F_CPU == 12000000
	#define WDT_TIMEOUT	165
#elif F_CPU == 16000000	
	#define WDT_TIMEOUT	134
#elif F_CPU == 24000000
	#define WDT_TIMEOUT	73
#else
	#error F_CPU not supported.
#endif

void wdt_enable(uint8_t timeout);
void wdt_disable();
#define wdt_update()	WDOG_COUNT = WDT_TIMEOUT;
