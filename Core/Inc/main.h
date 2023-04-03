/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ONBOARD_BTN_Pin GPIO_PIN_13
#define ONBOARD_BTN_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define POTYX_INT1_Pin GPIO_PIN_0
#define POTYX_INT1_GPIO_Port GPIOC
#define POTYX_INT1_EXTI_IRQn EXTI0_IRQn
#define ONOFF_LED_STRIP_Pin GPIO_PIN_1
#define ONOFF_LED_STRIP_GPIO_Port GPIOC
#define TRIG_6_Pin GPIO_PIN_2
#define TRIG_6_GPIO_Port GPIOC
#define ONOFF_POZYX_Pin GPIO_PIN_3
#define ONOFF_POZYX_GPIO_Port GPIOC
#define ECHO_4_Pin GPIO_PIN_0
#define ECHO_4_GPIO_Port GPIOA
#define ECHO_6_Pin GPIO_PIN_1
#define ECHO_6_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SMPS_EN_Pin GPIO_PIN_4
#define SMPS_EN_GPIO_Port GPIOA
#define SMPS_V1_Pin GPIO_PIN_5
#define SMPS_V1_GPIO_Port GPIOA
#define SMPS_PG_Pin GPIO_PIN_6
#define SMPS_PG_GPIO_Port GPIOA
#define SMPS_SW_Pin GPIO_PIN_7
#define SMPS_SW_GPIO_Port GPIOA
#define TRIG_1_Pin GPIO_PIN_4
#define TRIG_1_GPIO_Port GPIOC
#define ENCODER_2_A_Pin GPIO_PIN_1
#define ENCODER_2_A_GPIO_Port GPIOB
#define ENCODER_2_A_EXTI_IRQn EXTI1_IRQn
#define TRIG_3_Pin GPIO_PIN_2
#define TRIG_3_GPIO_Port GPIOB
#define ECHO_2_Pin GPIO_PIN_10
#define ECHO_2_GPIO_Port GPIOB
#define ECHO_5_Pin GPIO_PIN_11
#define ECHO_5_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOB
#define POZYX_INT2_Pin GPIO_PIN_14
#define POZYX_INT2_GPIO_Port GPIOB
#define POZYX_INT2_EXTI_IRQn EXTI15_10_IRQn
#define ENCODER_3_A_Pin GPIO_PIN_15
#define ENCODER_3_A_GPIO_Port GPIOB
#define ENCODER_3_A_EXTI_IRQn EXTI15_10_IRQn
#define ENCODER_3_B_Pin GPIO_PIN_6
#define ENCODER_3_B_GPIO_Port GPIOC
#define ENCODER_1_A_Pin GPIO_PIN_7
#define ENCODER_1_A_GPIO_Port GPIOC
#define ENCODER_1_A_EXTI_IRQn EXTI9_5_IRQn
#define ENCODER_4_B_Pin GPIO_PIN_8
#define ENCODER_4_B_GPIO_Port GPIOC
#define ECHO_1_Pin GPIO_PIN_8
#define ECHO_1_GPIO_Port GPIOA
#define ECHO_3_Pin GPIO_PIN_9
#define ECHO_3_GPIO_Port GPIOA
#define ENCODER_4_A_Pin GPIO_PIN_10
#define ENCODER_4_A_GPIO_Port GPIOA
#define ENCODER_4_A_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TRIG_2_Pin GPIO_PIN_15
#define TRIG_2_GPIO_Port GPIOA
#define ENCODER_2_B_Pin GPIO_PIN_10
#define ENCODER_2_B_GPIO_Port GPIOC
#define ENCODER_1_B_Pin GPIO_PIN_11
#define ENCODER_1_B_GPIO_Port GPIOC
#define TRIG_4_Pin GPIO_PIN_12
#define TRIG_4_GPIO_Port GPIOC
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LED_OUT_Pin GPIO_PIN_4
#define LED_OUT_GPIO_Port GPIOB
#define TRIG_5_Pin GPIO_PIN_5
#define TRIG_5_GPIO_Port GPIOB
#define USER_BTN_Pin GPIO_PIN_8
#define USER_BTN_GPIO_Port GPIOB
#define USER_BTN_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
