// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include "cosim_env.h"

cCosimEnv::cCosimEnv ()
  : monInstr(new cMonInstr("mon_instr")) {}

void cCosimEnv::initial() {
  monInstr->initial();
}

void cCosimEnv::final() {
  monInstr->final();
  delete monInstr;
}
