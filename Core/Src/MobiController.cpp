#include "MobiController.hpp"

#include "min.h"

MobiController::MobiController(min_context *min_ctx) : min_ctx(min_ctx) {
  min_init_context(this->min_ctx, 0);
}

void MobiController::loop() {
  min_poll(this->min_ctx, {}, 0);
}
