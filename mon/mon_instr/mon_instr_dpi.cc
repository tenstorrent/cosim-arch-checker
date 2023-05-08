// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include <map>
#include <string>

#include "mon_instr.h"

extern "C" {

static std::map<std::string, cMonInstr *> intfs;

void instr_register_intf(std::string name, cMonInstr *intf) {
  intfs.insert({name, intf});
}

void instr_deregister_intf(std::string name) { intfs.erase(name); }

void monitor_instr(char *name, int hart, std::uint64_t cycle, std::uint64_t tag, std::uint64_t pc, std::uint64_t opcode, std::uint32_t trap) {
  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->monRetire(hart, cycle, tag, pc, opcode, trap);
  }
}

void monitor_gpr(char *name, int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->monGpr(hart, cycle, addr, data);
  }
}

void monitor_fpr(char *name, int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->monFpr(hart, cycle, addr, data);
  }
}

void monitor_vr(char *name, int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t *data) {
  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->monVr(hart, cycle, addr, data);
  }
}

void monitor_csr(char *name, int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  auto ptr = intfs.find(name);
  if (ptr != intfs.end()) {
    ptr->second->monCsr(hart, cycle, addr, data);
  }
}
}
