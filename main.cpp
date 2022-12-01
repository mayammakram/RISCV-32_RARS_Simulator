#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <bitset>
#include <vector>
using namespace std;

unordered_map<long long, string> memory;
//to store registers initially all zeros then read program file to get initializations
unordered_map<string, long long> registers;

unordered_map<int, string> instructions;//program counter with instruction
//unordered_map<int, string> realInstructions;
unordered_map<string, int> labels; // label with address else 1004

int pc, maxPC;

void printingMemAndReg();
void readFromAssembly();
void readFromregister();
void readFromProgram();
void loadWord();
void loadHalfWord();
void loadByte();
void loadByteUnsigned();
void loadHalfWordUnsigned();
void storeWord();
void storeHalfWord();
void storeByte();
void loadUpperImmidate();
void getPartsforLoading(string& source, string& destination, int& offset);
void getPartsforStoring(string& source, string& destination, int& offset);

void addUpperImmediatetoPC();
void jal();
void jalr();
void branchEqual();
void branchNotEqual();
void branchLessThan();
void branchLessThanUnsigned();
void branchGreaterThan();
void branchGreaterThanUnsigned();

void selectInstruction();

//Farah
void getPartsforRegtoReg(string& source1, string& source2, string& destination);
void getPartsforRegtoReg_Imm(string& source1, string& immediate, string& destination);
long long bitExtracted(long long num, int k);

void add();                  //done
void addImmediate();         //done
void sub();                  //done
void andinstruction();       //done
void andImmediate();         //done
void orinstruction();        //done
void orImmediate();          //done
void xorInstruction();       //done
void xorImmediate();         //done

void shiftLeftLogical();           //done
void shiftLeftLogicalImmediate();  //done
void shiftRightLogical();          //done
void shiftRightLogicalImmediate(); //done
void shiftRightAri();              //done
void shiftRightAriImmediate();     //done

void setLessThan();                //done
void setLessThanImm();             //done
void setLessThanUnsigned();
void setLessThanUnsignedImm();



int main()
{
    int startingAddress = 0;
    cout << "Please enter starting address: \n";
    cin >> startingAddress;
    pc = startingAddress;
    readFromregister();
    readFromAssembly();
    return 0;
}

void printingMemAndReg()
{
    cout << "*************************************" << endl;
    cout << "PC: " << pc << endl;
    cout << "--------" << endl;
    cout << "Memory: " << endl;
    for (auto x : memory)
        cout << x.first << " " << x.second << endl;
    cout << "--------" << endl;
    cout << "Registers: " << endl;
    for (auto x : registers) {
        registers["zero"] = 0;
        cout << x.first << " " << x.second << endl;
    }
    cout << "--------" << endl;
}

void readFromAssembly()
{
    ifstream assemblyFile;
    string line, word;
    istringstream iss;
    assemblyFile.open("TestCases&Tests/AssemblyProgram.txt");

    if (assemblyFile.fail())
    {
        cout << "Error.\n";
        exit(1);
    }

    int tempPC = pc;
    while (!assemblyFile.eof())
    {

        //here assembly file is read line by line
        getline(assemblyFile, line);

        if (line.find(":") != string::npos) //if there is a label
        {
            iss.str(line);
            iss >> word;
            word.erase(word.find(":"), 1);
            labels.insert({ word, tempPC });
            iss.clear();

            //remove word from line
            line.erase(line.begin(), line.begin() + line.find(" ") + 1);
        }
        instructions.insert({ tempPC, line });
        tempPC += 4;

        // //here each line is read word by word
        // iss.clear();
        // iss.str(line);
        // iss >> word;
    }

    maxPC = tempPC;
    //    realInstructions = instructions;
    for (auto it : instructions) {
        selectInstruction();
        pc += 4;
    }
}

void readFromregister()
{
    //unordered map
    string name, value;
    ifstream registerFile;
    string line, word;
    istringstream iss;
    registerFile.open("TestCases&Tests/RegisterFile.txt");

    if (registerFile.fail())
    {
        cout << "Error.\n";
        exit(1);
    }
    while (!registerFile.eof())
    {
        getline(registerFile, line);
        iss.clear();
        iss.str(line);

        iss >> name >> value;
        registers.insert({ name, stoll(value) });
    }

    registers["zero"] = 0;

    readFromProgram();
}
void readFromProgram()
{
    string name, value;
    ifstream programFile;
    string line, word;
    istringstream iss;
    programFile.open("TestCases&Tests/ProgramData.txt");

    if (programFile.fail())
    {
        cout << "Error.\n";
        exit(1);
    }
    while (!programFile.eof())
    {
        getline(programFile, line);
        iss.clear();
        iss.str(line);

        iss >> name >> value;
        if (registers.find(name) != registers.end())
        {
            registers.at(name) = stoll(value);
        }
    }
}

//gets source and destination for add, sub, and, or, xor, sll, srl, sra, slt instructions
void getPartsforRegtoReg(string& source1, string& source2, string& destination)
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    destination = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    source1 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    source2 = tempInstruction;
}

//gets source, imediate, and destination for addi, andi, ori, xori, slli, srli, srai instructions
void getPartsforRegtoReg_Imm(string& source1, string& immediate, string& destination)
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    destination = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    source1 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    immediate = tempInstruction;
}
//gets source, destination, and offset for lw, lh, lb, lbu, lhu instructions
void getPartsforLoading(string& source, string& destination, int& offset)
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    int pos3 = tempInstruction.find("(");

    destination = tempInstruction.substr(pos1 + 1, pos2 - pos1 - 1);

    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space


    pos3 = tempInstruction.find("(");
    int pos4 = tempInstruction.find(")");

    offset = stoi(tempInstruction.substr(0, pos4 - pos3));
    source = tempInstruction.substr(pos3 + 1, pos4 - pos3 - 1);
}
//gets source, destination, and offset for sw, sh, sb instructions
void getPartsforStoring(string& source, string& destination, int& offset)
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    int pos3 = tempInstruction.find("(");

    source = tempInstruction.substr(pos1 + 1, pos2 - pos1 - 1);

    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space


    pos3 = tempInstruction.find("(");
    int pos4 = tempInstruction.find(")");
    offset = stoi(tempInstruction.substr(0, pos4 - pos3));
    destination = tempInstruction.substr(pos3 + 1, pos4 - pos3 - 1);

}

void loadWord()
{
    string source;
    string destination;
    int offset;

    getPartsforLoading(source, destination, offset);

    long long sourceValue = registers.at(source);

    string destinationValue1 = memory.at(sourceValue + offset);//string of first 8 bits
    string destinationValue2 = memory.at(sourceValue + offset + 1);
    string destinationValue3 = memory.at(sourceValue + offset + 2);
    string destinationValue4 = memory.at(sourceValue + offset + 3);

    string destinationValue = destinationValue4 + destinationValue3 + destinationValue2 + destinationValue1;
    long long decimalDestinationValue;

    //need to determine +ve or -ve
    if (destinationValue.substr(0, 1) == "1")
    {
        // change all 0 to 1 and 1 to 0
        for (int i = 0; i < destinationValue.length(); i++)
        {
            if (destinationValue.at(i) == '1')
                destinationValue.at(i) = '0';
            else
                destinationValue.at(i) = '1';
        }
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
        decimalDestinationValue++;
        decimalDestinationValue = -decimalDestinationValue;
    }
    else
    {
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
    }

    //store into register
    registers.at(destination) = decimalDestinationValue;
}


void loadHalfWord()
{
    string source;
    string destination;
    int offset;
    getPartsforLoading(source, destination, offset);

    long long sourceValue = registers.at(source);

    string destinationValue1 = memory.at(sourceValue + offset);//string of first 8 bits
    string destinationValue2 = memory.at(sourceValue + offset + 1);

    string destinationValue = destinationValue2 + destinationValue1;
    long long decimalDestinationValue;

    //need to determine +ve or negative
    if (destinationValue.substr(0, 1) == "1")
    {
        // change all 0 to 1 and 1 to 0
        for (int i = 0; i < destinationValue.length(); i++)
        {
            if (destinationValue.at(i) == '1')
                destinationValue.at(i) = '0';
            else
                destinationValue.at(i) = '1';
        }
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
        decimalDestinationValue++;
        decimalDestinationValue = -decimalDestinationValue;
    }
    else
    {
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
    }
    //store into register
    registers.at(destination) = decimalDestinationValue;
}
void loadByte()
{
    string source;
    string destination;
    int offset;
    getPartsforLoading(source, destination, offset);

    long long sourceValue = registers.at(source);

    string destinationValue = memory.at(sourceValue + offset);
    long long decimalDestinationValue;

    //need to determine +ve or negative
    if (destinationValue.substr(0, 1) == "1")//negative number
    {
        // change all 0 to 1 and 1 to 0
        for (int i = 0; i < destinationValue.length(); i++)
        {
            if (destinationValue.at(i) == '1')
                destinationValue.at(i) = '0';
            else
                destinationValue.at(i) = '1';
        }
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
        decimalDestinationValue++;
        decimalDestinationValue = -decimalDestinationValue;
    }
    else
    {
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
    }
    //store into register
    registers.at(destination) = decimalDestinationValue;
}
void loadByteUnsigned()
{
    string source;
    string destination;
    int offset;
    getPartsforLoading(source, destination, offset);

    long long sourceValue = registers.at(source);

    string destinationValue = memory.at(sourceValue + offset);
    long long decimalDestinationValue;

    //need to determine +ve or negative
    if (destinationValue.substr(0, 1) == "1")//negative number
    {
        // change all 0 to 1 and 1 to 0
        for (int i = 0; i < destinationValue.length(); i++)
        {
            if (destinationValue.at(i) == '1')
                destinationValue.at(i) = '0';
            else
                destinationValue.at(i) = '1';
        }
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
        decimalDestinationValue++;
    }
    else
    {
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
    }
    //store into register
    registers.at(destination) = decimalDestinationValue;
}
void loadHalfWordUnsigned()
{
    string source;
    string destination;
    int offset;
    getPartsforLoading(source, destination, offset);

    long long sourceValue = registers.at(source);

    string destinationValue1 = memory.at(sourceValue + offset);//string of first 8 bits
    string destinationValue2 = memory.at(sourceValue + offset + 1);

    string destinationValue = destinationValue2 + destinationValue1;
    long long decimalDestinationValue;

    //need to determine +ve or negative
    if (destinationValue.substr(0, 1) == "1")//negative number
    {
        // change all 0 to 1 and 1 to 0
        for (int i = 0; i < destinationValue.length(); i++)
        {
            if (destinationValue.at(i) == '1')
                destinationValue.at(i) = '0';
            else
                destinationValue.at(i) = '1';
        }
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
        decimalDestinationValue++;
    }
    else
    {
        decimalDestinationValue = stoll(destinationValue, nullptr, 2);
    }

    //store into register
    registers.at(destination) = decimalDestinationValue;
}
void storeWord()
{
    string source;
    string destination;
    int offset;
    getPartsforStoring(source, destination, offset);

    string sourceValue = to_string(registers.at(source));//string holding value in register which is int
    bitset<32> sValue = stoll(sourceValue);//change the int to binary
    sourceValue = sValue.to_string();//make the binary into string so that concatenation occurs
    long long destinationValue = registers.at(destination);//need it integer
    //store into memory
    if (memory.find(destinationValue + offset) == memory.end())
        memory.insert({ destinationValue + offset, sourceValue.substr(24,8) });
    //least significant
    else
        memory.at(destinationValue + offset) = sourceValue.substr(24, 8);

    if (memory.find(destinationValue + offset + 1) == memory.end())
        memory.insert({ destinationValue + offset + 1, sourceValue.substr(16,8) });//least significant
    else
        memory.at(destinationValue + offset + 1) = sourceValue.substr(16, 8);
    if (memory.find(destinationValue + offset + 2) == memory.end())
        memory.insert({ destinationValue + offset + 2, sourceValue.substr(8,8) });//least significant
    else
        memory.at(destinationValue + offset + 2) = sourceValue.substr(8, 8);
    if (memory.find(destinationValue + offset + 3) == memory.end())
        memory.insert({ destinationValue + offset + 3, sourceValue.substr(0,8) });//least significant
    else
        memory.at(destinationValue + offset + 3) = sourceValue.substr(0, 8);
}
void storeHalfWord()
{
    string source;
    string destination;
    int offset;
    getPartsforStoring(source, destination, offset);

    string sourceValue = to_string(registers.at(source));//string holding value in register which is int
    bitset<16> sValue = stoll(sourceValue);//change the int to binary
    sourceValue = sValue.to_string();//make the binary into string so that concatenation occurs

    long long destinationValue = registers.at(destination);//need it integer

    //store into memory
    if (memory.find(destinationValue + offset) == memory.end())
        memory.insert({ destinationValue + offset, sourceValue.substr(8,8) });//least significant
    else
        memory.at(destinationValue + offset) = sourceValue.substr(8, 8);

    if (memory.find(destinationValue + offset + 1) == memory.end())
        memory.insert({ destinationValue + offset + 1, sourceValue.substr(0,8) });//least significant
    else
        memory.at(destinationValue + offset + 1) = sourceValue.substr(0, 8);
}
void storeByte()
{
    string source;
    string destination;
    int offset;
    getPartsforStoring(source, destination, offset);

    string sourceValue = to_string(registers.at(source));//string holding value in register which is int
    bitset<8> sValue = stoll(sourceValue);//change the int to binary
    sourceValue = sValue.to_string();//make the binary into string so that concatenation occurs

    long long destinationValue = registers.at(destination);//need it integer

    //store into memory
    if (memory.find(destinationValue + offset) == memory.end())
        memory.insert({ destinationValue + offset, sourceValue.substr(0,8) });
    else
        memory.at(destinationValue + offset) = sourceValue.substr(0, 8);
}
void loadUpperImmidate()
{
    string instruction = instructions.at(pc);
    unsigned long pos1 = instruction.find(",");
    string destination = instruction.substr(4, 3);
    if (destination != "s11" && destination != "s10")
        destination.pop_back();
    string intermediate = instruction.substr(pos1 + 2);
    long long constant = stoll(intermediate);
    // long long destinationValue = constant;
    //store into register
    registers.at(destination) = bitExtracted(constant, 20);
}

// Function to extract k bits from p position
// and returns the extracted value as integer
long long bitExtracted(long long num, int k)
{
    vector<int> binaryNum;
    int i = 0;
    long long n = num;
    while (n > 0) {

        // storing remainder in binary array
        binaryNum.push_back(n % 2);
        n = n / 2;
        i++;
    }

    int p = binaryNum.size() - 20 + 1;
    return (((1 << k) - 1) & (num >> (p - 1)));
}

void selectInstruction()
{
    int pos1 = instructions[pc].find(" ");
    string word = instructions[pc].substr(0, pos1);

    cout << instructions[pc] << endl;
    if (word == "ECALL" || word == "EBREAK" || word == "FENCE" || word == "ecall" || word == "ebreak" || word == "fence")
    {
        printingMemAndReg();
        cout << "PC: " << pc << endl;
        cout << "Program ended \n";
        exit(0);
    }
    else if (word == "LB" || word == "lb")
    {
        loadByte();
        printingMemAndReg();
    }
    else if (word == "LBU" || word == "lbu")
    {
        loadByteUnsigned();
        printingMemAndReg();
    }
    else if (word == "LH" || word == "lh")
    {
        loadHalfWord();
        printingMemAndReg();
    }
    else if (word == "LW" || word == "lw")
    {
        loadWord();
        printingMemAndReg();
    }
    else if (word == "LHU" || word == "lhu")
    {
        loadHalfWordUnsigned();
        printingMemAndReg();
    }
    else if (word == "SW" || word == "sw")
    {
        storeWord();
        printingMemAndReg();
    }
    else if (word == "SH" || word == "sh")
    {
        storeHalfWord();
        printingMemAndReg();
    }
    else if (word == "SB" || word == "sb")
    {
        storeByte();
        printingMemAndReg();
    }
    else if (word == "LUI" || word == "lui")
    {
        loadUpperImmidate();
        printingMemAndReg();
    }

    else if (word == "AUIPC" || word == "auipc")
    {
        addUpperImmediatetoPC();
        printingMemAndReg();
    }

    else if (word == "JAL" || word == "jal")
    {
        jal();
        printingMemAndReg();
    }
    else if (word == "JALR" || word == "jalr")
    {
        jalr();
        printingMemAndReg();
    }
    else if (word == "BEQ" || word == "beq")
    {
        branchEqual();
        printingMemAndReg();
    }
    else if (word == "BNE" || word == "bne")
    {
        branchNotEqual();
        printingMemAndReg();
    }
    else if (word == "BLT" || word == "blt")
    {
        branchLessThan();
        printingMemAndReg();
    }
    else if (word == "BLTU" || word == "bltu")
    {
        branchLessThanUnsigned();
        printingMemAndReg();
    }
    else if (word == "BGE" || word == "bge")
    {
        branchGreaterThan();
        printingMemAndReg();
    }
    else if (word == "BGEU" || word == "bgeu")
    {
        branchGreaterThanUnsigned();
        printingMemAndReg();
    }
    else if (word == "ADD" || word == "add")
    {
        add();
        printingMemAndReg();
    }
    else if (word == "ADDI" || word == "addi")
    {
        addImmediate();
        printingMemAndReg();
    }
    else if (word == "SUB" || word == "sub")
    {
        sub();
        printingMemAndReg();
    }
    else if (word == "AND" || word == "and")
    {
        andinstruction();
        printingMemAndReg();
    }
    else if (word == "ANDI" || word == "andi")
    {
        andImmediate();
        printingMemAndReg();
    }
    else if (word == "OR" || word == "or")
    {
        orinstruction();
        printingMemAndReg();
    }
    else if (word == "ORI" || word == "ori")
    {
        orImmediate();
        printingMemAndReg();
    }
    else if (word == "XOR" || word == "xor")
    {
        xorInstruction();
        printingMemAndReg();
    }
    else if (word == "XORI" || word == "xori")
    {
        xorImmediate();
        printingMemAndReg();
    }
    else if (word == "SLL" || word == "sll")
    {
        shiftLeftLogical();
        printingMemAndReg();
    }
    else if (word == "SLLI" || word == "slli")
    {
        shiftLeftLogicalImmediate();
        printingMemAndReg();
    }
    else if (word == "SRL" || word == "srl")
    {
        shiftRightLogical();
        printingMemAndReg();
    }
    else if (word == "SRLI" || word == "srli")
    {
        shiftRightLogical();
        printingMemAndReg();
    }
    else if (word == "SRA" || word == "sra")
    {
        shiftRightAri();
        printingMemAndReg();
    }
    else if (word == "SRAI" || word == "srai")
    {
        shiftRightAriImmediate();
        printingMemAndReg();
    }
    else if (word == "SLT" || word == "slt")
    {
        setLessThan();
        printingMemAndReg();
    }
    else if (word == "SLTI" || word == "slti")
    {
        setLessThanImm();
        printingMemAndReg();
    }
    else if (word == "SLTU" || word == "sltu")
    {
        setLessThanUnsigned();
        printingMemAndReg();
    }
    else if (word == "SLTIU" || word == "sltiu")
    {
        setLessThanUnsignedImm();
        printingMemAndReg();
    }

}

void addUpperImmediatetoPC()    //auipc x4, 256
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");

    string rd = tempInstruction.substr(pos1 + 1, pos2 - pos1 - 1);
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string imm = tempInstruction;
    registers[rd] = pc + bitExtracted(stoll(imm), 20);
}

void jal()          //jal zero, loop
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");

    string rd = tempInstruction.substr(pos1 + 1, pos2 - pos1 - 1);
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label = tempInstruction;

    registers[rd] = pc + 4;
    pc = labels[label];

    cout << " Register Value:" << registers[rd] << endl;
    cout << "New PC:" << pc << endl;

    for (int i = pc; i < maxPC; i += 4)
    {
        selectInstruction();
        pc += 4;
    }

}

void jalr()             //jalr zero, 0(ra)
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    int pos3 = tempInstruction.find("(");

    string rd = tempInstruction.substr(pos1 + 1, pos2 - pos1 - 1);

    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space


    pos3 = tempInstruction.find("(");
    int pos4 = tempInstruction.find(")");
    string offset = tempInstruction.substr(0, pos3);
    string rs = tempInstruction.substr(pos3 + 1, pos4 - pos3 - 1);

    registers[rd] = pc + 4; //changing in zero WRONG
    pc = registers[rs] + stoi(offset);
    for (int i = pc; i < maxPC; i += 4)
    {
        selectInstruction();
        pc += 4;
    }
}

void branchEqual()
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    string rs1 = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label = tempInstruction;

    if (registers[rs1] == registers[rs2])
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }
}

void branchNotEqual()
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    string rs1 = instructions[pc].substr(pos1 + 1, (pos2 - pos1 - 1));
    instructions[pc].erase(instructions[pc].begin(), instructions[pc].begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label = tempInstruction;

    if (registers[rs1] != registers[rs2])
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }
}

void branchLessThan()
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    string rs1 = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label = tempInstruction;

    if (registers[rs1] < registers[rs2])
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }
}

void branchLessThanUnsigned()
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");

    string rs1 = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label(tempInstruction), rs1u, rs2u;

    int pos3 = to_string(registers[rs1]).find("-");
    int pos4 = to_string(registers[rs2]).find("-");

    if (pos3 != -1) {

        to_string(registers[rs1]).erase(to_string(registers[rs1]).begin(), to_string(registers[rs1]).begin() + pos3 + 1);
        rs1u = to_string(registers[rs1]);
    }

    else if (pos4 != -1) {

        to_string(registers[rs2]).erase(to_string(registers[rs2]).begin(), to_string(registers[rs2]).begin() + pos4 + 1);

        rs2u = to_string(registers[rs2]);
    }

    cout << "rs1u" << rs1u << "rs2u" << rs2u << endl;

    if (stoll(rs1u) < stoll(rs2u))
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }
}


void branchGreaterThan()
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");
    string rs1 = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label = tempInstruction;

    if (registers[rs1] >= registers[rs2])
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }

}


void branchGreaterThanUnsigned() // bgtu rs1, rs2, loop
{
    string tempInstruction = instructions[pc];
    int pos1 = tempInstruction.find(" ");
    int pos2 = tempInstruction.find(",");

    string rs1 = tempInstruction.substr(pos1 + 1, (pos2 - pos1 - 1));
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 1); //deletes the first part of the instruction + extra space

    pos1 = tempInstruction.find(" ");
    pos2 = tempInstruction.find(",");
    string rs2 = tempInstruction.substr(1, (pos2 - pos1 - 1)); //
    tempInstruction.erase(tempInstruction.begin(), tempInstruction.begin() + pos2 + 2); //deletes the first part of the instruction + extra space

    string label(tempInstruction), rs1u, rs2u;

    int pos3 = to_string(registers[rs1]).find("-");
    int pos4 = to_string(registers[rs2]).find("-");

    if (pos3 != -1) {

        to_string(registers[rs1]).erase(pos3 - 1, 1);
        rs1u = to_string(registers[rs1]);
    }

    else if (pos4 != -1) {

        to_string(registers[rs2]).erase(pos4 - 1, 1);
        rs2u = to_string(registers[rs2]);
    }

    if (stoll(rs1u) >= stoll(rs2u))
    {
        pc = labels[label];
        for (int i = pc; i < maxPC; i += 4)
        {
            selectInstruction();
            pc += 4;
        }
    }
}

//Farah
void add()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);

    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value + source2Value;

    registers[destination] = destinationValue;
}

void addImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);

    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value + immediateValue;

    registers[destination] = destinationValue;
}

void sub()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value - source2Value;

    registers[destination] = destinationValue;
}

void andinstruction()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value & source2Value;

    registers[destination] = destinationValue;
}

void andImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value & immediateValue;

    registers[destination] = destinationValue;
}

void orinstruction()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value | source2Value;

    registers[destination] = destinationValue;
}

void orImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value | immediateValue;

    registers[destination] = destinationValue;
}

void xorInstruction()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value ^ source2Value;

    registers[destination] = destinationValue;
}

void xorImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value ^ immediateValue;

    registers[destination] = destinationValue;
}

void shiftLeftLogical()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value << source2Value;

    registers[destination] = destinationValue;
}

void shiftLeftLogicalImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value << immediateValue;

    registers[destination] = destinationValue;
}

void shiftRightLogical()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value >> source2Value;

    registers[destination] = destinationValue;
}

void shiftRightLogicalImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value >> immediateValue;

    registers[destination] = destinationValue;
}

void shiftRightAri()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    long long destinationValue = source1Value >> source2Value;

    registers[destination] = destinationValue;
}

void shiftRightAriImmediate()
{
    string instruction = instructions[pc];

    string source1;
    string immediate;
    string destination;

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    long long destinationValue = source1Value >> immediateValue;

    registers[destination] = destinationValue;
}

void setLessThan()
{
    string instruction = instructions[pc];

    string source1;
    string source2;
    string destination;

    long long destinationValue;

    getPartsforRegtoReg(source1, source2, destination);


    long long source1Value = registers[source1];
    long long source2Value = registers[source2];

    if (source1Value < source2Value)
    {
        destinationValue = 1;
    }
    else
    {
        destinationValue = 0;
    }


    registers[destination] = destinationValue;
}

void setLessThanImm()
{
    string instruction;
    string source1;
    string immediate;
    string destination;

    instruction = instructions[pc];

    getPartsforRegtoReg_Imm(source1, immediate, destination);

    long long destinationValue;
    long long source1Value = registers[source1];
    long long immediateValue = stoll(immediate);

    if (source1Value < immediateValue)
    {
        destinationValue = 1;
    }
    else
    {
        destinationValue = 0;
    }


    registers[destination] = destinationValue;
}

void setLessThanUnsigned()
{
    string instruction;
    string source1;
    string source2;
    string destination;
    long long destinationValue;

    instruction = instructions[pc];

    getPartsforRegtoReg(source1, source2, destination);

    string rs1u, rs2u;

    int pos3 = to_string(registers[source1]).find("-");
    int pos4 = to_string(registers[source2]).find("-");

    if (pos3 != -1)
    {
        to_string(registers[source1]).erase(pos3 - 1, 1);
        rs1u = to_string(registers[source1]);
    }

    else if (pos4 != -1)
    {
        to_string(registers[source2]).erase(pos4 - 1, 1);
        rs2u = to_string(registers[source2]);
    }


    if (stoll(rs1u) < stoll(rs2u))
    {
        destinationValue = 1;
    }
    else
    {
        destinationValue = 0;
    }


    registers[destination] = destinationValue;

}

void setLessThanUnsignedImm()
{
    string instruction;
    string source1;
    string immediate;
    string destination;

    long long destinationValue;

    instruction = instructions[pc];

    getPartsforRegtoReg_Imm(source1, immediate, destination);


    string rs1u, immu;

    int pos3 = to_string(registers[source1]).find("-");
    int pos4 = to_string(registers[immediate]).find("-");

    if (pos3 != -1)
    {
        to_string(registers[source1]).erase(pos3 - 1, 1);
        rs1u = to_string(registers[source1]);
    }

    else if (pos4 != -1)
    {
        to_string(registers[immediate]).erase(pos4 - 1, 1);
        immu = to_string(registers[immediate]);
    }


    if (stoll(rs1u) < stoll(immu))
    {
        destinationValue = 1;
    }
    else
    {
        destinationValue = 0;
    }


    registers[destination] = destinationValue;
}










