#include "info.h"
#include <iostream>
#include <sstream>
#include <iomanip>

//Info
Info::Info(threadT tid, stateIdT stateId, const std::string &type, unitDataT * item, unsigned int size):threadId(tid),stateId(stateId),infoType(type),item(item),size(size){
    itemName = supportStatesSymbol[stateId];
    std::stringstream tmpStream;
    if (size == VEC_128) {
      tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[1]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]"<<" ("<<std::dec<<size<<")";
    } else if (size == VEC_256) {
      tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[3]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[2]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[1]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]"<<" ("<<std::dec<<size<<")";
    } else if (size == VEC_512) {
      tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[7]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[6]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[5]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[4]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[3]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[2]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[1]<<
                             "_"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]"<<" ("<<std::dec<<size<<")";
    } else {
      tmpStream<<type<<":[Data:"<<std::setfill('0')<<std::setw(sizeof(unitDataT)*2)<<std::hex<<item[0]<<"]";
    }
    formatString = tmpStream.str();
};

std::string Info::getItemName(){
    return(itemName);
};

std::string Info::getType(){
    return(infoType);
}

unitDataT * Info::getData(){
    return(item);
};

std::string Info::getFormatInfo(){
    return(formatString);
};

unsigned int Info::getSize(){
    return(size);
}

//InfoCol
InfoCol::InfoCol(threadT tid, int stepNum, const std::string &type):threadId(tid),infoColType(type),stepNum(stepNum){};

std::unordered_map<std::string, Info> InfoCol::getInfoDict(){
    return(infoDict);
};

void InfoCol::outputStates(InfoCol *infoColIns){
    std::unordered_map<std::string, Info> infoDictIns = infoColIns->getInfoDict();
    for(auto & infoIt : infoDict){
        int width = 48;
        if (infoIt.first.substr(0,1) == "V") {
          if (infoIt.second.getSize() == VEC_128) {
            width = 71;
          } else if (infoIt.second.getSize() == VEC_256) {
            width = 105;
          } else if (infoIt.second.getSize() == VEC_512) {
            width = 173;
          }
        }
        std::cout<<std::setw(20)<<infoIt.first<<std::setw(width)<<infoDict.at(infoIt.first).getFormatInfo()<<std::endl;
        if (infoDictIns.find(infoIt.first) != infoDictIns.end())
          std::cout<<std::setw(20)<<""<<std::setw(width)<<infoDictIns.at(infoIt.first).getFormatInfo()<<std::endl;
    }
};

void InfoCol::gatherInfo(Info & infoItem){
    std::string infoName = infoItem.getItemName();
    if (infoDict.find(infoName) != infoDict.end()){
        infoDict.at(infoName) = infoItem;
    }else{
        infoDict.insert_or_assign(infoName, infoItem);
    }
};

//Recorder
Record::Record(threadT tNum):threadNum(tNum){
    for(threadT tid = 0; tid<tNum; tid++){
        std::vector<InfoCol> infoDutList;
        recorderDutCol.insert_or_assign(tid, infoDutList);
        std::vector<InfoCol> infoSimList;
        recorderSimCol.insert_or_assign(tid, infoSimList);
    }
};

void Record::addInfo(threadT tid, bool ifdut, Info &info){
    if (ifdut == true){
        recorderDutCol.at(tid).back().gatherInfo(info);
    }else{
        recorderSimCol.at(tid).back().gatherInfo(info);
    }
};

void Record::addInfoCol(threadT tid, bool ifdut, InfoCol &col){
    if (ifdut == true){
        recorderDutCol.at(tid).push_back(col);
    }else{
        recorderSimCol.at(tid).push_back(col);
    }
};

InfoCol Record::getInfoColByStep(threadT tid, bool ifdut, int stepN){
    if (ifdut == true){
        return(recorderDutCol.at(tid).at(stepN));
    }else{
        return(recorderSimCol.at(tid).at(stepN));
    }
};
