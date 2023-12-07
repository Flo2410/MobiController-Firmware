#include "cpp_main.hpp"

#include "MobiController.hpp"
#include "main.h"
#include "min.h"
#include "stdio.h"
#include "stm32l452xx.h"

uint8_t rx_buffer[265];
struct min_context min_ctx;

int cpp_main() {
  printf("\n---------------------------------------------\n");
  printf("DEBUG INTERFACE!\n");
  printf("MobiController Firmware\n");
  printf("version %s\n", FIRMWARE_VERSION);
  printf("Time of compilation: %s %s\n", __DATE__, __TIME__);
  printf("Compiler version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
  printf("Copyright FHWN Florian Hye\n");
  printf("---------------------------------------------\n");

#ifdef MIN_DEBUG_PRINTING
  printf("MIN_DEBUG_PRINTING: enabled\n");
#else
  printf("MIN_DEBUG_PRINTING: disabled\n");
#endif

#ifdef MOBI_DEBUG
  printf("MOBI_DEBUG: enabled\n");
#else
  printf("MOBI_DEBUG: disabled\n");
#endif

  printf("---------------------------------------------\n\n");

  while (1) {
    MobiController::mobictl().loop();
  }
}