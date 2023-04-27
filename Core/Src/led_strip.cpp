
#include "led_strip.hpp"

#include "etl/scaled_rounding.h"
#include "tim.h"

namespace LED_STRIP {
/*
  ARGB Overwrites
*/

void init() {
  ARGB_Init();
  HAL_TIM_Base_Start_IT(&htim6);
}

void clear() {
  ARGB_Clear();
}

void set_brightness(uint8_t brightness) {
  ARGB_SetBrightness(brightness);
}

void set_rgb(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  ARGB_SetRGB(index, r, g, b);
}

void set_hsv(uint16_t index, uint8_t hue, uint8_t sat, uint8_t val) {
  ARGB_SetHSV(index, hue, sat, val);
}

void set_white(uint16_t index, uint8_t w) {
  ARGB_SetWhite(index, w);
}

void fill_rgb(uint8_t r, uint8_t g, uint8_t b) {
  ARGB_FillRGB(r, g, b);
}

void fill_hsv(uint8_t hue, uint8_t sat, uint8_t val) {
  ARGB_FillHSV(hue, sat, val);
}

void fill_white(uint8_t w) {
  ARGB_FillWhite(w);
}

// ----------------------------------------------------------------------------

/*
  Custom led functions
*/

void set_rgbw(uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  set_rgb(index, r, g, b);
  set_white(index, w);
}

void set_rgbw(uint16_t index, COLOR_RGBW color) {
  set_rgbw(index, color.r, color.g, color.b, color.w);
}

void fill_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  // fill_rgb(r, g, b);
  // fill_white(w);
  fill_range_rgbw(0, NUM_PIXELS, r, g, b, w);
}

void fill_rgbw(COLOR_RGBW color) {
  fill_rgbw(color.r, color.g, color.b, color.w);
}

void fill_range_rgbw(uint16_t start, uint16_t end, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (volatile uint16_t i = start; i < end; i++)
    set_rgbw(i, r, g, b, w);
}

void fill_range_rgbw(uint16_t start, uint16_t end, COLOR_RGBW color) {
  fill_range_rgbw(start, end, color.r, color.g, color.b, color.w);
}

void update() {
  while (!ARGB_Show()) {
  }
}

void clear_and_update() {
  clear();
  update();
}

// ----------------------------------------------------------------------------

/*
  Interrupt
*/

void handle_timer_interrupt(TIM_HandleTypeDef* htim) {
  if (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1 && htim != &htim6) return;
  if (!do_animation) return;

  if (current_animation.update_rate == 0) stop_animation();  // if update_rate is 0, animate the first frame then stop the animation

  if (count_to_10ms < current_animation.update_rate) {
    count_to_10ms++;
    return;
  }

  count_to_10ms = 0;

  // Do animation
  switch (current_animation.type) {
    case ANIMATION_TYPE::BLINK: {
      if (current_frame == 0) {
        // fill_rgbw(current_animation.color);
        uint8_t offset = etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.line_count);
        for (size_t i = 0; i < current_animation.line_count; i++) {
          uint8_t start = (i * offset) + current_frame * etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.frame_count);
          uint8_t end = start + current_animation.line_length;
          fill_range_rgbw(start, end, current_animation.color);
        }
      }
      if (current_frame == 1) clear();
      update();

      break;
    }
    case ANIMATION_TYPE::ROTATE_RIGHT: {
      clear();

      uint8_t offset = etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.line_count);
      for (size_t i = 0; i < current_animation.line_count; i++) {
        // offset between the lines + the current frame * NUM_PIXELS / frame_count
        // offset -> spacing
        // current frame -> movement
        // NUM_PIXELS / frame_count -> scaling to the whole led strip
        uint8_t start = (i * offset) + current_frame * etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.frame_count);
        uint8_t end = start + current_animation.line_length;
        fill_range_rgbw(start, end, current_animation.color);
      }

      update();
      break;
    }

    case ANIMATION_TYPE::ROTATE_LEFT: {
      clear();

      uint8_t offset = etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.line_count);
      for (size_t i = 0; i < current_animation.line_count; i++) {
        // uint8_t start = (i * offset) + NUM_PIXELS - (current_frame * etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.frame_count));
        uint8_t start = (i * offset) + (NUM_PIXELS - 1) - current_frame * etl::round_half_up_unscaled<10, uint8_t>(NUM_PIXELS * 10 / current_animation.frame_count);
        uint8_t end = start + current_animation.line_length;
        fill_range_rgbw(start, end, current_animation.color);
      }

      update();

      break;
    }
    default:
      break;
  }

  current_frame++;
  if (current_frame > current_animation.frame_count - 1) current_frame = 0;
}

// ----------------------------------------------------------------------------

/*
  Animations
*/

void stop_animation() {
  do_animation = false;
}

void start_animation(ANIMATION_CONFIG animation) {
  if ((animation.type == ANIMATION_TYPE::BLINK && animation.line_count * animation.line_length > NUM_PIXELS) ||
      (animation.type != ANIMATION_TYPE::BLINK && animation.line_count * animation.line_length >= NUM_PIXELS)) {
    stop_animation();
    fill_rgbw(animation.color);
    return;
  }

  current_animation = animation;
  do_animation = true;
}

void driving_light() {
  uint8_t quater = NUM_PIXELS / 4;

  fill_range_rgbw(0, quater, 0, 0, 0, 255);                       // Head light
  fill_range_rgbw(2 * quater + 1, 3 * quater + 1, 255, 0, 0, 0);  // Tail light
  update();
}

void battery_warning_light() {
  blink({255, 0, 0, 0}, 10, NUM_PIXELS, 1);
}

void beacon_rgbw(COLOR_RGBW color, uint8_t update_rate, uint8_t frame_count, uint8_t line_length, uint8_t line_count, bool rotate_left) {
  ANIMATION_CONFIG animation;
  animation.type = rotate_left ? ANIMATION_TYPE::ROTATE_LEFT : ANIMATION_TYPE::ROTATE_RIGHT;
  animation.color = color;
  animation.frame_count = frame_count;
  animation.update_rate = update_rate;
  animation.line_length = line_length;
  animation.line_count = line_count;

  start_animation(animation);
}

void blink(COLOR_RGBW color, uint8_t update_rate, uint8_t line_length, uint8_t line_count) {
  ANIMATION_CONFIG animation;
  animation.type = ANIMATION_TYPE::BLINK;
  animation.frame_count = 2;
  animation.color = color;
  animation.update_rate = update_rate;
  animation.line_length = line_length;
  animation.line_count = line_count;

  start_animation(animation);
}

}  // namespace LED_STRIP
