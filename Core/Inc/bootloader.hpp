#pragma once

#include "stm32l452xx.h"

#define BOOT_ADDR 0x1FFF0000  // my MCU boot code base address

void JumpToBootloader(void);