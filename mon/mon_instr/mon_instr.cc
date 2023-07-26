// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include <iostream>
#include <sstream>
#include <iomanip>

#include "mon_instr.h"

extern "C" void instr_register_intf(std::string name, cMonInstr *intf);
extern "C" void instr_deregister_intf(std::string name);


cMonInstr::cMonInstr(std::string name) 
  : name(name),
    bridge(new cBridge(k_NumHarts, k_VLen)) {}
void cMonInstr::initial() {
  instr_register_intf(name, this);
  parseArgs();
}

// Command line options
void cMonInstr::parseArgs() {
  s_vpi_vlog_info info;
  vpi_get_vlog_info(&info);

  for (int i = 0; i < info.argc; i++) {
    std::string arg(info.argv[i]);

    if (arg.find("+dutmon_tracer") == 0)
      monTracer = true;
  }
}

void cMonInstr::final() {
  instr_deregister_intf(name);
}

void cMonInstr::monGpr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  rvInstr.gpr.valid = true;
  rvInstr.gpr.rd_addr = addr;
  rvInstr.gpr.rd_wdata = data;
}

void cMonInstr::monFpr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  rvInstr.fpr.valid = true;
  rvInstr.fpr.frd_addr = addr;
  rvInstr.fpr.frd_wdata = data;
}

void cMonInstr::monVr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t *data, std::uint8_t mask) {
  for (int i = 0; i < 8; i++) {
    rvInstr.vr.valid[i] = (mask >> i) & 1;
    if (rvInstr.vr.valid[i]) {
      rvInstr.vr.vrd_addr[i] = addr+i;
      for (int j = 0; j < k_VLen/64; j++)
        rvInstr.vr.vrd_wdata[i][j] = data[(k_VLen/64)*i+j];
    }
  }
}

void cMonInstr::monCsr(int hart, std::uint64_t cycle, std::uint32_t addr, std::uint64_t data) {
  sCsr c;
  c.valid = true;
  c.csr_addr = addr;
  c.csr_wdata = data;
  rvInstr.csrs.push_back(c);
}

void cMonInstr::monRetire(int hart, std::uint64_t cycle, std::uint64_t tag, std::uint64_t pc, std::uint64_t opcode, std::uint32_t trap) {
  rvInstr.valid = true;
  rvInstr.cycle = cycle;
  rvInstr.tag = tag;
  rvInstr.pc.valid = true;
  rvInstr.pc.pc_rdata = pc;
  rvInstr.opcode = opcode;
  rvInstr.trap = trap;

  if (monTracer) {
    display(hart, rvInstr);
  }

  bridge->processDutInstrRetire(hart, rvInstr);

  rvInstr.clear();
}

void cMonInstr::display(int hart, const sRvInstr &instr) {
  if (instr.gpr.valid)
    std::cout << "<" << std::dec << instr.cycle << ">"
              << " DutGprWr: "
              << "[Hart=" << hart << ","
              << " Reg=X" << std::dec << instr.gpr.rd_addr << ","
              << " Data=0x" << std::hex << instr.gpr.rd_wdata
              << "]\n";
  if (instr.fpr.valid)
    std::cout << "<" << std::dec << instr.cycle << ">"
              << " DutFprWr: "
              << "[Hart=" << hart << ","
              << " Reg=F" << std::dec << instr.fpr.frd_addr << ","
              << " Data=0x" << std::hex << instr.fpr.frd_wdata
              << "]\n";
  for (int reg = 0; reg < 8; reg++) {
    if (instr.vr.valid[reg]) {
      std::stringstream regval;
      uint64_t val[k_VLen/64] = {0};
      int num_dwords = k_VLen/64;
      for (int i = 0; i < num_dwords; i++) {
        val[i] = instr.vr.vrd_wdata[reg][i];
      }
      regval << std::hex << std::setfill('0') << std::setw(16) << val[num_dwords-1];
      for (int i=num_dwords-2; i>0; i--) {
         regval  << "_" << std::hex << std::setfill('0') << std::setw(16) << val[i];  
      }
      regval << "_" << std::hex << std::setfill('0') << std::setw(16) << val[0];
      std::cout << "<" << std::dec << instr.cycle << ">"
                << " DutVrWr: "
                << "[Hart=" << hart << ","
                << " Reg=V" << std::dec << instr.vr.vrd_addr[reg] << ","
                << " Data=0x" << std::hex << regval.str()
                << "]\n";
    }
  }
  // Retire
  std::cout << "<" << std::dec << instr.cycle << ">"
            << " DutRetire: "
            << "[Hart=" << hart << ","
            << " InstrTag=0x" << std::hex << instr.tag
            << " (" << std::dec << (instr.tag>>16) << "," << (instr.tag&0xffff) <<  ")" << ","
            << " PC=0x" << std::hex << instr.pc.pc_rdata << ","
            << " Opcode=0x" << std::hex << instr.opcode
            << "]\n";
}
