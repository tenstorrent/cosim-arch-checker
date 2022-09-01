// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include "cosim_env.h"

extern "C" {

cCosimEnv *env;

void env_init() {
  env = new cCosimEnv();  
  env->initial();
}

void env_final() {
  env->final();
  delete env;
}

}
