#include "cpp_main.hpp"

#include "ARGB.h"
#include "adc.h"
#include "bh1750.hpp"
#include "bno055_stm32.h"
#include "can.h"
#include "can_lib.hpp"
#include "encoder.hpp"
#include "gpio.h"
#include "hcsr04.hpp"
#include "i2c.h"
#include "led_strip.hpp"
#include "main.h"
#include "stm32l4xx_ll_adc.h"
#include "tim.h"
#include "usb_com_port.hpp"

uint8_t rx_buffer[64];

int cpp_main() {
  /* Initialization */
  bno055_assignI2C(&hi2c1);
  bno055_setup();
  bno055_setOperationModeNDOF();

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);  // Calibrate ADC
  // HAL_ADC_Start(&hadc1);                                        // Start ADC
  // HAL_ADC_Start_IT(&hadc1);

  // Start TIM16 for 0.5ms clock
  // HAL_TIM_Base_Start_IT(&htim16);

  // hcsr04_init(/*&htim2*/);
  HCSR04 *sensor_1 = new HCSR04(&htim1, TIM_Channel_1, TRIG_1_GPIO_Port, TRIG_1_Pin);
  HCSR04 *sensor_2 = new HCSR04(&htim2, TIM_Channel_3, TRIG_2_GPIO_Port, TRIG_2_Pin);
  HCSR04 *sensor_3 = new HCSR04(&htim1, TIM_Channel_2, TRIG_3_GPIO_Port, TRIG_3_Pin);
  HCSR04 *sensor_4 = new HCSR04(&htim2, TIM_Channel_1, TRIG_4_GPIO_Port, TRIG_4_Pin);
  HCSR04 *sensor_5 = new HCSR04(&htim2, TIM_Channel_4, TRIG_5_GPIO_Port, TRIG_5_Pin);
  HCSR04 *sensor_6 = new HCSR04(&htim2, TIM_Channel_2, TRIG_6_GPIO_Port, TRIG_6_Pin);

  Encoder *encoder_1 = new Encoder(ENCODER_1_A_GPIO_Port, ENCODER_1_A_Pin, ENCODER_1_B_GPIO_Port, ENCODER_1_B_Pin);
  Encoder *encoder_2 = new Encoder(ENCODER_2_A_GPIO_Port, ENCODER_2_A_Pin, ENCODER_2_B_GPIO_Port, ENCODER_2_B_Pin);
  Encoder *encoder_3 = new Encoder(ENCODER_3_A_GPIO_Port, ENCODER_3_A_Pin, ENCODER_3_B_GPIO_Port, ENCODER_3_B_Pin);
  Encoder *encoder_4 = new Encoder(ENCODER_4_A_GPIO_Port, ENCODER_4_A_Pin, ENCODER_4_B_GPIO_Port, ENCODER_4_B_Pin);

  BH1750 *light_sensor = new BH1750(&hi2c1, BH1750::DEFAULT_ADDRESS);

  // CAN Test
  CAN_LIB *can = new CAN_LIB(&hcan1);
  HAL_StatusTypeDef can_status = can->drive(0, -100, 0);
  printf("CAN Status: %d\n", can_status);

  // For capture_test:
  // HAL_TIM_Base_Start_IT(&htim2);
  // HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

  // LED STRIP
  LED_STRIP::init();
  LED_STRIP::clear_and_update();
  LED_STRIP::set_brightness(50);

  HAL_GPIO_WritePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin, GPIO_PIN_SET);

  // LED_STRIP::driving_light();
  // LED_STRIP::blink({255, 255, 0, 0}, 50, 5, 4);
  // LED_STRIP::beacon_rgbw({0, 0, 255, 0}, 1, NUM_PIXELS, 5, 2, false);

  printf("HI (:\n");

  uint8_t secs = 0;
  bool drive = true;

  // POZYX
  // uint8_t pozyx_data;
  // HAL_StatusTypeDef pozyx_status_write;
  // HAL_StatusTypeDef pozyx_status_read;

  while (1) {
    /* Super loop */
    printf("\n");

    bno055_vector_t v = bno055_getVectorEuler();
    printf("Roll: %.2f Pitch: %.2f Yaw: %.2f\n", v.x, v.y, v.z);
    v = bno055_getVectorQuaternion();
    printf("W: %.2f X: %.2f Y: %.2f Z: %.2f\n", v.w, v.x, v.y, v.z);
    int8_t temp = bno055_getTemp();
    printf("Temp: %d°C\n", temp);

    // hcsr04_read();
    // printf("d = %d cm - %d us\n ", distance, difference);
    sensor_1->read();
    HAL_Delay(10);
    sensor_2->read();
    HAL_Delay(10);
    sensor_3->read();
    HAL_Delay(10);
    sensor_4->read();
    HAL_Delay(10);
    sensor_5->read();
    HAL_Delay(10);
    sensor_6->read();
    HAL_Delay(10);
    printf("sensor_1 = %.2f : sensor_2 = %.2f : sensor_3 = %.2f : sensor_4 = %.2f : sensor_5 = %.2f : sensor_6 = %.2f\n",
           sensor_1->get_distance(), sensor_2->get_distance(), sensor_3->get_distance(), sensor_4->get_distance(), sensor_5->get_distance(), sensor_6->get_distance());

    printf("Encoder 1: %d - Encoder 2: %d - Encoder 3: %d - Encoder 4: %d\n", encoder_1->get_counter(), encoder_2->get_counter(), encoder_3->get_counter(), encoder_4->get_counter());

    printf("Light %f lux\n", light_sensor->read_light());

    HAL_ADC_Start(&hadc1);  // Start ADC
    HAL_ADC_PollForConversion(&hadc1, 1);
    uint32_t adc_res = HAL_ADC_GetValue(&hadc1);
    // float U = (adc_res) * (3.34 / 61440);  // 3.34 => V_ref ; 61440 = 2^16 - 2^12

    float U = __LL_ADC_CALC_DATA_TO_VOLTAGE(3300UL, adc_res, LL_ADC_RESOLUTION_12B) * 0.001;
    printf("adc_res :%ld U: %f\n", adc_res, U);
    // HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

    printf("rx_buffer: %s\n", rx_buffer);
    USB_COM_PORT::send_str("Hello, World!\n");
    HAL_Delay(10);
    USB_COM_PORT::send_data(rx_buffer, 64);

    if (secs >= 2) {
      // MOSFET
      // HAL_GPIO_TogglePin(ONOFF_POZYX_GPIO_Port, ONOFF_POZYX_Pin);
      // HAL_GPIO_TogglePin(ONOFF_LED_STRIP_GPIO_Port, ONOFF_LED_STRIP_Pin);

      // CAN
      if (!drive) {
        can_status = can->send_stop();
        drive = true;
      } else {
        can_status = can->drive(0, -100, 0);
        drive = false;
      }

      printf("CAN Status: %d\n", can_status);
      secs = 0;
    }
    secs++;

    // POZYX
    // uint8_t reg = 0x0;
    // pozyx_status_write = HAL_I2C_Master_Transmit(&hi2c1, 0x4B << 1, &reg, 1, 100);
    // pozyx_status_read = HAL_I2C_Master_Receive(&hi2c1, 0x4B << 1, &pozyx_data, 1, 100);
    // printf("Pozyx Data: 0x%02x with status_write: %d status_read: %d\n", pozyx_data, pozyx_status_write, pozyx_status_read);

    HAL_Delay(1000);
  }
}