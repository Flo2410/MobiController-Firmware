#ifndef MOBI_CONTROLLER_H_
#define MOBI_CONTROLLER_H_

#include "min.h"

class MobiController {
 public:
  MobiController(min_context *min_ctx);

  void loop();

 private:
  min_context *min_ctx;

  ~MobiController();
};

#endif
