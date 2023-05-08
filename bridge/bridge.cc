// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#include "bridge.h"

#include <sstream>          // stringstream
#include <iostream>         // cout
#include <thread>           // std::this_thread::sleep_for
#include <chrono>           // std::chrono::seconds
#include <cstring>          // strlen
#include <cstdlib>          // system

// Constructor
cBridge::cBridge(int numHarts, int vLen)
  : numHarts_(numHarts),
    vLen_(vLen),
    cac_(CacCore(numHarts))    
{
  cac_.configureVlen(vLen_);
  parseArgs();
  connectWhisper();
}

// Destructor
cBridge::~cBridge() {
  disconnectWhisper();
}

// Command line options
void cBridge::parseArgs() {
  s_vpi_vlog_info info;
  vpi_get_vlog_info(&info);

  for (int i = 0; i < info.argc; i++) {
    std::string arg(info.argv[i]);

    std::cout << "Arg: " << arg << "\n";
    if (arg.find("+bridge_tracer") == 0)
      bridge_tracer = true;
    if (arg.find("+testfile=") == 0)
      testname_ = arg.substr(strlen("+testfile="));
    if (arg.find("+bootcode=") == 0)
      bootcode_ = arg.substr(strlen("+bootcode="));
    if (arg.find("+whisper_path=") == 0)
      whisperPath_ = arg.substr(strlen("+whisper_path="));
    if (arg.find("+whisper_json_path=") == 0)
      whisperJsonPath_ = arg.substr(strlen("+whisper_json_path="));
    if (arg.find("+arch_checks_exclude") == 0)
      archChecksExclude_ = arg.substr(strlen("+arch_checks_exclude="));
    if (arg.find("+arch_checks_instr_exclude") == 0)
      archChecksInstrExclude_ = arg.substr(strlen("+arch_checks_instr_exclude="));
  }
}

// Whisper command options
std::string cBridge::getWhisperCmd(std::string testname_) {
  std::string cmd = whisperPath_ + " " + testname_ + " " + bootcode_ +
                    " --harts " + std::to_string(numHarts_) + 
                    " --raw --configfile " + whisperJsonPath_ + 
                    " --logfile iss_cosim.log --traceload" +
                    " --commandlog isscmd.log" +
                    " --server whisper_connect &";

  return cmd;
}

// Whisper connect
void cBridge::connectWhisper() {
  std::string cmd = getWhisperCmd(testname_);
  std::cout << "Cosim whisper command: " << cmd << "\n";
  system(cmd.c_str());

  auto start = std::chrono::high_resolution_clock::now();
  while (true) {
    std::this_thread::sleep_for (std::chrono::milliseconds(30));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (whisperConnect("whisper_connect") >= 0) {
      std::cout << "Whisper connect succeeded in " << duration << " ms\n";
      break;
    }
    else if (duration > k_whisperTimeoutInMilliSeconds) {
      std::cout << "Whisper connect failed. Stopping after " << duration << " ms.\n";
      break;
    }
  }
}

// Whisper disconnect
void cBridge::disconnectWhisper() {
  whisperDisconnect();
}

// DUT Interface APIs
// Process instruction called on retire
void cBridge::processDutInstrRetire(int hart, sRvInstr &dutInstr) {
  // Push DUT state to CAC
  updateCac(hart, dutInstr);

  // Get whisper state and push to CAC
  sWhisperState whisInstr;
  stepWhisper(hart, dutInstr.cycle, dutInstr.tag, whisInstr);
  updateCac(hart, dutInstr.cycle, dutInstr.tag, whisInstr);

  // Check DUT vs whisper state
  // except when asked not to by user
  cac_.step(hart);
  if (disChecks(whisInstr)) {
    pokeWhisper(hart, dutInstr);
    cac_.resetStatus(hart);
  }
  if (!cac_.getStatus(hart)){
    std::cout << "\nError: Core Arch Checker Mismatch\n";
    vpi_control(vpiFinish);
  }
}

// Get whisper instruction state
void cBridge::stepWhisper(int hart, uint64_t cycle, uint64_t tag, sWhisperState &whisInstr) {

  bool success = whisperStep(hart, cycle, tag,  whisInstr.pc, whisInstr.opcode, whisInstr.changeCount, 
    whisInstr.buffer, whisInstr.bufferSize, whisInstr.privMode, whisInstr.fpFlags, whisInstr.trap);
  if (!success) {
    std::cout << "Error: whisperStep failed at hart=" << hart << " step=" << cac_.getStep(hart) << "\n";
  }
  if (bridge_tracer) {
    std::cout << "<" << std::dec << cycle << ">" 
              << " Whisper Step #" << cac_.getStep(hart) << ": " 
              << "[Hart=" << hart << ","
              << " InstrTag=0x" << std::hex << tag << ","
              << " ChangeCount=" << std::dec << whisInstr.changeCount << ","
              << " PC=0x" << std::hex << whisInstr.pc << ","
              << " Opcode=0x" << std::hex << whisInstr.opcode << ","
              << " " << whisInstr.buffer
              << "]\n";
  }
  
  // Hack to step whisper an extra time on wfi to synch with RTL jump to handler
  std::string disasm(whisInstr.buffer, whisInstr.bufferSize);
  if (disasm.find("wfi") != std::string::npos) {
    success = whisperStep(hart, cycle, tag,  whisInstr.pc, whisInstr.opcode, whisInstr.changeCount, 
      whisInstr.buffer, whisInstr.bufferSize, whisInstr.privMode, whisInstr.fpFlags, whisInstr.trap);
    if (bridge_tracer)
      std::cout << "<" << std::dec << cycle << ">" 
                << " Whisper Step #" << cac_.getStep(hart) << ": Extra step due to wfi" << "\n"; 
  }
}

// Push DUT state to CAC
void cBridge::updateCac(int hart, const sRvInstr &dutInstr) {
  // PC
  updatePc(hart, eSrc::k_Dut, dutInstr.pc.pc_rdata);
  // GPR
  if (dutInstr.gpr.valid) {
    size8BytesT dwordArray [1] = {dutInstr.gpr.rd_wdata};
    updateRegs(hart, eSrc::k_Dut, eResource::k_IntReg, dutInstr.gpr.rd_addr, dwordArray);
  }
  // FPR
  if (dutInstr.fpr.valid) {
    size8BytesT dwordArray [1] = {dutInstr.fpr.frd_wdata}; 
    updateRegs(hart, eSrc::k_Dut, eResource::k_FpReg, dutInstr.fpr.frd_addr, dwordArray);
  }
  // VR
  if (dutInstr.vr.valid) {
    size8BytesT dwordArray [128] = {0};
    for (int i = 0; i< vLen_/64; i++) {
      dwordArray[i] = dutInstr.vr.vrd_wdata[i];
    }
    updateRegs(hart, eSrc::k_Dut, eResource::k_VecReg, dutInstr.vr.vrd_addr, dwordArray);
  }
}

// Push whisper state to CAC
void cBridge::updateCac(int hart, uint64_t cycle, uint64_t tag, sWhisperState &whisInstr) {
  // PC
  updatePc(hart, eSrc::k_Whisper, whisInstr.pc);
  // Iterate over other changes - r, f, v,
  size8BytesT dwordArray [1] = {0};
  size8BytesT dwordVecArray [128] = {0};
  uint32_t entries = vLen_/64;
  for (auto i = 0u; i < whisInstr.changeCount; i++) {
    bool success = whisperChange(hart, whisInstr.resource, whisInstr.address, whisInstr.value, whisInstr.valid);
    if (!success) {
      std::cout << "Error: whisperChange failed at hart=" << hart << " step=" << cac_.getStep(hart) << "\n";
    }
    if (!whisInstr.valid) {
      std::cout << "Error: whisperChange returned invalid entry at hart=" << hart << " step=" << cac_.getStep(hart) << "\n";
    }
    if (bridge_tracer) {
      std::cout << "<" << std::dec << cycle << ">" 
                << " Whisper Step #" << cac_.getStep(hart) << ": " 
                << "[Hart=" << hart << ","
                << " InstrTag=0x" << std::hex << tag << ","
                << " Resource=" << (char)whisInstr.resource << ","
                << " Addr=0x" << std::hex << whisInstr.address << ","
                << " Data=0x" << std::hex << whisInstr.value 
                << "]\n";
    }

    switch(whisInstr.resource) {
      case 'r':
        dwordArray [0] = whisInstr.value;
        updateRegs(hart, eSrc::k_Whisper, eResource::k_IntReg, whisInstr.address, dwordArray);
        break;
      case 'f':
        dwordArray [0] = whisInstr.value;
        updateRegs(hart, eSrc::k_Whisper, eResource::k_FpReg, whisInstr.address, dwordArray); 
        break;
      case 'v':
        dwordVecArray [i % entries] = whisInstr.value;
        if ((i % entries) == (entries - 1))
          updateRegs(hart, eSrc::k_Whisper, eResource::k_VecReg, whisInstr.address, dwordVecArray); 
        break;
      default:
        break;
    }
  }
}

// Utility functions
void cBridge::updatePc(int hart, eSrc src, uint64_t data) { 
  size8BytesT dwordArray [1] = {data};
  if (src == eSrc::k_Dut) {
    cac_.updateRegister(hart, CAC_STATE_PC_ID, dwordArray);
  } else {
    cac_.updateRefRegister(hart, CAC_STATE_PC_ID, dwordArray);
  }
}

void cBridge::updateRegs(int hart, eSrc src, eResource resource, uint64_t addr, size8BytesT dwordArray[]) {
  if (src == eSrc::k_Dut) {
    if ((resource == eResource::k_IntReg) && (addr == 0x0))
      return;
    cac_.updateRegister(hart, resource, addr, dwordArray);
  } else {
    cac_.updateRefRegister(hart, resource, addr, dwordArray);
  }
}

// Skip checks on certain conditions if requested
bool cBridge::disChecks(const sWhisperState &whisInstr) {
  if (archChecksInstrExclude_ == "")
    return false;

  std::string disasm(whisInstr.buffer, whisInstr.bufferSize);
  std::stringstream ss(archChecksInstrExclude_);

  while(ss.good()) {
    std::string instr;
    std::getline(ss, instr, ',' );

    if (disasm.find(instr) != std::string::npos) {
      return true;
    }
  }
  return false;
}

// Poke resources in whisper
void cBridge::pokeWhisper(int hart, const sRvInstr &dutInstr) {
  bool valid = false;
  bool success = false;
  if (dutInstr.gpr.valid)
    success = whisperPoke(hart, 'r', dutInstr.gpr.rd_addr, dutInstr.gpr.rd_wdata, valid);
  if (dutInstr.fpr.valid)
    success = whisperPoke(hart, 'f', dutInstr.fpr.frd_addr, dutInstr.fpr.frd_wdata, valid);
  // TODO Vector poke
  if (!success) {
    std::cout << "Error: whisperPoke failed at hart=" << hart << " step=" << cac_.getStep(hart) << "\n";
  }
  if (!valid) {
    std::cout << "Error: whisperPoke returned invalid entry at hart=" << hart << " step=" << cac_.getStep(hart) << "\n";
  }
}
