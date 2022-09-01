#include "src/cacCore.h"
#include "gtest/gtest.h"

// Basic test template
TEST(TestHelloWorld, HeloWorld) {
    CacCore cbd(4);
    std::string act = cbd.getHello();
    std::string ans = "CacCore has been constructed!";
    EXPECT_EQ(act, ans)<<"Type the debug Message here: "<<ans<<std::endl;
};

// Test for misc
TEST(TESTMISC, TESTMISC_BASIC){
    // Test class for Info
    size8BytesT pc [] = {0xdeadbeef};
    size8BytesT x0 [] = {0xcafe0001};
    Info infoDut(0, CAC_STATE_PC_ID, "DUT", pc);
    EXPECT_EQ(infoDut.getFormatInfo(), "DUT:[Data:00000000deadbeef]")<<"Info format string is mismatched !"<<std::endl;
    Info infoSim(0, CAC_STATE_PC_ID, "SIM", pc);
    EXPECT_EQ(infoSim.getFormatInfo(), "SIM:[Data:00000000deadbeef]")<<"Info format string is mismatched !"<<std::endl;
    // Test class for InfoCol
    Info infoDutX0(0, CAC_STATE_RegX0_ID, "DUT", pc);
    Info infoSimX0(0, CAC_STATE_RegX0_ID, "SIM", pc);

    InfoCol dutInfoCol(0, 1, "DUT");
    dutInfoCol.gatherInfo(infoDut);
    dutInfoCol.gatherInfo(infoDutX0);
    InfoCol simInfoCol(0, 1, "SIM");
    simInfoCol.gatherInfo(infoSim);
    simInfoCol.gatherInfo(infoSimX0);
    std::unordered_map<std::string, Info> testDutinfocol = dutInfoCol.getInfoDict();
    EXPECT_EQ(infoDut.getFormatInfo(), testDutinfocol.at("PC").getFormatInfo())<<"InfoCol dict lookup mismatched !"<<std::endl;
    std::unordered_map<std::string, Info> testSiminfocol = simInfoCol.getInfoDict();
    EXPECT_EQ(infoSimX0.getFormatInfo(), testSiminfocol.at("X0").getFormatInfo())<<"InfoCol dict lookup mismatched !"<<std::endl;
};

// Test for class Register
TEST(TestRegister, BasicRegisterTest){
    size8BytesT foo [2] = {16, 32};
    std::vector<size8BytesT> expectedVector(std::begin(foo), std::end(foo));
    Register r(0, 0, 128, foo);
    sizenBitT act = r.getSize();
    sizenBitT ans = 128;
    std::vector<size8BytesT> ansVector = r.getValue();
    EXPECT_EQ(act, ans)<<"ERROR: (Register Test) Size is mismatch!";
    EXPECT_EQ(expectedVector, ansVector)<<"ERROR: (Register Test) Value is mismatch!";

    size8BytesT foo2 [2] = {0xdead, 0xbeef};
    std::vector<size8BytesT> expectedVector2(std::begin(foo2), std::end(foo2));
    r.setValue(foo2);
    std::vector<size8BytesT> ansVector2 = r.getValue();
    EXPECT_EQ(expectedVector2, ansVector2)<<"ERROR: (Register Test) setValue function is Wrong!";

    size8BytesT foo3 [2] = {0xdead, 0xbeef};
    bool ans3 = r.checkValue(foo3);
    EXPECT_TRUE(ans3) << "ERROR: (Register Test) Value Setting is not working!";
};

// Test for class RegisterSnapshot
TEST(TestRegisterSnapshot, BasicSnapshotTest){
    threadT threadId = 0;
    RegisterSnapshot rs(threadId);
    size8BytesT testPCValue0 [] = {0xdeadbeef};
    size8BytesT testPCValue1 [] = {0xdeadbeef};
    size8BytesT testPCValue2 [] = {0xbeefdead};
    size8BytesT testRegXXValue0 [] = {0xcafe0001};
    size8BytesT testRegXXValue1 [] = {0xcafe0001};
    size8BytesT testRegXXValue2 [] = {0xcafe0002};
    rs.updateValue(CAC_STATE_PC_ID, testPCValue0);
    rs.updateValue(CAC_STATE_RegX0_ID, testRegXXValue0);
    rs.updateValue(CAC_STATE_RegX1_ID, testRegXXValue0);
    rs.updateValue(CAC_STATE_RegX2_ID, testRegXXValue0);
    rs.updateValue(CAC_STATE_RegX3_ID, testRegXXValue0);
    rs.updateValue(CAC_STATE_RegX4_ID, testRegXXValue0);
    rs.updateValue(CAC_STATE_RegX5_ID, testRegXXValue0);

    EXPECT_TRUE(rs.checkValue(CAC_STATE_PC_ID, testPCValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX0_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX1_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX2_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX3_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX4_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_TRUE(rs.checkValue(CAC_STATE_RegX5_ID, testRegXXValue1))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";

    EXPECT_FALSE(rs.checkValue(CAC_STATE_PC_ID, testPCValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX0_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX1_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX2_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX3_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX4_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
    EXPECT_FALSE(rs.checkValue(CAC_STATE_RegX5_ID, testRegXXValue2))<< "ERROR: (RegisterSnapshot Test) Unexpected checking value!";
};

// Test for Core
TEST(TestCore, BasicCoreTest){
    threadT coreNum = 2;
    //instantiate CBoard by core num
    CacCore cbd(coreNum);
    threadT tid0 = 0;
    threadT tid1 = 1;
    bool tmpresult = false;
    int stepInspt = 0;
    size8BytesT dutPCValue0 [] = {0xcafe0000};
    size8BytesT refPCValue0 [] = {0xcafe0000};
    size8BytesT dutPCValue1 [] = {0xcafe0008};
    size8BytesT refPCValue1 [] = {0xcafe0008};    
    size8BytesT dutRegXXValue0 [] = {0xbeefbeef};
    size8BytesT refRegXXValue0 [] = {0xbeefbeef};
    size8BytesT dutRegXXValue1 [] = {0xdeadbeef};
    size8BytesT refRegXXValue1 [] = {0xdeadbeef};
    //single hart
    //Hart 0 Step 1
    //From Simulator:
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, refPCValue0);
    // Direct using state ID
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, refRegXXValue0);
    // Using encoding and offset
    cbd.updateRefRegister(tid0, REGISTER_RT_FIX_ENCODING, 1, refRegXXValue1);
    //From Dut:
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, dutPCValue0);
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, dutRegXXValue0);
    cbd.updateRegister(tid0,  REGISTER_RT_FIX_ENCODING, 1, dutRegXXValue1);
    cbd.step(tid0);
    stepInspt = cbd.getStep(tid0);
    EXPECT_EQ(stepInspt, 1)<<"ERROR: (CAC Test) Wrong Step Counts!";
    tmpresult = cbd.getStatus(tid0);
    //size8BytesT dutPCValue0 [] = {0xcafe0008};
    //size8BytesT dutPCValue1 [] = {0xcafe0010};
    EXPECT_TRUE(tmpresult);
    //Hart 0 Step 2
    //From Simulator:
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, refPCValue1);
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, refRegXXValue1);
    cbd.updateRefRegister(tid0, REGISTER_RT_FLT_ENCODING, 6, refRegXXValue1);
    //From Dut:
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, dutPCValue1);
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, dutRegXXValue1);
    cbd.updateRegister(tid0, REGISTER_RT_FLT_ENCODING, 6,dutRegXXValue1);
    cbd.step(tid0);
    tmpresult = cbd.getStatus(tid0);
    EXPECT_TRUE(tmpresult);
    stepInspt = cbd.getStep(tid0);
    EXPECT_EQ(stepInspt, 2)<<"ERROR: (CBoard Test) Wrong Step Counts!";

    //Multi hart
    //From Simulator:
    //Hart0
    cbd.updateRefRegister(tid0, CAC_STATE_PC_ID, refPCValue0);
    cbd.updateRefRegister(tid0, CAC_STATE_RegX0_ID, refRegXXValue0);
    //Hart1
    cbd.updateRefRegister(tid1, CAC_STATE_PC_ID, refPCValue1);
    cbd.updateRefRegister(tid1, CAC_STATE_RegX0_ID, refRegXXValue1);
    //From Dut:
    //Hart0
    cbd.updateRegister(tid0, CAC_STATE_PC_ID, dutPCValue0);
    cbd.updateRegister(tid0, CAC_STATE_RegX0_ID, dutRegXXValue0);
    //Hart1
    cbd.updateRegister(tid1, CAC_STATE_PC_ID, dutPCValue1);
    cbd.updateRegister(tid1, CAC_STATE_RegX0_ID, dutRegXXValue1);
    cbd.step(tid0);
    tmpresult = cbd.getStatus(tid0);
    EXPECT_TRUE(tmpresult);
    cbd.step(tid1);
    tmpresult = cbd.getStatus(tid1);
    EXPECT_TRUE(tmpresult);
};