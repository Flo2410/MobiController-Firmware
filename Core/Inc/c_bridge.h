#ifndef C_BRIDGE_H_
#define C_BRIDGE_H_

#ifdef __cplusplus
// make the method callable from C

#include "stm32l452xx.h"

extern "C" {
void mobictl_handle_commands(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload);
}
#endif

#endif