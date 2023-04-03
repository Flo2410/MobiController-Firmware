#ifndef CPP_MAIN_H_
#define CPP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "stm32l4xx.h"

extern uint8_t rx_buffer[265];
extern struct min_context min_ctx;

int cpp_main();

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*CPP_MAIN_H_*/