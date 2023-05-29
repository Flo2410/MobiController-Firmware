#ifndef LED_STRIP_H_
#define LED_STRIP_H_

#include "ARGB.h"
#include "stm32l4xx.h"

namespace LED_STRIP {

/*
  Structs
*/
struct COLOR_RGBW {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

enum ANIMATION_TYPE {
  BLINK = 0,
  ROTATE_LEFT,
  ROTATE_RIGHT,
  FILL
};

struct ANIMATION_CONFIG {
  COLOR_RGBW color;
  uint8_t update_rate;  // multiples of 10ms
  ANIMATION_TYPE type;
  uint8_t frame_count;
  uint8_t line_length;
  uint8_t line_count;
  bool do_loop = true;
};

/*
  Globals
*/
inline bool do_animation = false;
inline uint16_t count_to_10ms = 0;
inline ANIMATION_CONFIG current_animation;
inline uint8_t current_frame = 0;

/*
  ARGB Overwrites
*/
void init();                                                          // Init led strip
void clear();                                                         // Clear strip
void set_brightness(uint8_t brightness);                              // Set global brightness
void set_rgb(uint16_t index, uint8_t r, uint8_t g, uint8_t b);        // Set single LED by RGB
void set_hsv(uint16_t index, uint8_t hue, uint8_t sat, uint8_t val);  // Set single LED by HSV
void set_white(uint16_t index, uint8_t w);                            // Set white component in LED (RGBW)
void fill_rgb(uint8_t r, uint8_t g, uint8_t b);                       // Fill all strip with RGB color
void fill_hsv(uint8_t hue, uint8_t sat, uint8_t val);                 // Fill all strip with HSV color
void fill_white(uint8_t w);                                           // Fill all strip's white component (RGBW)

/*
  Custom led functions
*/
void set_rgbw(uint16_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);                       // Set single LED by RGBW
void set_rgbw(uint16_t index, COLOR_RGBW color);                                                 // Set single LED by RGBW
void fill_rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);                                      // Fill all leds with RGBW colorCore/Inc/led_strip.hpp
void fill_rgbw(COLOR_RGBW color);                                                                // Fill all leds with RGBW color
void fill_range_rgbw(uint16_t start, uint16_t end, uint8_t r, uint8_t g, uint8_t b, uint8_t w);  // Fill a range of leds with RGBW color
void fill_range_rgbw(uint16_t start, uint16_t end, COLOR_RGBW color);                            // Fill a range of leds with RGBW color
void update();                                                                                   // Update the strip
void clear_and_update();

/*
  Interrupt
*/
void handle_timer_interrupt(TIM_HandleTypeDef* htim);

/*
  Animations
*/

void stop_animation();                                                                                                                            // Stop the animation
void start_animation(ANIMATION_CONFIG animation);                                                                                                 // Start the animation, only needed if the animation was stopped by the stop_animation() function;
void driving_light();                                                                                                                             // Car-like head- and taillight
void battery_warning_light();                                                                                                                     // Lights which are used to indicate a critical battery charge
void power_on_animation();                                                                                                                        // Animation thats played when starting up.
void beacon_rgbw(COLOR_RGBW color, uint8_t update_rate, uint8_t frame_count, uint8_t line_length, uint8_t line_count, bool rotate_left = false);  // Beacon light with RGBW color
void blink(COLOR_RGBW color, uint8_t update_rate, uint8_t line_length = NUM_PIXELS, uint8_t line_count = 1);                                      // Blink the whole stip or sections in a color;

}  // namespace LED_STRIP

#endif /* LED_STRIP_H_ */