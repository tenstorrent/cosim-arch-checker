// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include "mon_instr.h"

class cCosimEnv {

public:
cCosimEnv();

void initial();
void final();

private:
cMonInstr *monInstr;
};
