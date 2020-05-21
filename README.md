# ESP8266 alternative deep sleep

I've got some problems getting deep sleep running on an ESP-01 module. The cable was correctly soldered like described [here](https://www.instructables.com/id/Enable-DeepSleep-on-an-ESP8266-01/). My ESP did go to sleep when calling `ESP.deepSleep(3e6)`, but did not wake up. As I bought it from China, it might be a knock-off which does not satisfy all the official specs.

After some search I found the solution in a [GitHub Issue](https://github.com/esp8266/Arduino/issues/6007#issuecomment-578811346): 

```c
#define ets_wdt_disable ((void (*)(void))0x400030f0)
#define ets_delay_us ((void (*)(int))0x40002ecc)

#define _R (uint32_t *)0x60000700
void nk_deep_sleep(uint64_t time)
{
	ets_wdt_disable();
	*(_R + 4) = 0;
	*(_R + 17) = 4;
	*(_R + 1) = *(_R + 7) + 5;
	*(_R + 6) = 8;
	*(_R + 2) = 1 << 20;
	ets_delay_us(10);
	*(_R + 39) = 0x11;
	*(_R + 40) = 3;
	*(_R) &= 0xFCF;
	*(_R + 1) = *(_R + 7) + (45*(time >> 8));
	*(_R + 16) = 0x7F;
	*(_R + 2) = 1 << 20;
	__asm volatile ("waiti 0");
}
```

After using the function described there it worked very well and my ESP woke up as expected.

## Test it

This repository contains a [PlatformIO project](https://platformio.org/), which you can use to check if this also works for you. If you are not using an ESP-01, you might need to adjust [platformio.ini](platformio.ini). The important bits are in [src/main.cpp](src/main.cpp).