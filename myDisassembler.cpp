// ECE2500 Project 1 
// MIPS Disassembler 
// Last modified: 10/25/2018 by Man Jun Koh

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <utility>

enum TYPE {R,I};

struct Instruction {

    TYPE type;
    int opcode = 0;
    int rs = 0;
    int rt = 0;
    int rd = 0;
    int shamt = 0;
    int funct = 0;
    signed long int immediate = 0;
};

std::string convertHexToBinary(std::string instructionHex);
Instruction parseInstruction(std::string instructionBinary, int &numInstruction);
std::string findRegister(int numRegister);
std::string findInstructionName(int opcodeVal, TYPE type);

    int main(int argc, char *argv[])
{
    
    std::ifstream in;
    in.open(argv[1]);

    if(!in.is_open())
    {
        std::cerr << "unable to open the input file" << std::endl;
        return EXIT_FAILURE;
    }

    // set the output name same as the input name but with .s extension 
    std::string outputFile = "";
    std::string inputFile = argv[1];

    // copy the name of inputfile
    for(int i = 0; i < inputFile.size(); i++)
    {
        if(inputFile[i] == '.')
            break;
        outputFile.append(1,inputFile[i]);
    }

    // add the proper output extension
    outputFile.append(".s");
    
    std::ofstream out(outputFile);
    std::string instructionHex;
    std::string instructionBinary; 
    std::string command; //command from the command line 

    int numInstruction = 0;
    bool isValidInstruction = false;
    std::vector<std::string> validInstruction;
    std::vector<Instruction> validInstructions;
    TYPE myType = I;

    std::string rs, rt, rd, instName, shamt, funct; 

    // start reading in the input file

    Instruction inst;  // initialize instruction struct 

    in >> instructionHex;       // or getline(in,instruction);
    while(!in.fail())
    {
        numInstruction++;
        isValidInstruction = true;
        if(instructionHex.size() != 8)
        {
            out << "error in " << inputFile << ", line " << numInstruction << 
                        ": machine code does not have 8 hexadecimal digits." << std::endl;
            isValidInstruction = false;
        }

       // std::cout << instructionHex << std::endl;
        instructionBinary = convertHexToBinary(instructionHex);

        if(isValidInstruction == true)
        {
            inst = parseInstruction(instructionBinary,numInstruction);

            if(inst.type == R)
            {
                rs = findRegister(inst.rs);
                rt = findRegister(inst.rt);
                rd = findRegister(inst.rd);
                shamt = inst.shamt;
                instName = findInstructionName(inst.funct,R);
                if(instName == "sll" || instName == "srl")
                    out << instName << " " << rd << ", " << rt << ", " << inst.shamt << std::endl;
                else
                    out << instName << " " << rd << ", " << rs << ", " << rt << std::endl;
            }
            else
            {
                rs = findRegister(inst.rs);
                rt = findRegister(inst.rt);
                instName = findInstructionName(inst.opcode, I);

                if (instName == "lw" || instName == "sw" || instName == "sb" || instName == "sc" || instName == "sh")
                {
                    out << instName << " " << rt << ", " << inst.immediate << 
                                "(" << rs << ") " << std::endl;
                }
                else if(instName == "lui")
                    out << instName << " " << rt << ", " << inst.immediate << std::endl;
                else
                {
                    out << instName << " " << rt << ", " << rs << ", " << inst.immediate << std::endl;
                }
            }
            
        }
        

        in >> instructionHex;
    }

    return 0;
}

std::string convertHexToBinary(std::string instructionHex)
{
    std::string instructionBinary = "";
    for(int i = 0; i < instructionHex.length(); i++)
    {
        switch(instructionHex[i])
        {
            case '0': instructionBinary.append("0000"); break;
            case '1': instructionBinary.append("0001"); break;
            case '2': instructionBinary.append("0010"); break;
            case '3': instructionBinary.append("0011"); break;
            case '4': instructionBinary.append("0100"); break;
            case '5': instructionBinary.append("0101"); break;
            case '6': instructionBinary.append("0110"); break;
            case '7': instructionBinary.append("0111"); break;
            case '8': instructionBinary.append("1000"); break;
            case '9': instructionBinary.append("1001"); break;
            case 'a': instructionBinary.append("1010"); break;
            case 'b': instructionBinary.append("1011"); break;
            case 'c': instructionBinary.append("1100"); break;
            case 'd': instructionBinary.append("1101"); break;
            case 'e': instructionBinary.append("1110"); break;
            case 'f': instructionBinary.append("1111"); break;
        }
    }
    return instructionBinary;
}

Instruction parseInstruction(std::string instructionBinary, int &numInstruction)
{
    Instruction inst;

    std::string temp = "";
    for (int i = 0; i < 6; i++)
    {
        temp.append(1, instructionBinary[i]);
        inst.opcode = std::stoi(temp, nullptr, 2);
    }

    // case 1 = R instruction
    std::string temprs = "", temprt = "", temprd = "", tempshamt = "", tempfunct = "", tempimmed = "";

    if (inst.opcode == 0)
    {
        inst.type = R;
        for (int i = 6; i < 11; i++)
        {
            temprs.append(1, instructionBinary[i]);
            temprt.append(1, instructionBinary[i + 5]);
            temprd.append(1, instructionBinary[i + 10]);
            tempshamt.append(1, instructionBinary[i + 15]);
            inst.rs = std::stoi(temprs, nullptr, 2);
            inst.rt = std::stoi(temprt, nullptr, 2);
            inst.rd = std::stoi(temprd, nullptr, 2);
            inst.shamt = std::stoi(tempshamt, nullptr, 2);
        }
        for (int i = 26; i < 32; i++)
        {
            tempfunct.append(1, instructionBinary[i]);
            inst.funct = std::stoi(tempfunct, nullptr, 2);
        }

    }
    else
    {
        inst.type = I;
        for (int i = 6; i < 11; i++)
        {
            temprs.append(1, instructionBinary[i]);
            temprt.append(1, instructionBinary[i + 5]);
            inst.rs = std::stoi(temprs, nullptr, 2);
            inst.rt = std::stoi(temprt, nullptr, 2);
        }
        for(int i = 16; i < 32; i++)
        {
            tempimmed.append(1,instructionBinary[i]);
            inst.immediate = std::stol(tempimmed,0,2);
        }
    }

    return inst;
}

std::string findRegister(int numRegister)
{
    std::map<int,std::string> regMap;

    regMap[0] = "$zero";
    regMap[1] = "$at";
    regMap[2] = "$v0";
    regMap[3] = "$v1";
    regMap[4] = "$a0";
    regMap[5] = "$a1";
    regMap[6] = "$a2";
    regMap[7] = "$a3";
    regMap[8] = "$t0";
    regMap[9] = "$t1";
    regMap[10] = "$t2";
    regMap[11] = "$t3";
    regMap[12] = "$t4";
    regMap[13] = "$t5";
    regMap[14] = "$t6";
    regMap[15] = "$t7";
    regMap[16] = "$s0";
    regMap[17] = "$s1";
    regMap[18] = "$s2";
    regMap[19] = "$s3";
    regMap[20] = "$s4";
    regMap[21] = "$s5";
    regMap[22] = "$s6";
    regMap[23] = "$s7";
    regMap[24] = "$t8";
    regMap[25] = "$t9";
    regMap[26] = "$k0";
    regMap[27] = "$k1";
    regMap[28] = "$gp";
    regMap[29] = "$sp";
    regMap[30] = "$fp";
    regMap[31] = "$ra";

    auto target = regMap.find(numRegister);
    std::string foundRegister = target->second;

    return foundRegister;
}

std::string findInstructionName(int opcodeVal, TYPE type)
{
    std::map<int,std::string> instMap;

    switch(type)
    {
        case R:

        instMap[32] = "add";
        instMap[33] = "addu";
        instMap[36] = "and";
        instMap[39] = "nor";
        instMap[37] = "or";
        instMap[42] = "slt";
        instMap[43] = "sltu";
        instMap[0] = "sll";
        instMap[2] = "srl";
        instMap[34] = "sub";
        instMap[35] = "subu";
        break;

    case I :
        instMap[8] = "addi";
        instMap[9] = "addiu";
        instMap[12] = "andi";
        instMap[4] = "beq";
        instMap[5] = "bne";
        instMap[36] = "lbu";
        instMap[37] = "lhu";
        instMap[48] = "ll";
        instMap[15] = "lui";
        instMap[35] = "lw";
        instMap[13] = "ori";
        instMap[10] = "slti";
        instMap[11] = "sltiu";
        instMap[40] = "sb";
        instMap[56] = "sc";
        instMap[41] = "sh";
        instMap[43] = "sw";
        break;
    }
    

    auto target = instMap.find(opcodeVal);
    std::string foundName;
    if(target == instMap.end())
        std::cerr << "error: instruction name not found and cannot disassemble" << std::endl;
    else
    {
        foundName = target->second;
         return foundName;
    }

    return foundName;
}