/*
The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of RISCV Processor
Developer's Name: Hardik Aggarwal, Komalpreet Singh, Ritesh Patil, Edgar Aditya Thorpe
Developer's Email id: 2021csb1173@iitrpr.ac.in, 2021csb1237@iitrpr.ac.in, 2021csb1120@iitrpr.ac.in, 2021csb1169@iitrpr.ac.in
Date:
*/

/* myRISCVSim.cpp
   Purpose of this file: implementation file for myRISCVSim
*/
#include <bits/stdc++.h>
#include "../include/myRISCVSim.h"
#include <unordered_map>
#include <math.h>
// #include <string.h>
// #include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

int stalls = 0;

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
unsigned int operand1;
unsigned int operand2;
int pipeline = 1;

struct instruction_set no_op;

struct FD_register
{
  int pc;
  char instruction_bin[32];

} FD_register;

struct DE_register
{

  int pc;
  struct instruction_set instruction;

} DE_register;

struct EM_register
{

  int pc;
  int operand2;
  struct instruction_set instruction;
  int alu_result;

} EM_register;

struct MW_register
{

  int MEM_result = 0;
  struct instruction_set instruction;
  int pc;
  int alu_result;
} MW_register;

using namespace std;

// Register file
int X[32];

// memory
unordered_map<unsigned int, int> instruction_memory;

unordered_map<unsigned int, int> MEM;

// intermediate datapath and control path signals
unsigned int instruction_word;

int alu_result = 0;

int pc = 0, nextpc = 0;
int clk = 0;

void run_riscvsim()
{
  if (!pipeline)
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
  else
  {

    pc = -1;
    fetch();
    decode();
    execute();
    mem();
    write_back();
    pc = 0;

    // MW_register.instruction.rd=0;
    // FD_register.instruction1.rd=0;
    // EM_register.instruction.rd=0;
    // DE_register.instruction.rd=0;
    while (1)
    {
      write_back();
      mem();
      execute();
      decode();
      fetch();
    }
  }
}

void set_instruction_bin(unsigned int a)
{
  for (int i = 0; i < 32; i++)
    instruction.instruction_bin[i] = '0';
  int i = 0;
  while (a)
  {
    if (a % 2 == 1)
      instruction.instruction_bin[i] = '1';
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

  X[2] = 0x7ffffff0; // stack pointer
  X[3] = 0x10000000; // global pointer

  MEM.clear();
  instruction_memory.clear();
  pc = -1;
  instruction_memory[-1] = 0x00000033;
  fetch();
  decode();
  no_op = instruction;
  pc = 0;
  MEM.clear();
  instruction_memory.clear();
  instruction_memory[-1] = 0x00000033;
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
    printf("Error opening input .mc file\n");
    exit(1);
  }
  while (fscanf(fp, "%x %x", &address, &instruction) != EOF)
  {
    if (instruction != 0x7fffffff)
    {
      instruction_memory[address] = instruction;
    }
    else
    {
      break;
    }
  }
  int data;
  while (fscanf(fp, "%x %x", &address, &data) != EOF)
  {
    MEM[address] = data;
  }

  fclose(fp);
}

// writes the data memory in "data_out.mc" file
void write_data_memory()
{
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mc", "w");
  if (fp == NULL)
  {
    printf("Error opening data_out.mc file for writing\n");
    return;
  }

  for (i = 0; i < 4000; i = i + 4)
  {
    fprintf(fp, "0x%x 0x%x\n", i + 0x10000000, MEM[i + 0x10000000]);
  }
  fclose(fp);

  fp = fopen("Register_file.mc", "w");
  if (fp == NULL)
  {
    printf("Error opening Register_file.mc file for writing\n");
    return;
  }

  for (int i = 0; i < 32; i++)
  {
    fprintf(fp, "X%d : 0x%x\n", i, X[i]);
  }
  fclose(fp);

  cout << endl;
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

  // cout << pc << endl;
  nextpc = pc + 4;
  if (stalls > 0)
    return;

  instruction_word = instruction_memory[pc];
  if (instruction_word == 0x7fffffff)
  {
    printf("Exiting...\n");
    swi_exit();
  }
  // if (instruction_memory.find(pc) == instruction_memory.end())
  if (!instruction_memory[pc])
  {
    printf("No instruction at PC 0x%x\n", pc);
    printf("Exiting...\n");
    swi_exit();
  }
  set_instruction_bin(instruction_word);

  printf("FETCH: Fetched instruction 0x%x from PC 0x%x\n", instruction_word, pc);

  if (pipeline)
  {
    FD_register.pc = pc;
    for (int i = 0; i < 32; i++)
    {
      FD_register.instruction_bin[i] = instruction.instruction_bin[i];
    }
  }
}
// reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode()
{

  if (pipeline)
  {
    // instruction.instruction_bin = FD_register.instruction_bin;
    for (int i = 0; i < 32; i++)
    {
      instruction.instruction_bin[i] = FD_register.instruction_bin[i];
    }
    instruction.opcode = bin2dec(0, 6);
    instruction.rd = bin2dec(7, 11);
    instruction.rs1 = bin2dec(15, 19);
    instruction.rs2 = bin2dec(20, 24);
    instruction.func3 = bin2dec(12, 14);
    instruction.func7 = bin2dec(25, 31);
    if (stalls == 0)
    {
      if ((instruction.rs1 == DE_register.instruction.rd || instruction.rs2 == DE_register.instruction.rd) && DE_register.instruction.rd != 0)
      {
        stalls = 3;
        // cout << "stalls: " << stalls << endl;
        // cout << "rd: " << DE_register.instruction.rd << endl;
        // cout << "rs1/rs2: " << instruction.rs1 << " " << instruction.rs2 << endl;
      }
      else if ((instruction.rs1 == EM_register.instruction.rd || instruction.rs2 == EM_register.instruction.rd) && EM_register.instruction.rd != 0)
      {

        stalls = 2;
        // cout << "stalls: " << stalls << endl;
        // cout << "rd: " << EM_register.instruction.rd << endl;
        // cout << "rs1/rs2: " << instruction.rs1 << " " << instruction.rs2 << endl;
      }
      else if ((instruction.rs1 == MW_register.instruction.rd || instruction.rs2 == MW_register.instruction.rd) && MW_register.instruction.rd != 0)
      {
        stalls = 1;
        // cout << "stalls: " << stalls << endl;
        // cout << "rd: " << MW_register.instruction.rd << endl;
        // cout << "rs1/rs2: " << instruction.rs1 << " " << instruction.rs2 << endl;
      }
    }

    cout << "stalls: " << stalls << endl;
    cout << "rd: " << DE_register.instruction.rd << endl;
    cout << "rd: " << EM_register.instruction.rd << endl;
    cout << "rd: " << MW_register.instruction.rd << endl;
    cout << "rs1/rs2: " << instruction.rs1 << " " << instruction.rs2 << endl;

    if (stalls > 0)
    {
      DE_register.pc = -1;
      DE_register.instruction = no_op;
      stalls--;
    }
    else
    {
      DE_register.pc = FD_register.pc;
      DE_register.instruction = instruction;
    }
    if (stalls > 0)
      return;
  }

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

  printf("DECODE:\nOperation is %c type\n", instruction.type);
  cout << "opcode:" << opcode << endl;
  printf("func3: %d\n", instruction.func3);
  printf("func7: %d\n", instruction.func7);
  printf("rs1: X%d\n", instruction.rs1);
  printf("rs2: X%d\n", instruction.rs2);
  printf("rd: X%d\n", instruction.rd);
  printf("immediate: 0x%x\n", instruction.immediate);

  if (pipeline)
  {
    DE_register.instruction = instruction;
    DE_register.pc = FD_register.pc;
  }
}

// executes the ALU operation based on ALUop
void execute()
{

  int curr_pc = pc;
  // instruction is the structure instance
  if (pipeline)
  {
    instruction = DE_register.instruction;
    curr_pc = DE_register.pc;
  }
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
    // handling overflow conditions
    bool overflow = false;
    alu_result = operand1 - operand2;

    if (operand1 > 2147483648 && operand2 > 0)
    {
      if (alu_result > 0)
      {
        overflow = true;
      }
    }
    if (operand1 > 2147483648 && operand2 > 2147483648)
    {
      if (alu_result > 0)
      {
        overflow = false;
      }
    }
    if (instruction.func3 == 0) // beq
    {
      name = "BEQ";
      if (alu_result == 0)
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 1) // bne
    {
      name = "BNE";
      if (alu_result != 0)
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 4) // blt
    {
      name = "BLT";
      if ((alu_result < 0) || (overflow == true))
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 5) // bge
    {
      name = "BGE";
      if (alu_result >= 0 && overflow == false)
        nextpc = curr_pc + instruction.immediate;
    }
    break;
  }

  case 111: // jal
  {
    name = "JAL";
    nextpc = curr_pc + instruction.immediate;
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
    alu_result = curr_pc + (instruction.immediate << 12);
    break;
  }
  }
  cout << "EXECUTE: Executed " << instruction.name << " operation\n";
  if (pipeline)
  {
    EM_register.instruction = instruction;
    EM_register.alu_result = alu_result;
    EM_register.operand2 = operand2;
    EM_register.pc = curr_pc;
  }
}
// perform the memory operation
int MEM_result = 0;

void mem()
{
  bool access = true;
  int curr_pc = pc;

  if (pipeline)
  {
    instruction = EM_register.instruction;
    alu_result = EM_register.alu_result;
    curr_pc = EM_register.pc;
  }
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
  { // store memory access
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
      MEM_result = MEM[alu_result];
    }
  }

  else
  {
    printf("MEMORY: Memory not accessed\n");
    access = 0;
  }

  if (access)
    printf("MEMORY: Memory accessed for instruction at PC 0x%x having value 0x%x\n", curr_pc, MEM_result);

  if (pipeline)
  {
    MW_register.instruction = instruction;
    MW_register.MEM_result = MEM_result;
    MW_register.pc = EM_register.pc;
    MW_register.alu_result = EM_register.alu_result;
  }
}

// writes the results back to register file
int result;

void write_back()
{
  int curr_pc = pc;
  if (pipeline)
  {
    instruction = MW_register.instruction;
    MEM_result = MW_register.MEM_result;
    alu_result = MW_register.alu_result;
    curr_pc = MW_register.pc;
  }

  // printf("Instruction rd: %d\n", instruction.rd);

  bool write = 0;

  if (instruction.rd != 0)
  {
    if (instruction.opcode == 19 || instruction.opcode == 51)
    {
      result = alu_result; // the MEM and wb buffer registers(storing required data for wb stage)
      write = 1;
    }
    else if (instruction.opcode == 3)
    {                      // load instruction
      result = MEM_result; // here mem result stores the MEM[rs1+imm] in sign extended form
      write = 1;
    }
    // nothing to be done for store instruction in writeback stage
    else if (instruction.opcode == 111)
    { // jal instruction
      result = curr_pc + 4;
      write = 1;
    }
    else if (instruction.opcode == 103)
    { // jalr
      write = 1;
      result = curr_pc + 4;
    }
    else if (instruction.opcode == 55)
    { // lui
      write = 1;
      result = alu_result;
    }
    else if (instruction.opcode == 23)
    { // auipc
      write = 1;
      result = alu_result;
    }
  }
  if (write)
  {
    X[instruction.rd] = result;
    printf("WRITEBACK: Write 0x%x to X%d\n", result, instruction.rd);
  }
  else
  {
    printf("No Writeback\n");
  }
  if (nextpc == 3)
    nextpc = 0;
  if (stalls == 0)
    pc = nextpc;

  printf("\n\nFD_reg: pc=%d\n", FD_register.pc);
  printf("DE_reg: pc=%d, rd=%d\n", DE_register.pc, DE_register.instruction.rd);
  printf("EM_reg: pc=%d, rd=%d\n", EM_register.pc, EM_register.instruction.rd);
  printf("MW_reg: pc=%d, rd=%d\n\n\n", MW_register.pc, MW_register.instruction.rd);
}
