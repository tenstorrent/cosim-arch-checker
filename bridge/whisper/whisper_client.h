// Licensed under the Apache License, Version 2.0, see LICENSE.TT for details

#pragma once

#include "svdpi.h"

extern "C"
int
whisperConnect(const char* filePath);

extern "C"
void
whisperDisconnect();

extern "C"
bool
whisperStep(int hart, uint64_t time, uint64_t instrTag, uint64_t& pc, 
	    uint32_t& instruction, unsigned& changeCount, char* buffer, unsigned bufferSize,
	    uint32_t& privMode, uint32_t& fpFlags, bool& hasTrap);

extern "C"
bool
whisperSimpleStep(int hart, uint64_t& pc, uint32_t& instruction, unsigned& changeCount);

extern "C"
bool
whisperChange(int hart, uint32_t& resource, uint64_t& addr, uint64_t& value,
  bool& valid);

extern "C"
bool
whisperMcmRead(int hart, uint64_t time, uint64_t instrTag,
  uint64_t addr, unsigned size, uint64_t value, bool internal, bool& valid);

extern "C"
bool
whisperMcmInsert(int hart, uint64_t time, uint64_t instrTag,
  uint64_t addr, unsigned size, uint64_t value, bool& valid);

extern "C"
bool
whisperMcmWrite(int hart, uint64_t time, uint64_t addr, unsigned size,
  svOpenArrayHandle handle, bool& valid);

extern "C"
bool
whisperPoke(int hart, char resource, uint64_t addr, uint64_t value,
  bool& valid);

extern "C"
bool
whisperPeek(int hart, char resource, uint64_t addr, uint64_t& value,
  bool& valid);

extern "C"
bool
whisperReset(int hart, bool& valid);
