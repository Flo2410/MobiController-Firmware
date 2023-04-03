#include "cpp_main.hpp"

#include "MobiController.hpp"
#include "main.h"
#include "stdio.h"
#include "stm32l452xx.h"

uint8_t rx_buffer[64];

int cpp_main() {
  printf("\n----------------------------------------\n");
  printf("DEBUG INTERFACE!\n");
  printf("MobiController Firmware\n");
  printf("version 0.1\n");
  printf("Copyright FHWN Florian Hye\n");
  printf("----------------------------------------\n\n");

  MobiController *mobictl = new MobiController();

  while (1) {
    /* Super loop */
  }
}