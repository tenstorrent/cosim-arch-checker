#include "cacCore.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// CacCore
CacCore::CacCore(threadT tNum):threadNum(tNum){
    record = new Record(threadNum);
    for(threadT tid = 0; tid<tNum; tid++){
        RegisterSnapshot regSnpSt(tid);
        registerSnapshot.insert_or_assign(tid, regSnpSt);
        std::vector<Register> ckBuff;
        checkingBuffer.insert_or_assign(tid, ckBuff);
        stepCount.insert_or_assign(tid, 0);
        dutChangeCount.insert_or_assign(tid, 0);
        simChangeCount.insert_or_assign(tid, 0);
        status.insert_or_assign(tid, true);

        InfoCol dutInfoColIns(tid, stepCount.at(tid), "DUT");
        record->addInfoCol(tid, true, dutInfoColIns);
        InfoCol simInfoColIns(tid, stepCount.at(tid), "SIM");
        record->addInfoCol(tid, false, simInfoColIns);
    }
};

std::string CacCore::getHello(){
    return("CacCore has been constructed!");
}

int CacCore::getStep(threadT threadId){
    return(stepCount.at(threadId));
};

// get if mismatch
bool CacCore::getStatus(threadT threadId){
    return(status.at(threadId));
};

void CacCore::resetStatus(threadT threadId){
    status.at(threadId) = true;
};

// Configuration API
void CacCore::configureVlen(unsigned int vlen) {
    cfg_vlen = vlen;
    for(threadT tid = 0; tid<threadNum; tid++){
      registerSnapshot.at(tid).updateSize(cfg_vlen);
    }
}

unsigned int CacCore::getRegisterSize(stateIdT id) {
    if (id >= CAC_STATE_RegV0_ID && id <= CAC_STATE_RegV31_ID) {
      return cfg_vlen;
    } else {
      return supportStatesSize[id];
    }
}

// Simulator API to update Register
void CacCore::updateRefRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, unitDataT * data){
    stateIdT id = generateStateId(typeEncoding, typeOffset);
    registerSnapshot.at(threadId).updateValue(id, data);
    Info infoIns(threadId, id, "SIM", data, getRegisterSize(id));
    record->addInfo(threadId, false, infoIns);
    simChangeCount.at(threadId) = simChangeCount.at(threadId) + 1;
};
void CacCore::updateRefRegister(threadT threadId, stateIdT id, unitDataT * data){
    registerSnapshot.at(threadId).updateValue(id, data);
    Info infoIns(threadId, id, "SIM", data, getRegisterSize(id));
    record->addInfo(threadId, false, infoIns);
    simChangeCount.at(threadId) = simChangeCount.at(threadId) + 1;
};


// Dut API to update Register
void CacCore::updateRegister(threadT threadId, unsigned int typeEncoding, unsigned int typeOffset, unitDataT * data){
    stateIdT id = generateStateId(typeEncoding, typeOffset);
    Info infoIns(threadId, id, "DUT", data, getRegisterSize(id));
    record->addInfo(threadId, true, infoIns);
    Register reg(threadId, id, getRegisterSize(id), data);
    checkingBuffer.at(threadId).push_back(reg);
    dutChangeCount.at(threadId) = dutChangeCount.at(threadId) + 1;
};
void CacCore::updateRegister(threadT threadId, stateIdT id, unitDataT * data){
    Info infoIns(threadId, id, "DUT", data, getRegisterSize(id));
    record->addInfo(threadId, true, infoIns);
    Register reg(threadId, id, getRegisterSize(id), data);
    checkingBuffer.at(threadId).push_back(reg);
    dutChangeCount.at(threadId) = dutChangeCount.at(threadId) + 1;
};



bool CacCore::checkRegister(threadT threadId, stateIdT id, unitDataT * data){
    return(registerSnapshot.at(threadId).checkValue(id, data));
};

// make a lock step
void CacCore::step(threadT threadId){
    // print changecount mismatch as warning
    // Updates with same previous values are allowed, so not flagging as error
    // Updates with different values will show up as errors downstream
    if (dutChangeCount.at(threadId) != simChangeCount.at(threadId)) {
      std::cout<<"\nWarning: ChangeCount Mismatch"
               <<" DUT: "<<dutChangeCount.at(threadId)
               <<" SIM: "<<simChangeCount.at(threadId)<<std::endl;
    }
    // use rtl changecount and check against iss snapshot
    std::vector<Register> buffer = checkingBuffer.at(threadId);
    for (std::vector<Register>::iterator it = buffer.begin(); it != buffer.end(); ++it) {
        std::vector<size8BytesT> reg = it->getValue();
        unitDataT *dat = reg.data();
        bool ckRst;
        ckRst = checkRegister(threadId, it->getRegisterId(), dat);
        status.at(threadId) = status.at(threadId) && ckRst;
    }
    //print out
    if (status.at(threadId) == false){
        std::cout<<"\nRegister Mismatch"<<std::endl;
    }
    std::cout<<"Step: "<<std::dec<<stepCount.at(threadId)<<std::endl;
    InfoCol dutInfoColDebug = record->getInfoColByStep(threadId, true, stepCount.at(threadId));
    InfoCol simInfoColDebug = record->getInfoColByStep(threadId, false, stepCount.at(threadId));
    dutInfoColDebug.outputStates(&simInfoColDebug);

    stepCount.at(threadId) = stepCount.at(threadId) + 1;
    checkingBuffer.at(threadId).clear();
    dutChangeCount.insert_or_assign(threadId, 0);
    simChangeCount.insert_or_assign(threadId, 0);

    InfoCol dutInfoColIns(threadId, stepCount.at(threadId), "DUT");
    record->addInfoCol(threadId, true, dutInfoColIns);
    InfoCol simInfoColIns(threadId, stepCount.at(threadId), "SIM");
    record->addInfoCol(threadId, false, simInfoColIns);
};

// Generate State Id by type encoding and offset
// 0:RT_FIX, 1:RT_FLT, 2:RT_X, 3: RT_PAS
// GPR, FPR, CSR, Vec, PC
stateIdT CacCore::generateStateId(unsigned int typeEncoding, unsigned int typeOffset){
    if (typeEncoding == REGISTER_RT_FIX_ENCODING){
        return(CAC_STATE_RegX0_ID + typeOffset);
    } else if (typeEncoding == REGISTER_RT_FLT_ENCODING) {
        return(CAC_STATE_RegF0_ID + typeOffset);
    } else if (typeEncoding == REGISTER_RT_VEC_ENCODING) {
        return(CAC_STATE_RegV0_ID + typeOffset);
    }else{
        std::cout<<"\nError: Unknown register type encoding"<<std::endl;
        exit(1);
    }

}
