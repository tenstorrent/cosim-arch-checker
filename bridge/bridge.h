// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include <string>
#include <vpi_user.h>       // vpi_printf

#include "bridge_base.h"
#include "whisper_client.h"
#include "cacCore.h"        // CoreArchChecker

class cBridge : public cBridgeBase {

public:
  cBridge(int numHarts, int vLen);
  ~cBridge();

  // DUT Interface API
  // Process instruction called on retire
  //   - Metadata
  //   - PC
  //   - Register Read/Write
  //   - CSRs
  //   - Memory Access
  //   - AMO
  //   - Table Walks 
  //   - Exceptions/Interrupts
  virtual void processDutInstrRetire(int hart, sRvInstr &dutInstr) override;

private:
  // Encapsulate whisper state
  typedef struct sWhisperState {
    //used in whisperStep
    uint64_t tag;
    uint64_t time;
    uint64_t pc;
    uint32_t opcode;
    unsigned changeCount;
    char buffer[128];
    unsigned bufferSize = 128;
    //Used in whisperChange
    uint32_t resource;
    uint64_t address;
    uint64_t value;
    bool valid;
    uint32_t privMode;
    uint32_t fpFlags;
    bool trap;
    
  } sWhisperState;

  typedef enum {
    k_IntReg = 0,
    k_FpReg = 1,
    k_VecReg = 4
  } eResource;

  typedef enum {
    k_Dut,
    k_Whisper
  } eSrc;

private:
  
  void parseArgs();
  void connectWhisper();
  void disconnectWhisper();
  std::string getWhisperCmd(std::string testname);
 
  void stepWhisper(int hart, uint64_t cycle, uint64_t tag, sWhisperState &whisInstr);
  void updateCac(int hart, const sRvInstr &dutInstr);
  void updateCac(int hart, uint64_t cycle, uint64_t tag, sWhisperState &whisInstr);
  void updatePc(int hart, eSrc src, uint64_t data);
  void updateRegs(int hart, eSrc src, eResource resource, uint64_t addr, size8BytesT dwordArray[]);
  bool disChecks(const sWhisperState &whisInstr);
  void pokeWhisper(int hart, const sRvInstr &dutInstr);

private:
  int numHarts_ = 0;
  int vLen_ = 0;
  CacCore cac_;

  bool bridge_tracer = true;
  std::string testname_ = "";
  std::string bootcode_ = "";
  std::string whisperPath_ = "";
  std::string whisperJsonPath_ = "";
  std::string archChecksExclude_ = "";
  std::string archChecksInstrExclude_ = "";

  const int k_whisperTimeoutInMilliSeconds = 1000;

};
