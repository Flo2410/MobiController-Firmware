#include "bootloader.hpp"

#include "adc.h"
#include "can.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "stm32l452xx.h"
#include "stm32l4xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"

void JumpToBootloader(void) {
  void (*SysMemBootJump)(void);
  uint8_t i;

  __disable_irq();
  // Reset USB
  USB->CNTR = 0x0003;

  // De-init all peripherals
  HAL_ADC_DeInit(&hadc1);
  HAL_TIM_PWM_DeInit(&htim1);
  HAL_TIM_Base_DeInit(&htim1);
  HAL_TIM_Base_DeInit(&htim6);
  HAL_UART_DeInit(&huart2);
  HAL_CAN_DeInit(&hcan1);

  // Disable Systick
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  // Reset clock to default
  HAL_RCC_DeInit();

  // Clear all interrupt bits
  for (i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  __enable_irq();

  SysMemBootJump = (void (*)(void))(*((uint32_t *)(BOOT_ADDR + 4)));
  __set_MSP(*(uint32_t *)BOOT_ADDR);
  SysMemBootJump();

  while (1)
    ;  // Just in case...
}