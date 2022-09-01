#include "register.h"
#include <iostream>
// Register
Register::Register(threadT tid, stateIdT rid, sizenBitT bitSize, unitDataT * data):threadId(tid), registerId(rid), size(bitSize),valueV(data, data+size/64){};

stateIdT Register::getRegisterId(){
    return(registerId);
};

void Register::setValue(unitDataT * data){
    valueV.clear();
    //valueV.assign(data, data+size/64);
    for (int i = 0; i<size/64; i = i + 1){
        valueV.push_back(data[i]);
    }
};

bool Register::checkValue(unitDataT * data){
    std::vector<unitDataT> valueRef;
    for (int i = 0; i<size/64; i = i + 1){
        valueRef.push_back(data[i]);
    }
    return(valueRef == valueV);
};

sizenBitT Register::getSize(){
    return(size);
};

void Register::updateSize(sizenBitT sz){
  size = sz;
  valueV.resize(size/64, 0);
};

std::vector<size8BytesT> Register::getValue(){
    return(valueV);
};

//RegisterSnapshot
RegisterSnapshot::RegisterSnapshot(threadT tid):threadId(tid){
    for(const stateIdT &supportStateId : supportStates){
        sizenBitT regSize = supportStatesSize[supportStateId];
        size8BytesT rstValue[] = {0x0};
        Register reg(threadId, supportStateId, regSize, rstValue);
        snapshotCol.insert_or_assign(supportStateId, reg);
    }
};

std::vector<size8BytesT> RegisterSnapshot::getValue(stateIdT id){
    Register reg = snapshotCol.at(id);
    return(reg.getValue());
};

void RegisterSnapshot::updateSize(unsigned int vlen){
    for(const stateIdT &id : supportStates){
      if (id >= CAC_STATE_RegV0_ID && id <= CAC_STATE_RegV31_ID) {
        Register reg = snapshotCol.at(id);
        reg.updateSize(vlen);
        snapshotCol.at(id) = reg;
      }
    }
}

void RegisterSnapshot::updateValue(stateIdT id, size8BytesT * data){
    Register reg = snapshotCol.at(id);
    reg.setValue(data);
    snapshotCol.at(id) = reg;
};

bool RegisterSnapshot::checkValue(stateIdT id, size8BytesT * data){
    Register reg = snapshotCol.at(id);
    return(reg.checkValue(data));
};
