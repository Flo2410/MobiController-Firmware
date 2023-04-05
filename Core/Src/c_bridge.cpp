#include "c_bridge.h"

#include "MobiController.hpp"
#include "stm32l452xx.h"

#ifdef __cplusplus
// make the method callable from C
extern "C" {
void mobictl_handle_commands(uint8_t min_id, uint8_t const *min_payload, uint8_t len_payload) {
  MobiController::mobictl().handle_commands(min_id, min_payload, len_payload);
}
}
#endif