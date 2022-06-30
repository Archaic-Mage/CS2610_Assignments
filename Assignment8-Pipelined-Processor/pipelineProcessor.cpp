/** Authors - Soham Tripathy - Teja Vardhan **/

#include <bits/stdc++.h>
#include "modules.h"

using namespace std;

/*** ALU ***/

int ALU::adder(int a, int b, bool as) {
    int r;
    if(as) {
        r = a - b;
    } else {
        r = a + b;
    }
    return r & 0xff;
}
int ALU::mul(int a, int b) {
    int r = a * b;
    return r & 0xff;
}
int ALU::AND(int a, int b) {
    return a & b;
}
int ALU::OR(int a, int b) {
    return a | b;
}
int ALU::NOT(int a) {
    return ~a;
}
int ALU::XOR(int a, int b) {
    return a ^ b;
}

/*** DCACHE ***/

int DCache::request(int addr) {
    int set = addr >> 2;
    int offset = addr & 3;
    int low = data[set].offset[offset];
    return low;
}

void DCache::write(int addr, int item){
    int set = addr >> 2;
    int offset = addr & 3;
    data[set].offset[offset] = item;
}


/*** ICACHE ***/

int ICache::request(int npc) {
    int set = npc >> 2;
    int offset = npc & 3;
    int upper_byte = data[set].offset[offset];
    if(offset == 3) {
        offset = -1;
        set++;
        if(set >= 64) {
            set = 0;
        }
    }
    offset++;
    int lower_byte = data[set].offset[offset];
    int ret = upper_byte << 8;
    ret += lower_byte;
    return ret;
}

/*** PC ***/

int PC::read()
{
    return val;
}

void PC::write(int _val)
{
    val = _val;
}

void PC::increment() {
    val += 2;
}

/*** REGISTER ***/

int Register::read()
{
    return val;
}

void Register::write(int _val)
{
    val = _val;
}

int RegisterFile::read(int rPos)
{
    if (fread1 == true && fread2 == true)
    {
        request_failed = true;
    }
    else if (fread1 == false)
    {
        fread1 = true;
        return R[rPos].read();
    }
    else if (fread2 == false)
    {
        fread2 = true;
        return R[rPos].read();
    }
    return 0;
}

void RegisterFile::write(int rPos, int _val)
{
    if (fwrite == true)
    {
        request_failed = true;
    }
    else
    {
        fwrite = true;
        R[rPos].write(_val);
    }
}

RegisterFile::RegisterFile()
{
    for (int i = 0; i < 16; i++)
    {
        isWriting[i] = false;
    }
    request_failed = false;
    fread1 = fread2 = fwrite = false;
    busy = false;
}

void RegisterFile::reset()
{
    request_failed = false;
    fread1 = fread2 = fwrite = false;
}

void RegisterFile::getBusy()
{
    busy = true;
}

void RegisterFile::relax()
{
    busy = false;
}

/*** IF ID BUFFER ***/

void IFIDBuffer::set(int currPc, int instruction)
{
    currPcVal = currPc;
    this->instruction = instruction;
}

int IFIDBuffer::getcurrPc()
{
    return currPcVal;
}

int IFIDBuffer::getInstruction()
{
    return instruction;
}

IFIDBuffer::IFIDBuffer()
{
    busy = true;

    currPcVal = -1;
    instruction = 0xf000;
}

/*** IF ***/

IFIDBuffer IFModule::execute() {
    IFIDBuffer buf;
    if(!go) {
        buf.busy = true;
        ready = true;
        return buf;
    }
    int v = pc.read();
    int instruction = inputCache.request(v);
    pc.increment();
    buf.set(pc.val, instruction);
    buf.busy = false;
    ready = true;
    return buf;
}



/*** ID EX BUFFER ***/

IDEXBuffer::IDEXBuffer()
{

    busy = true;

    arithmetic = false;

    logical = false;

    load = false;

    store = false;

    jump = false;

    bneq = false;

    HALT_SIGNAL = false;

    subop = 0;

    //currPc, denotes the address of the current instrcution
    currPc = -1;

    //in case of arithmetic and logic instructions, this is first operand
    //in case of load store, this register is base
    src1;
    srcval1;
    validsrc1 = false; 

    //in case of arithmetic and logic instructions, this is second operand
    //in case of load store, src2 and srcval2 are not valid
    src2;
    srcval2;
    validsrc2 = false;

    dest;
    destval;
    validdest = false; 

    offset = 0;

    jump_addr = 2;
}

/*** ID ***/

int signExtend(int x, int s)
{
    return ((s << 7) | (s << 6) | (s << 5) | (s << 4) | x);
}

IDEXBuffer IDregFileModule::execute()
{
    IDEXBuffer buf;
    if (ifidBuf.busy)
    {
        buf.busy = true;
        //buf.ready = true;
        ready = true;
        return buf;
    }
    int instruction = ifidBuf.getInstruction();
    buf.currPc = ifidBuf.getcurrPc();
    int opcode = instruction >> 12;
    int mode = opcode >> 2;
    int subop = opcode & 3;
    if (mode == 0)
    {
        buf.arithmetic = true;
        buf.logical = false;
        buf.load = false;
        buf.jump = false;
        buf.bneq = false;
        buf.store = false;
        buf.subop = subop;
        regFile.getBusy();
        regFile.reset();
        int destA = (instruction >> 8) & 0xf;
        int src1A = (instruction >> 4) & 0xf;
        int src2A = instruction & 0xf;
        if (subop == 3)
        {
            if (regFile.isWriting[destA])
            {
                buf.busy = true;
                ready = false;
                return buf;
            }
            buf.validsrc1 = false;

            buf.validsrc2 = false;

            buf.dest = destA;
            buf.destval = regFile.read(destA);
            buf.validdest = false; 
        }
        else
        {
            if (regFile.isWriting[src2A] || regFile.isWriting[src1A])
            {
                buf.busy = true;
                ready = false;
                return buf;
            }
            buf.src1 = src1A;
            buf.srcval1 = regFile.read(src1A);
            buf.validsrc1 = true;

            buf.src2 = src2A;
            buf.srcval2 = regFile.read(src2A);
            buf.validsrc2 = true;

            buf.dest = destA;
            buf.validdest = false;
        }

        regFile.isWriting[destA] = true;
    }
    else if (mode == 1)
    {
        buf.logical = true;
        buf.arithmetic = false;
        buf.load = false;
        buf.jump = false;
        buf.bneq = false;
        buf.store = false;
        buf.subop = subop;
        regFile.getBusy();
        regFile.reset();
        int destA = (instruction >> 8) & 0xf;
        int src1A = (instruction >> 4) & 0xf;
        int src2A = instruction & 0xf;
        if (subop == 2)
        {
            if (regFile.isWriting[destA] || regFile.isWriting[src1A])
            {
                total_stalls++;
                data_stalls++;
                buf.busy = true;
                ready = false;
                return buf;
            }
            buf.src1 = src1A;
            buf.srcval1 = regFile.read(src1A);
            buf.validsrc1 = true;

            buf.validsrc2 = false;

            buf.dest = destA;
            buf.validdest = false;
        }
        else
        {
            if (regFile.isWriting[src2A] || regFile.isWriting[src1A])
            {
                total_stalls++;
                data_stalls++;
                buf.busy = true;
                ready = false;
                return buf;
            }
            buf.src1 = src1A;
            buf.srcval1 = regFile.read(src1A);
            buf.validsrc1 = true;

            buf.src2 = src2A;
            buf.srcval2 = regFile.read(src2A);
            buf.validsrc2 = true;

            buf.dest = destA;
            buf.validdest = false;
        }
        regFile.isWriting[destA] = true;
    }
    else if (mode == 2)
    {
        if (subop == 0)
        {
            buf.logical = false;
            buf.arithmetic = false;
            buf.load = true;
            buf.jump = false;
            buf.bneq = false;
            buf.store = false;
            regFile.getBusy();
            regFile.reset();
            int destA = (instruction >> 8) & 0xf;
            int src1A = (instruction >> 4) & 0xf;
            int src2A = instruction & 0xf;
            if (regFile.isWriting[src1A])
            {
                total_stalls++;
                data_stalls++;
                buf.busy = true;
                ready = false;
                return buf;
            }
            //base register
            buf.src1 = src1A;
            buf.srcval1 = regFile.read(src1A);
            buf.validsrc1 = true;

            //src2 is busy
            buf.validsrc2 = false;

            //load will write in dest
            buf.dest = destA;
            buf.validdest = false;
            regFile.isWriting[destA] = true;

            //offset will be given in src2A
            int s = src2A >> 3;
            buf.offset = signExtend(src2A, s);
        }
        else if (subop == 1)
        {
            buf.store = true;
            buf.logical = false;
            buf.arithmetic = false;
            buf.load = false;
            buf.jump = false;
            buf.bneq = false;
            regFile.getBusy();
            regFile.reset();
            int destA = (instruction >> 8) & 0xf;
            int src1A = (instruction >> 4) & 0xf;
            int src2A = instruction & 0xf;
            if (regFile.isWriting[src1A] || regFile.isWriting[destA])
            {
                total_stalls++;
                data_stalls++;
                buf.busy = true;
                ready = false;
                return buf;
            }
            //base register
            buf.src1 = src1A;
            buf.srcval1 = regFile.read(src1A);
            buf.validsrc1 = true;

            //src2 is busy
            buf.validsrc2 = false;

            //store will read from dest
            buf.dest = destA;
            buf.destval = regFile.read(destA);
            buf.validdest = true;

            //offset will be given in src2A
            int s = src2A >> 3;
            buf.offset = signExtend(src2A, s);
        }
        else if (subop == 2)
        {
            buf.jump_addr = (instruction >> 4) & 0xff;
            buf.store = false;
            buf.logical = false;
            buf.arithmetic = false;
            buf.load = false;
            buf.jump = true;
            buf.bneq = false;
            //branch issues;

        }
        else
        {
            buf.jump_addr = instruction & 0xff;
            buf.store = false;
            buf.logical = false;
            buf.arithmetic = false;
            buf.load = false;
            buf.jump = false;
            buf.bneq = true;
            int destA = (instruction >> 8) & 0xf;
            regFile.getBusy();
            regFile.reset();
            if(regFile.isWriting[destA])
            {
                total_stalls++;
                data_stalls++;
                buf.busy = true;
                ready = false;
                return buf;
            }
            int dest = regFile.read(destA);
            buf.dest = destA;
            buf.destval = dest;
            buf.validdest = true;

            
        }
    }
    else if (mode == 3)
    {
        buf.HALT_SIGNAL = true;
        buf.busy = false;
        ready = false;
        ifidBuf.busy = true;
        return buf;
    }
    buf.busy = false;
    ready = true;
    return buf;
}

bool IDregFileModule::resolveBranch(int reg)
{
    return (reg == regFile.read(0));
}



/*** EM BUFFER ***/

EMBuffer::EMBuffer()
{
    busy = true;

    HALT_SIGNAL = false;

    aluOutput = 0;
    currPc = -1;
    load = false;
    store = false;
    writeToRegister = false;

    //will denote write_to register in arithmetic and logic operations and load instruction
    dest;
    //in case of store instruction or bneq instruction, dest is the concerned register and destval is its value;
    destval;
    validdest = false; //will be used in operator forwarding
}

/*** EX ***/

EMBuffer EXModule::execute()
{
    EMBuffer buf;
    buf.busy = true;
    if(idexBuf.busy) {
        ready = true;
        buf.busy = true;
        return buf;
    }

    buf.currPc = idexBuf.currPc;
    total_instructions++;
    int subop = idexBuf.subop;
    if(idexBuf.arithmetic)   //mode=0
    {
        arithmetic_instructions++;
        int val1, val2, val;
        if( (subop >> 1) & (subop & 1) ) //subop=3
        {
            //incrementer
            val1 = idexBuf.destval;
            val2 = inc.read();
            val = alu.adder(val1, val2, 0);
        }
        else if( (subop>>1) & (!(subop&1)) ) //subop=2
        {
            //multiplier
            val1 = idexBuf.srcval1;
            val2 = idexBuf.srcval2;
            //TODO: sign extend and etc in case result is not coming right
            val = alu.mul(val1, val2);
        }
        else if( (!(subop>>1)) & (subop&1)) //subop=1
        {
            //subtract
            val1 = idexBuf.srcval1;
            val2 = idexBuf.srcval2;
            //TODO: sign extend and etc in case result is not coming right
            val = alu.adder(val1, val2, 1);
        }
        else //subop=0
        {
            //add
            val1 = idexBuf.srcval1;
            val2 = idexBuf.srcval2;
            //TODO: sign extend and etc in case result is not coming right
            val = alu.adder(val1, val2, 0);
        }
        buf.writeToRegister = true;
        buf.dest = idexBuf.dest;
        buf.aluOutput = val;
        buf.destval = val;
        buf.validdest = true;
    }
    else if(idexBuf.logical)   //mode=1
    {
        logical_instructions++;
        int val1, val2, val;
        if( (subop >> 1) & (subop & 1) ) //subop=3
        {
            //xor
            int val1 = idexBuf.srcval1;
            int val2 = idexBuf.srcval2;
            int val = alu.XOR(val1, val2);
        }
        else if( (subop>>1) & (!(subop&1)) ) //subop=2
        {
            //not
            int val1 = idexBuf.srcval1;
            int val = alu.NOT(val1);
        }
        else if( (!(subop>>1)) & (subop&1)) //subop=1
        {
            //or
            int val1 = idexBuf.srcval1;
            int val2 = idexBuf.srcval2;
            int val = alu.OR(val1, val2);
        }
        else //subop=0
        {
            //and
            int val1 = idexBuf.srcval1;
            int val2 = idexBuf.srcval2;
            int val = alu.AND(val1, val2);
        }
        buf.writeToRegister = true;
        buf.dest = idexBuf.dest;
        buf.aluOutput = val;
        buf.destval = val;
        buf.validdest = true;
    }
    else if(idexBuf.load)
        {
            data_instructions++;
            // buf.writeToRegister = false;
            buf.load = true;
            //calc effective address
            int val1 = idexBuf.srcval1;
            int val2 = idexBuf.offset;
            int val = alu.adder(val1, val2, 0);
            buf.aluOutput = val;
            buf.dest = idexBuf.dest;
            buf.validdest = idexBuf.validdest;
        }
    else if(idexBuf.store)
        {
            data_instructions++;
            // buf.writeToRegister = false;
            buf.store = true;
            //calc effective address
            int val1 = idexBuf.srcval1;
            int val2 = idexBuf.offset;
            int val = alu.adder(val1, val2, 0);
            buf.aluOutput = val;
            buf.dest = idexBuf.dest;
            buf.destval = idexBuf.destval;
            buf.validdest = idexBuf.validdest;
        }
    else if(idexBuf.jump)
        {
            control_instructions++;
            // buf.writeToRegister = false;
            //calc effective address
            int val1 = idexBuf.currPc;
            int val2 = idexBuf.jump_addr;
                //cout << "val1: " << val1 <<" val2:" << val2 ;
                val2 = val2 << 1;
                //cout << "new : " << val2 << endl;
            int val = alu.adder(val1, val2, 0);
            buf.aluOutput = val;

            //set pc new value
            pc.write(val);

            //generate go flush signal
            FLUSH = true;
            control_stalls += 2;
            total_stalls += 2;
            buf.busy = true;
            ready = true;
            return buf;
        }
    else if(idexBuf.bneq)
        {
            control_instructions++;
            // buf.writeToRegister = false;
            //compare with 0
            int val1 = idexBuf.destval;
            int val2 = 0;
            if(val1 == val2) {
                //calc effective address
                int val1 = idexBuf.currPc;
                int val2 = idexBuf.jump_addr;
                //cout << "val1: " << val1 <<" val2:" << val2 ;
                val2 = (val2 << 1) & 0xff;
                //cout << "new : " << val2 << endl;
                int val = alu.adder(val1, val2, 0);
                buf.aluOutput = val;
                
                //set pc new value
                pc.write(val);
            //generate go flush signal
            FLUSH = true;
            control_stalls += 2;
            total_stalls += 2;
            buf.busy = true;
            ready = true;
            return buf;

            }
            else
            {
                //keep same pc value;
                //pc.write(idexBuf.currPc);
                buf.busy = true;
                ready = true;
                return buf;
            }

            // //generate go flush signal
            // FLUSH = true;
            // control_stalls += 2;
            // total_stalls += 2;
            // buf.busy = true;
            // ready = true;
            // return buf;
        }
    else if (idexBuf.HALT_SIGNAL)
        {
            halt_instructions++;
            buf.HALT_SIGNAL = true;
            buf.busy = false;
            ready = false;
            idexBuf.busy = true;
            return buf;
        }
    buf.busy = false;
    ready = true;
    return buf;
}

/***MEM ***/

MWBuffer MEMModule::execute()
{
    MWBuffer buf;
    
    if (emBuf.busy)
    {
        buf.busy = true;
        ready = true;
        return buf;
    }

    //cout << "MEM: " << emBuf.currPc << endl;
    buf.currPc = emBuf.currPc;
    if (emBuf.load)
    {
        buf.load = true;
        buf.aluInstr = false;
        int d = dataCache.request(emBuf.aluOutput);
        //if no error
        LMD.write(d);
        buf.lmd = d;
        buf.dest = emBuf.dest;
        buf.destval = d;
        buf.validdest = true;
    }
    else if (emBuf.store)
    {
        buf.load = false;
        buf.aluInstr = false;
        //here, emBuf.aluOut is m address and emBuf.dest is register contents
        dataCache.write(emBuf.aluOutput, emBuf.destval);
    }
    else if(emBuf.writeToRegister)
    {
        buf.aluInstr = true;
        buf.load = false;
        buf.dest = emBuf.dest;
        buf.destval = emBuf.aluOutput;
        buf.validdest = true;
        buf.val = emBuf.aluOutput;
    }
    else if(emBuf.HALT_SIGNAL)
    {
        buf.HALT_SIGNAL = true;
        buf.busy = false;
        ready = false;
        emBuf.busy = true;
        return buf;
    } 
    else 
    {
        cerr << "Can't reach here, logical error.\n";
    }
    buf.busy = false;
    ready = true;
    return buf;
}

/*** MEMORY WRITE BUFFER ***/

MWBuffer::MWBuffer()
{
    busy = true;
    HALT_SIGNAL = false;

    load = false;
    aluInstr = false;

    dest;
    destval;
    validdest = false;

    val;

    lmd;

    currPc = -1;
}

/*** WB ***/

WBSTATUS WBModule::execute()
{
    WBSTATUS status;
    if (mwBuf.busy)
    {
        status.invalid = true;
        status.ready = true;
        ready = true;
        return status;
    }
    //cout << "WB:" << mwBuf.currPc << endl;
    if (mwBuf.aluInstr)
    {
        regFile.write(mwBuf.dest, mwBuf.destval);
        /*
        if(regFile.request_failed)
        {
            status.busy = true;
            status.ready = false;
            ready = false;
            regFile.reset();
            return status;
        }
        */
        regFile.isWriting[mwBuf.dest] = false;
        //cout << "Written " << mwBuf.destval << " on reg " << mwBuf.dest << endl;
    }
    else if (mwBuf.load)
    {
        regFile.write(mwBuf.dest, mwBuf.lmd);
        regFile.isWriting[mwBuf.dest] = false;
    }
    else if (mwBuf.HALT_SIGNAL)
    {
        status.invalid = false;
        status.ready = false;
        ready = false;
        mwBuf.busy = true;
        return status;
    }
    status.invalid = false;
    status.ready = true;
    ready = true;
    return status;
}

/*** PROCESSOR ***/

int total_instructions;
int arithmetic_instructions;
int logical_instructions;
int data_instructions;
int control_instructions;
int halt_instructions;
float cpi;
int total_stalls;
int data_stalls;
int control_stalls;

void Processor::setup(ifstream &finI, ifstream &finD, ifstream &finR)
{
    //read icache values
    for (int i = 0; i < NUMSETS; i++)
    {
        finI >> hex >> inputCache.data[i].offset[0];
        finI >> hex >> inputCache.data[i].offset[1];
        finI >> hex >> inputCache.data[i].offset[2];
        finI >> hex >> inputCache.data[i].offset[3];
    }

    //read dcache values
    for (int i = 0; i < NUMSETS; i++)
    {
        finD >> hex >> dataCache.data[i].offset[0];
        finD >> hex >> dataCache.data[i].offset[1];
        finD >> hex >> dataCache.data[i].offset[2];
        finD >> hex >> dataCache.data[i].offset[3];
    }

    //read register file values
    int v;
    for (int i = 0; i < 16; i++)
    {
        finR >> hex >> v;
        //regFile.write(i,v);
        regFile.R[i].val = v;
    }

    //setup global counts
    total_instructions = 0;
    arithmetic_instructions = 0;
    logical_instructions = 0;
    data_instructions = 0;
    control_instructions = 0;
    halt_instructions = 0;
    total_stalls = 0;
    data_stalls = 0;
    control_stalls = 0;

}

void Processor::cycle()
{
    // processor clock cycle
    streambuf *orig_buf = cout.rdbuf();

    // set null
    clock_cycle++;

    int flag1 = 0;

    if (IF.go)
    {
        IFID = IF.execute();
    }
    if (!IDregFile.stall)
    {
        IDEX = IDregFile.execute();
    }
    if (!EX.stall)
    {
        EM = EX.execute();
    }
    if (!MEM.stall)
    {
        MW = MEM.execute();
    }
    if (!WB.stall)
    {
        wbstatus = WB.execute();
    }
    regFile.reset();

    if(FLUSH) {
        //render last 2 computations useless
        IF.go = true;
        IDregFile.ready = true;
        IDregFile.stall = false;
        IFID.busy = true;
        IDEX.busy = true;
        FLUSH = false;
    }
    //forward
    if (WB.ready)
    {
        WB.mwBuf = MW;
        IF.go = true;
        IF.stall = false;
        IDregFile.stall = false;
        EX.stall = false;
        MEM.stall = false;
        if (MEM.ready)
        {
            MEM.emBuf = EM;
            IF.go = true;
            IF.stall = false;
            IDregFile.stall = false;
            EX.stall = false;
            if (EX.ready)
            {
                EX.idexBuf = IDEX;
                IF.go = true;
                IF.stall = false;
                IDregFile.stall = false;
                if (IDregFile.ready)
                {
                    IDregFile.ifidBuf = IFID;
                    IF.go = true;
                    IF.stall = false;
                    if (IF.ready) 
                    {
                        IF.go = true;
                    }
                    else
                    {
                        //cout << "Blocking-4" << endl;
                        IF.go = false;
                    }
                }
                else
                {
                    //cout << "Blocking-3" << endl;
                    IF.go = false;
                    IF.stall = true;
                    //IDregFile.ifidBuf.ready = false;
                }
            }
            else
            {
                //cout << "Blocking-2" << endl;
                IF.go = false;
                IF.stall = true;
                IDregFile.stall = true;
                //EX.idexBuf.ready = false;
            }
        }
        else
        {
            //cout << "Blocking-1" << endl;
            IF.go = false;
            IF.stall = true;
            IDregFile.stall = true;
            EX.stall = true;
            // MEM.emBuf.ready = false;
            // IDregFile.ifidBuf.ready = false;
        }
    }
    else
    {
        //cout << "Blocking" << endl;
        IF.go = false;
        IF.stall = true;
        IDregFile.stall = true;
        EX.stall = true;
        MEM.stall = true;
        // IDregFile.ifidBuf.ready = false;
        // WB.mwBuf.ready = false;
        COMPLETE = true; //?
    }
    
}

void Processor::output() {
    ofstream fout;

    //dataCache
    fout.open("./output/DCache.txt");
    for(int i = 0; i < NUMSETS; i++) {
        fout << hex << dataCache.data[i].offset[0] << endl;
        fout << hex << dataCache.data[i].offset[1] << endl;
        fout << hex << dataCache.data[i].offset[2] << endl;
        fout << hex << dataCache.data[i].offset[3] << endl;
    }
    fout.close();

    //processing data
    cpi = (float) clock_cycle/total_instructions ;
    fout.open("./output/Output.txt");
    fout << "Total number of instructions executed: " << std::dec << total_instructions << endl;
    fout << "Number of instrcutions in each class" << endl;
    fout << "Arithmetic instructions              : " << std::dec << arithmetic_instructions << endl;
    fout << "Logical instructions                 : " << std::dec << logical_instructions << endl;
    fout << "Data instructions                    : " << std::dec << data_instructions << endl;
    fout << "Control instructions                 : " << std::dec << control_instructions << endl;
    fout << "Halt instructions                    : " << std::dec << halt_instructions << endl;
    fout << "Cycles Per Instrcution               : " << std::dec << cpi << endl;
    fout << "Total number of stalls               : " << std::dec << total_stalls << endl;
    fout << "Data stalls (raw)                    : " << std::dec << data_stalls << endl;
    fout << "Control stalls                       : " << std::dec << control_stalls << endl;
    fout.close();
}


int main() {


    Processor processor;
    ifstream finI, finD, finR;
    finI.open("./sample_tc/input/ICache.txt");
    finD.open("./sample_tc/input/DCache.txt");
    finR.open("./sample_tc/input/regFile.txt");

    //read icache values
    for (int i = 0; i < NUMSETS; i++)
    {
        finI >> hex >> processor.inputCache.data[i].offset[0];
        finI >> hex >> processor.inputCache.data[i].offset[1];
        finI >> hex >> processor.inputCache.data[i].offset[2];
        finI >> hex >> processor.inputCache.data[i].offset[3];
    }

    //read dcache values
    for (int i = 0; i < NUMSETS; i++)
    {
        finD >> hex >> processor.dataCache.data[i].offset[0];
        finD >> hex >> processor.dataCache.data[i].offset[1];
        finD >> hex >> processor.dataCache.data[i].offset[2];
        finD >> hex >> processor.dataCache.data[i].offset[3];
    }

    //read register file values
    int v;
    for (int i = 0; i < 16; i++)
    {
        finR >> hex >> v;
        //regFile.write(i,v);
        processor.regFile.R[i].val = v;
    }

    //setup global counts
    total_instructions = 0;
    arithmetic_instructions = 0;
    logical_instructions = 0;
    data_instructions = 0;
    control_instructions = 0;
    halt_instructions = 0;
    total_stalls = 0;
    data_stalls = 0;
    control_stalls = 0;
    
    for (int i = 0; i < 5; i++)
    {
        processor.stall[i] = false;
    }
    processor.pc.val = 0;
    processor.HALT_SIGNAL = false;
    processor.COMPLETE = false;
    processor.FLUSH = false;
    processor.clock_cycle = 0;
    processor.IF.go = true;
    processor.wbstatus.invalid = true;
    processor.wbstatus.ready = true;
    while (! processor.COMPLETE)
    {
        processor.cycle();
    }
    //cout << "Number of clock cycles taken = " << clock_cycle << endl;

    processor.output();

    return 0;
}
