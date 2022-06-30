#ifndef MODULES_H
#define MODULES_H

#include <iostream>
#include <cstdio>
#include <climits>
#include <fstream>
using namespace std;

#define int8 int8_t
#define uint8 uint8_t
#define int16 int16_t
#define uint16 uint16_t
#define NUM_REGS 16
#define flag bool

#define IWIDTH 16
#define DWIDTH 8
#define NUMSETS 64
#define BLOCK_SIZE 4

//global variables for the processor
extern int total_instructions;
extern int arithmetic_instructions;
extern int logical_instructions;
extern int data_instructions;
extern int control_instructions;
extern int halt_instructions;
extern float cpi;
extern int total_stalls;
extern int data_stalls;
extern int control_stalls;

class Register
{
public:
    int val;
    int read();
    void write(int _val); //_val is well inside 8 s
};

class Register16
{
public:
    int val;
    int read();
    void write(int _val); //_val is well inside 8 s
};

class PC
{
public:
    int val;
    int read();
    void write(int _val);
    void increment();
};

class RegisterFile
{
public:
    flag request_failed;
    flag isWriting[16];
    Register R[NUM_REGS];
    flag fread1;
    flag fread2;
    flag fwrite;
    flag busy;
    int read(int rPos);
    void write(int rPos, int _val);
    void reset();
    void getBusy();
    void relax();
    RegisterFile();
};

class IBlock
{
public:
    int offset[BLOCK_SIZE];
};

class DBlock
{
public:
    int offset[BLOCK_SIZE];
};

class ICache
{
public:
    IBlock data[NUMSETS];
    int request(int);
};

class DCache
{
public:
    DBlock data[NUMSETS];
    int request(int);
    void write(int, int);
};

class IFIDBuffer
{
public:
    //current instruction pc value
    int currPcVal;

    //instruction fetched
    int instruction;

    //whether the contents of his buffer matter
    flag busy;

    //setter; redundant
    void set(int, int);

    //getter; redudant
    int getcurrPc();

    //getter; redundant
    int getInstruction();

    //constructor
    IFIDBuffer();
};

class IFModule
{
public: //in deveopement phase, let's keep everything public. We can introduce data hidinglater on.
    PC &pc;
    IFModule(PC &_pc, ICache &_inputCache) : pc(_pc), inputCache(_inputCache)
    {
        stall = false;
        ready = true;
    }
    IFIDBuffer execute(/* args */);
    ICache &inputCache;
    PC currPc;

    //a go signal to this module
    flag go;
    flag ready; //redundant as of yet
    flag stall;
};

class IDEXBuffer
{
public:
    //whether contents matter
    flag busy;

    //operation is arithmetic
    flag arithmetic;

    //operation in losgical
    flag logical;

    //operation is load
    flag load;

    //operation is store
    flag store;

    //operation is jump
    flag jump;

    //operation is bneq
    flag bneq;

    //issue halt signal to processor
    flag HALT_SIGNAL;

    //2bit subop for ALU
    int subop;

    //currPc, denotes the address of the current instrcution
    int currPc;

    //in case of arithmetic and logic instructions, this is first operand
    //in case of load store, this register is base
    int src1;
    int srcval1;
    flag validsrc1; //will be used in operator forwarding

    //in case of arithmetic and logic instructions, this is second operand
    //in case of load store, src2 and srcval2 are not valid
    int src2;
    int srcval2;
    flag validsrc2; //will be used in operator forwarding

    //will denote write_to register in arithmetic and logic operations and load instruction
    int dest;
    //in case of store instruction or bneq instruction, dest is the concerned register and destval is its value;
    int destval;
    int validdest; //will be used in operator forwarding

    //in load store, offset stores the immediate 4  value of address offset
    int offset;

    //stores jump offset in jump instructions
    int jump_addr;

    IDEXBuffer();
};

class IDregFileModule
{
public:
    IDregFileModule(RegisterFile &_regFile, DCache &_dataCache) : regFile(_regFile), dataCache(_dataCache)
    {
        stall = false;
        ready = true;
    }
    RegisterFile &regFile;
    DCache &dataCache;
    IFIDBuffer ifidBuf;
    IDEXBuffer execute(/* args */);
    flag resolveBranch(int);
    
    //status flag
    flag ready;
    flag stall;
};

/*
class EXModule
{
public:
    IDEXBuffer idexbuf;
    EMBuffer execute(IDEXBuffer);
    
};*/

class EMBuffer
{
public:
    flag busy;
    flag HALT_SIGNAL;

    int aluOutput;
    int currPc;

    flag load;
    flag store;
    flag writeToRegister;

    //will denote write_to register in arithmetic and logic operations and load instruction
    int dest;
    //in case of store instruction or bneq instruction, dest is the concerned register and destval is its value;
    int destval;
    int validdest; //will be used in operator forwarding

    EMBuffer();
};

class ALU
{
    public:
    int adder(int , int, flag);
    int mul(int, int);
    int AND(int, int);
    int OR(int, int);
    int NOT(int);
    int XOR(int, int);
};

class EXModule
{
public:
    ALU &alu;
    PC &pc;
    flag &FLUSH;
    EXModule(ALU &_alu, PC &_pc, flag &f) : alu(_alu), pc(_pc), FLUSH(f)
    {
        inc.write(1);
        ready = true;
        stall = false;
    }
    IDEXBuffer idexBuf;
    Register inc;
    EMBuffer execute();
    
    //status flag(s)
    flag ready;
    flag stall;
};

class MWBuffer
{
public:
    flag load;
    flag aluInstr;
    //issue halt signal to processor
    flag HALT_SIGNAL;

    int dest;
    int destval;
    flag validdest;

    int val;
    int lmd;

    int currPc; 

    flag busy;
    MWBuffer();
};

class MEMModule
{
public:
    MEMModule(DCache &_dataCache) : dataCache(_dataCache)
    {
        stall = false;
        ready = true;
    }

    DCache &dataCache;
    EMBuffer emBuf;
    Register LMD;
    MWBuffer execute(/* args */);

    //status flag(s)
    flag ready;
    flag stall;
};

class WBSTATUS {
    public:
    flag invalid;
    flag ready;
    WBSTATUS() {
        invalid = true;
        ready = true;
    }
};

class WBModule
{
public:
    MWBuffer mwBuf;
    RegisterFile &regFile;
    DCache &dataCache;
    WBModule(RegisterFile &_regFile, DCache &_dataCache) : regFile(_regFile), dataCache(_dataCache)
    {
        stall = false;
        ready = true;
    }

    //ICache &inputCache;
    WBSTATUS execute(/* args */);

    //status flag(s)
    flag ready;
    flag stall;
};

class ControlUnit
{
};

class Processor
{
public:
    //ControlUnit CU;
    ICache inputCache;
    DCache dataCache;
    RegisterFile regFile;
    PC pc;
    Register16 IR;

    IFModule IF;
    IFIDBuffer IFID;
    IDregFileModule IDregFile;
    IDEXBuffer IDEX;
    EXModule EX;
    EMBuffer EM;
    MEMModule MEM;
    MWBuffer MW;
    WBModule WB;

    WBSTATUS wbstatus;

    ALU alu;
    flag HALT_SIGNAL;
    flag COMPLETE;
    flag FLUSH;
    int clock_cycle;

    flag stall[5];
    //more

    //more data

    //methods
    Processor() : IF(pc, inputCache), IDregFile(regFile, dataCache), EX(alu, pc, FLUSH), MEM(dataCache), WB(regFile, dataCache) {}
    void setup(ifstream &, ifstream &, ifstream &);
    void startup();
    void cycle();
    void output();

    //test individual elements
    void testicache();
};

#endif