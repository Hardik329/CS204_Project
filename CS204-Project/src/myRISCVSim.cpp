
/*

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of RISCV Processor

Developer's Name:
Developer's Email id:
Date:

*/

/* myRISCVSim.cpp
   Purpose of this file: implementation file for myRISCVSim
*/

#include "myRISCVSim.h"
#include <stdio.h>
#include <unordered_map>
#include <math.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

// Register file
static unsigned int X[32];
// flags

// memory
static unordered_map<unsigned int,int> instruction_memory;

static unordered_map<unsigned int,int> MEM;


// intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;

int alu_result = 0;

int pc = 0, nextpc = 0;
int clock = 0;

struct instruction_set
{

  char instruction_bin[32];
  int rs1, rs2;
  int rd;
  char opcode;
  int immediate;
  int func3, func7;
  char type;
  string name;


} instruction;


void run_riscvsim()
{
  while (1)
  {
    fetch();
    decode();
    execute();
    mem();
    write_back();
  }
}

void set_instruction_bin(unsigned int a)
{
  for(int i=0;i<32;i++) instruction.instruction_bin[i]='0';
  int i = 0;
  while (a)
  {
    if(a%2==1) instruction.instruction_bin[i]='1';
    a /= 2;
    i++;
  }
}

// it is used to set the reset values
// reset all registers and memory content to 0
void reset_proc()
{
  for (int i = 0; i < 32; i++)
    X[i] = 0;
  for (int i = 0; i < 1000; i++)
    MEM[i] = 0;
  for (int i = 0; i < 1000; i++)
    instruction_memory[i] = 0;
}

// load_program_memory reads the input memory, and populates the instruction
//  memory
void load_program_memory(char *file_name)
{
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if (fp == NULL)
  {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while (fscanf(fp, "%x %x", &address, &instruction) != EOF)
  {
    instruction_memory[address/4] = instruction;
  }
  fclose(fp);
}

// writes the data memory in "data_out.mem" file
void write_data_memory()
{
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if (fp == NULL)
  {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }

  for (i = 0; i < 4000; i = i + 4)
  {
    fprintf(fp, "%x %x\n", i, MEM[i/4]);
  }
  fclose(fp);
}

// should be called when instruction is swi_exit
void swi_exit()
{
  write_data_memory();
  exit(0);
}

int bin2dec(int a, int b)
{
  int ans = 0;
  int p = 1;
  for (int i = a; i <= b; i++)
  {
    ans += (instruction.instruction_bin[i] - '0') * p;
    p *= 2;
  }

  return ans;
}

// reads from the instruction memory and updates the instruction register
void fetch()
{
  if(!instruction_memory[pc/4]){
    printf("No instruction at PC %x\n",pc);
    swi_exit();
  }
  instruction_word = instruction_memory[pc/4];
  set_instruction_bin(instruction_word);
  nextpc = pc + 4;
  if(instruction_word==0x7fffffff) swi_exit();

  printf("Fetched instruction at PC 0x%x\n",pc);
}


// reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode()
{

  instruction.opcode = bin2dec(0, 6);
  instruction.rd = bin2dec(7, 11);
  instruction.rs1 = bin2dec(15, 19);
  instruction.rs2 = bin2dec(20, 24);
  instruction.func3 = bin2dec(12, 14);
  instruction.func7 = bin2dec(25, 31);

  int opcode = instruction.opcode;

  switch (opcode)
  {

  case 51:
    instruction.type = 'R';
    break;

  case 19:
  case 3:
  case 103:
    instruction.type = 'I';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(20, 31);
    }
    else if (instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(20, 31)) - pow(2, 12);
    }
    break;

  case 35:
    instruction.type = 'S';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = instruction.rd + ((instruction.func7) * 32);
    }
    else if (instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (instruction.rd + ((instruction.func7) * 32)) - pow(2, 12);
    }
    break;

  case 99:
    instruction.type = 'B';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(8, 11) + ((bin2dec(25, 30)) * 16) + (pow(2, 10) * (instruction.instruction_bin[7] - '0'));
    }
    else if (instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = bin2dec(8, 11) + ((bin2dec(25, 30)) * 16) + (pow(2, 10) * (instruction.instruction_bin[7] - '0')) - pow(2, 11);
    }
    instruction.immediate *= 2;
    break;

  case 111:
    instruction.type = 'J';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(21, 30) + (pow(2, 10) * (instruction.instruction_bin[20] - '0')) + (bin2dec(12, 19) * pow(2, 11));
    }
    else if (instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = bin2dec(21, 30) + (pow(2, 10) * (instruction.instruction_bin[20] - '0')) + (bin2dec(12, 19) * pow(2, 11)) - pow(2, 19);
    }
    instruction.immediate *= 2;
    break;

  case 55:
  case 23:
    instruction.type = 'U';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(12, 31);
    }
    else if (instruction.instruction_bin[31] == '1')
    { 
      instruction.immediate = (bin2dec(12, 31)) - pow(2, 20);
    }
    break;
  }

  printf("Decoded a %c type instruction at PC 0x%x\n",instruction.type,pc);
  cout<<"opcode:"<<opcode<<endl;
  printf("func3: %d\n",instruction.func3);
  printf("func7: %d\n",instruction.func7);
  printf("immediate: %d\n",instruction.immediate);
  printf("rs1: %d\n",instruction.rs1);
  printf("rs2: %d\n\n",instruction.rs2);

  
}

// executes the ALU operation based on ALUop
void execute()
{
  // i is the structure instace from decode
  operand1 = X[instruction.rs1];
  operand2 = X[instruction.rs2];
  string &name = instruction.name;

  switch (instruction.opcode)
  { // distributing with respect to different func3
  case 51:
  { // opcode of R type returns 51 decimal value

    if (instruction.func3 == 0 && instruction.func7 == 0) // add
    {
      name = "ADD";
      alu_result = operand1 + operand2;
    }
    else if (instruction.func3 == 0 && instruction.func7 == 32) // sub
    {                                                           // func7 for sub is 32
      name = "SUB";
      alu_result = operand1 - operand2;
    }
    else if (instruction.func3 == 4) // xor
    {
      name = "XOR";
      alu_result = operand1 ^ operand2;
    }
    else if (instruction.func3 == 6) // or
    {
      name = "OR";
      alu_result = operand1 | operand2;
    }
    else if (instruction.func3 == 7) // and
    {
      name = "AND";
      alu_result = operand1 & operand2;
    }
    else if (instruction.func3 == 1) // sll
    {
      name = "SLL";
      alu_result = operand1 << operand2;
    }
    else if (instruction.func3 == 5 && instruction.func7 == 0) // srl
    {
      name = "SRL";
      alu_result = (int)((unsigned int)operand1 >> operand2);
    }
    else if (instruction.func3 == 5 && instruction.func7 == 32) // sra
    {
      name = "SRA";
      alu_result = operand1 >> operand2;
    }
    else if (instruction.func3 == 2) // slt
    {
      name = "SLT";
      if (operand1 < operand2)
      {
        alu_result = 1;
      }
      else
      {
        alu_result = 0;
      }
    }
    break;
  }

  case 19: // I format(immediate adressing)
  {
    operand2 = instruction.immediate;

    if (instruction.func3 == 0) // addi
    {
      name = "ADDI";
      alu_result = operand1 + operand2;
    }
    else if (instruction.func3 == 6) // ori
    {
      name = "ORI";
      alu_result = operand1 | operand2;
    }
    else if (instruction.func3 == 7) // andi
    {
      name = "ANDI";
      alu_result = operand1 & operand2;
    }
    break;
  }

  case 3: // I format with register adressing(TH load instructions)  lb,lh,lw
  {
    name = "LOAD";
    alu_result = operand1 + instruction.immediate;
    break;
  }
  case 35: // store instructions sb,sh,sw
  {
    name = "STORE";
    alu_result = operand1 + instruction.immediate;
    break;
  }
  case 99: // branch instructions  beq,bne,bge,blt
  {
    alu_result = operand1 - operand2;
    if (instruction.func3 == 0 && alu_result == 0)  // beq
    {
      name = "BEQ";
      nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 1 && alu_result != 0) // bne
    {
      name = "BNE";
      nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 4 && alu_result < 0) // blt
    {
      name = "BLT";
      nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 5 && alu_result >= 0) // bge
    {name = "BGE";
      nextpc = pc + instruction.immediate;
    }

    break;
  }
  case 111: // jal
  {
    name = "JAL";
    nextpc = pc + instruction.immediate;
    break;
  }
  case 103: // jalr
  {
    name = "JALR";
    alu_result = operand1 + instruction.immediate;
    nextpc = alu_result;
    break;
  }
  case 55: // lui
  {
    name = "LUI";
    alu_result = instruction.immediate << 12;
    break;
  }
  case 23: // auipc
  {
    name = "AUIPC";
    alu_result = pc + (instruction.immediate << 12);
    break;
  }
  }
  printf("Executed instruction at PC 0x%x\n",pc);

}
// perform the memory operation
int MEM_result = 0;
void mem()
{
  if (instruction.opcode == 3)
  { // mem for load instruction
    if (instruction.func3 == 0)
    {
      unsigned int temp = 0;
      if (MEM[alu_result] < 0)
      { // 2 compliment representation of MEM[alu_result]
        temp = pow(2, 32) + MEM[alu_result];
      }
      else
      {
        temp = MEM[alu_result];
      }
      int array[32];
      for (int i = 0; i <= 32; i++)
      {
        array[i] = temp % 2;
        temp = temp / 2;
      }
      int f = 1;
      for (int i = 0; i < 7; i++)
      {
        MEM_result = f * array[i] + MEM_result;
        f = f * 2;
      }
      MEM_result = MEM_result - array[7] * pow(2, 7);
    }
    else if (instruction.func3 == 1)
    {
      unsigned int temp = 0;
      if (MEM[alu_result] < 0)
      { // 2 compliment representation of MEM[alu_result]
        temp = pow(2, 32) + MEM[alu_result];
      }
      else
      {
        temp = MEM[alu_result];
      }
      int array[32];
      for (int i = 0; i <= 32; i++)
      {
        array[i] = temp % 2;
        temp = temp / 2;
      }
      int f = 1;
      for (int i = 0; i < 15; i++)
      {
        MEM_result = f * array[i] + MEM_result;
        f = f * 2;
      }
      MEM_result = MEM_result - array[15] * pow(2, 15);
    }
    else if (instruction.func3 == 2)
    {
      MEM_result = MEM[alu_result];
    }
  }
  
  else if (instruction.opcode == 35)
  {                                        //store memory access
    MEM_result = 0;
    if (instruction.func3 == 0)
    {
      unsigned int temp = 0;
      if (MEM[alu_result] < 0)
      { // 2 compliment representation of MEM[alu_result]
        temp = pow(2, 32) + MEM[alu_result];
      }
      else
      {
        temp = MEM[alu_result];
      }
      int array[32];
      for (int i = 0; i <= 32; i++)
      {
        array[i] = temp % 2;
        temp = temp / 2;
      }
      unsigned int temp1 = 0;
      if (X[instruction.rs2] < 0)
      { // 2 compliment representation of X[instruction.rs2]
        temp1 = pow(2, 32) + X[instruction.rs2];
      }
      else
      {
        temp1 = X[instruction.rs2];
      }
      int array1[32];
      for (int i = 0; i <= 32; i++)
      {
        array1[i] = temp1 % 2;
        temp1 = temp1 / 2;
      }
      for (int i = 0; i <= 7; i++)
      {
        array[i] = array1[i];
      }
      int f = 1;
      for (int i = 0; i < 31; i++)
      {
        MEM_result = f * array[i] + MEM_result;
        f = f * 2;
      }
      MEM_result = MEM_result - array[31] * pow(2, 31);
      MEM[alu_result] = MEM_result;
    }
    else if (instruction.func3 == 1)
    {
      unsigned int temp = 0;
      if (MEM[alu_result] < 0)
      { // 2 compliment representation of MEM[alu_result]
        temp = pow(2, 32) + MEM[alu_result];
      }
      else
      {
        temp = MEM[alu_result];
      }
      int array[32];
      for (int i = 0; i <= 32; i++)
      {
        array[i] = temp % 2;
        temp = temp / 2;
      }
      unsigned int temp1 = 0;
      if (X[instruction.rs2] < 0)
      { // 2 compliment representation of X[instruction.rs2]
        temp1 = pow(2, 32) + X[instruction.rs2];
      }
      else
      {
        temp1 = X[instruction.rs2];
      }
      int array1[32];
      for (int i = 0; i <= 32; i++)
      {
        array1[i] = temp1 % 2;
        temp1 = temp1 / 2;
      }
      for (int i = 0; i <= 15; i++)
      {
        array[i] = array1[i];
      }
      int f = 1;
      for (int i = 0; i < 31; i++)
      {
        MEM_result = f * array[i] + MEM_result;
        f = f * 2;
      }
      MEM_result = MEM_result - array[31] * pow(2, 31);
      MEM[alu_result] = MEM_result;
    }
    else if (instruction.func3 == 2)
    {
      MEM[alu_result] = X[instruction.rs2];
    }
  }

  else return;

  printf("Memory accessed for instruction at PC 0x%x\n",pc);
}
// writes the results back to register file
void write_back()
{
  if (instruction.opcode == 19 || instruction.opcode == 51)
  {
    X[instruction.rd] = alu_result; // the MEM and wb buffer registers(storing required data for wb stage)
  }
  else if (instruction.opcode == 3)
  {                                 // load instruction
    X[instruction.rd] = MEM_result; // here mem result stores the M[rs1+imm] in sign extended form
  }                                 // nothing to be done for store instruction in writeback stage
  else if (instruction.opcode == 111)
  { // jal instruction
    X[instruction.rd] = pc + 4;
  }
  else if (instruction.opcode == 103)
  { // jalr
    X[instruction.rd] = pc + 4;
  }
  else if (instruction.opcode == 55)
  { // lui
    X[instruction.rd] = alu_result;
  }
  else if (instruction.opcode == 23)
  { // auipc
    X[instruction.rd] = alu_result;
  }
  clock++;
  printf("Writeback for instruction at PC 0x%x\n",pc);
  cout<<"Instruction info: "<<instruction.name<<" instruction\n";
  pc=nextpc;

  printf("Clock cycle: %d\n\n",clock);

  
}