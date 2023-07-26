// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include <string>

#include "bridge.h"
#include "txn_instr.h"
#include "params.h"
#include "vpi_user.h"

class cMonInstr {

public:
cMonInstr(std::string name);
void initial();
void parseArgs();
void final();

void monRetire(int hart, std::uint64_t cycle, std::uint64_t tag, std::uint64_t pc, std::uint64_t opcode, std::uint32_t trap);
void monGpr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data);
void monFpr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data);
void monVr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t *data, std::uint8_t);
void monCsr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data);

private:
void display(int hart, const sRvInstr &instr);

private:
std::string name;
bool monTracer = false;
sRvInstr rvInstr;
cBridge *bridge;

};
