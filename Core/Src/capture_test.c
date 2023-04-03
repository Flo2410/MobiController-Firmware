// #include "capture_test.h"

// #include "stdio.h"
// #include "stm32l4xx.h"

// volatile uint8_t gu8_State = IDLE;
// volatile uint32_t gu32_T1 = 0;
// volatile uint32_t gu32_T2 = 0;
// volatile uint32_t gu32_Ticks = 0;
// volatile uint16_t gu16_TIM2_OVC = 0;
// volatile uint32_t gu32_Freq = 0;

// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
//   if (gu8_State == IDLE) {
//     gu32_T1 = TIM2->CCR1;
//     gu16_TIM2_OVC = 0;
//     gu8_State = DONE;
//   } else if (gu8_State == DONE) {
//     gu32_T2 = TIM2->CCR1;
//     gu32_Ticks = (gu32_T2 + (gu16_TIM2_OVC * 4294967296)) - gu32_T1;
//     gu32_Freq = (uint32_t)(F_CLK / gu32_Ticks);
//     if (gu32_Freq != 0) {
//       printf("Frequency = %lu Hz\n", gu32_Freq);
//     }
//     gu8_State = IDLE;
//   }
// }

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
//   gu16_TIM2_OVC++;
// }
