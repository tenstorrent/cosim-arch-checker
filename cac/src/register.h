#ifndef REGISTER_H
#define REGISTER_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "caclib.h"

class Register
{
    public:
        Register(threadT tid, stateIdT rid, sizenBitT bitSize, unitDataT * data);
        void setValue(unitDataT * data);
        threadT getThreadId();
        sizenBitT getSize();
        void updateSize(sizenBitT sz);
        stateIdT getRegisterId();
        std::vector<unitDataT> getValue();
        bool checkValue(unitDataT * data);
    private:
        threadT threadId;
        stateIdT registerId;
        sizenBitT size;
        std::vector<unitDataT> valueV;
};

class RegisterSnapshot
{
    public:
        RegisterSnapshot(threadT tid);
        std::vector<unitDataT> getValue(stateIdT id);
        void updateSize(unsigned int vlen);
        void updateValue(stateIdT id, unitDataT * data);
        bool checkValue(stateIdT id, unitDataT * data);
    private:
        threadT threadId;
        std::unordered_map<stateIdT, Register> snapshotCol;
};

#endif
