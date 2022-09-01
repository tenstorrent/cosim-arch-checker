// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include "txn_instr.h"

class cBridgeBase {

public:

  virtual ~cBridgeBase() {}
  
  virtual void processDutInstrRetire(int hart, sRvInstr &dutInstr) = 0;

};
