#ifndef MOBI_CONTROLLER_H_
#define MOBI_CONTROLLER_H_

#include "min.h"

class MobiController {
 public:
  MobiController();

 private:
  struct min_context min_ctx;

  ~MobiController();
};

#endif
