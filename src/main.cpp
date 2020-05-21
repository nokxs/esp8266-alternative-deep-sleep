
 #include <Arduino.h>

#define STATUS_LED  2   

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
 
void setup() {  
  pinMode(STATUS_LED, OUTPUT);
  
  Serial.begin(115200);
  Serial.setTimeout(2000);

  digitalWrite(STATUS_LED, false);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.print("Chip ID: ");
  Serial.println(ESP.getChipId());

  Serial.println(".");  
  Serial.println("I'm awake.");

  delay(1000);
  digitalWrite(STATUS_LED, true);

  Serial.println("Going into deep sleep for 3 seconds");
  delay(200);

  //ESP.deepSleep(3e6); // This did not work with my ESP-01 board
  nk_deep_sleep(3e6); // Use this function instead the normal deep sleep function
  
  delay(100);
}

void loop() {
}