#ifndef INFO_H
#define INFO_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "caclib.h"

class Info
{
    public:
        Info(threadT tid, stateIdT stateId, const std::string &type, unitDataT * item, unsigned int size);
        std::string getItemName();
        unitDataT * getData();
        std::string getFormatInfo();
        std::string getType();
        unsigned int getSize();
    private:
        threadT threadId;
        stateIdT stateId;
        std::string infoType;
        std::string itemName;
        unitDataT * item;
        unsigned int size;
        std::string formatString;
};

class InfoCol
{
    public:
        InfoCol(threadT tid, int stepNum, const std::string &type);
        std::unordered_map<std::string, Info> getInfoDict();
        void gatherInfo(Info &infoItem);
        void outputStates(InfoCol *infoColIns);
    private:
        threadT threadId;
        std::string infoColType;
        std::unordered_map<std::string, Info> infoDict;
        int stepNum;
};

class Record
{
    public:
        Record(threadT tNum);
        void addInfoCol(threadT tid, bool ifdut, InfoCol &col);
        void addInfo(threadT tid, bool ifdut, Info & info);
        InfoCol getInfoColByStep(threadT tid, bool ifdut, int stepN);
    private:
        threadT threadNum;
        std::unordered_map<threadT, std::vector<InfoCol>> recorderDutCol;
        std::unordered_map<threadT, std::vector<InfoCol>> recorderSimCol;
};


#endif
