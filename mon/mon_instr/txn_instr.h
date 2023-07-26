// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include <vector>
#include "params.h"

struct sPc {
  bool valid;
  uint64_t pc_rdata;

  sPc() {
    clear();
  }

  void clear() {
    valid = false;
  }
};

struct sGpr {
  bool valid;
  uint64_t rd_addr;
  uint64_t rd_wdata;

  sGpr() {
    clear();
  }

  void clear() {
    valid = false;
  }
};
 
struct sFpr {
  bool valid;
  uint64_t frd_addr;
  bool frd_wvalid;
  uint64_t frd_wdata;

  sFpr() {
    clear();
  }

  void clear() {
    valid = false;
  }
};

struct sVr {
  bool valid[8];
  uint32_t vrd_addr[8];
  uint64_t vrd_wdata[8][k_VLen/64];
 
  sVr() {
    clear();
  }

  void clear() {
    for(int i = 0; i < 8; i++) {
      valid[i] = false;
    }
  }
};

struct sCsr {
  bool valid;
  uint32_t csr_count;
  uint32_t csr_addr;
  uint64_t csr_wmask;
  uint64_t csr_wdata;

  sCsr() {
    clear();
  }

  void clear() {
    valid = false;
  }
};

struct sRvInstr {
  // Metadata
  bool valid;
  uint64_t cycle;
  uint64_t tag;
  uint32_t opcode;
  bool trap;

  // Registers
  sPc pc;
  sGpr gpr;
  sFpr fpr;
  sVr vr;
  std::vector<sCsr> csrs;

  void clear() {
    valid = false;
    pc.clear();
    gpr.clear();
    fpr.clear();
    vr.clear();
    csrs.clear();
  }
};
