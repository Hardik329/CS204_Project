
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
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include<math.h>

// Register file
static unsigned int X[32];
// flags
// memory
static unsigned char MEM[4000];
// map<unsigned int,int> data_memory;
static unsigned int instruction_memory[4000];

// map<int,int> MEM;

// intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;

int alu_result = 0;

int pc = 0, nextpc = 0;

struct instruction_set
{

  char instruction_bin[32];
  int rs1, rs2;
  int rd;
  char opcode;
  int immediate;
  int func3, func7;
  char type;

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

void set_instruction_bin(int a)
{
  int i = 0;
  while (a)
  {
    instruction.instruction_bin[i] = a % 2 + '0';
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
    address /= 4;
    // write_word(MEM, address, instruction);
    instruction_memory[address] = instruction;
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
    fprintf(fp, "%x %x\n", i, MEM[i]);
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
  instruction_word = instruction_memory[pc];
  set_instruction_bin(instruction_word);
  nextpc = pc + 4;
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
    instruction.type = 'I';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(20,31);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(20,31))-pow(2,12);
    }
    break;
  case 3:
    instruction.type = 'I';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(20,31);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(20,31))-pow(2,12);
    }
    break;
  case 103:
    instruction.type = 'I';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(20,31);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(20,31))-pow(2,12);
    }
    break;

  case 35:
    instruction.type = 'S';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = instruction.rd + ((instruction.func7) * 32);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (instruction.rd + ((instruction.func7) * 32))-pow(2,12);
    }
    break;

  case 99:
    instruction.type = 'B';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(8,11) + ((bin2dec(25,30)) * 16) + (pow(2,10)*(instruction.instruction_bin[7]-'0'));
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = bin2dec(8,11) + ((bin2dec(25,30)) * 16) + (pow(2,10)*(instruction.instruction_bin[7]-'0')) -pow(2,11) ;
    }
    break;

  case 111:
    instruction.type = 'J';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(21,30) + (pow(2,10)*(instruction.instruction_bin[20]-'0')) + (bin2dec(12,19)*pow(2,11));
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = bin2dec(21,30) + (pow(2,10)*(instruction.instruction_bin[20]-'0')) + (bin2dec(12,19)*pow(2,11)) - pow(2,19);
    }
    break;

  case 55:
    instruction.type = 'U';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(12,31);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(12,31))-pow(2,20);
    }
    break;
  case 23:
    instruction.type = 'U';
    if (instruction.instruction_bin[31] == '0')
    {
      instruction.immediate = bin2dec(12,31);
    }
    else if(instruction.instruction_bin[31] == '1')
    {
      instruction.immediate = (bin2dec(12,31))-pow(2,20);
    }
    break;
  }
}

// executes the ALU operation based on ALUop
void execute()
{
  // i is the structure instace from decode
  operand1 = X[instruction.rs1];
  operand2 = X[instruction.rs2];

  switch (instruction.opcode)
  { // distributing with respect to different func3
  case 51:
  { // opcode of R type returns 51 decimal value

    if (instruction.func3 == 0 && instruction.func7 == 0)  //add
    {
      alu_result = operand1 + operand2;
    }
    else if (instruction.func3 == 0 && instruction.func7 == 32)  //sub
    { // func7 for sub is 32
      alu_result = operand1 - operand2;
    }
    else if (instruction.func3 == 4)  //xor
    {
      alu_result = operand1 ^ operand2;
    }
    else if (instruction.func3 == 6)  //or
    {
      alu_result = operand1 | operand2;
    }
    else if (instruction.func3 == 7)  //and
    {
      alu_result = operand1 & operand2;
    }
    else if (instruction.func3 == 1) //sll
    {
      alu_result = operand1 << operand2;
    }
    else if (instruction.func3 == 5 && instruction.func7 == 0) //srl
    {
      int m = (unsigned)operand1;
      alu_result = m >> operand2;
    }
    else if (instruction.func3 == 5 && instruction.func7 == 32) //sra
    {
      alu_result = operand1 >> operand2;
    }
    else if (instruction.func3 == 2) // slt
    {
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

  case 19: // I format(immidiate adressing)
  {
    operand2 = instruction.immediate;

    if (instruction.func3 == 0 && instruction.func7 == 0)    //addi
    {
      alu_result = operand1 + operand2;
    }
    else if (instruction.func3 == 6)     //ori
    {
      alu_result = operand1 | operand2;
    }
    else if (instruction.func3 == 7)    //andi
    {
      alu_result = operand1 & operand2;
    }
    break;
  }

  case 3: // I format with register adressing(TH load instructions)  lb,lh,lw
  {
    alu_result = operand1 + instruction.immediate;
    break;
  }
  case 35: // store instructions sb,sh,sw
  {
    alu_result = operand1 + instruction.immediate;
    break;
  }
  case 99: // branch instructions  beq,bne,bge,blt
  {
    alu_result = operand1 - operand2;
    if(instruction.func3==0&& alu_result==0){
      nextpc=pc +instruction.immediate;
    }
    else if(instruction.func3==1 && alu_result!=0){
      nextpc=pc+instruction.immediate;
    }
    else if(instruction.func3==4 && alu_result<0){
      nextpc=pc+instruction.immediate;
    }
    else if(instruction.func3==5 && alu_result>=0){
      nextpc=pc+instruction.immediate;
    }
    
    break;
    break;
  }
  case 111: //jal
  {
    break;
  }
  case 103:   //jalr
  {
    alu_result = operand1 + instruction.immediate;
    break;
  }
  case 55: //lui
  {
    break;
  }
  case 23: //auipc
  {
    break;
  }
  }
}
// perform the memory operation
void mem()
{
  
}
// writes the results back to register file
void write_back()
{

}

// int read_word(char *mem, unsigned int address) {
//   int *data;
//   data =  (int*) (mem + address);
//   return *data;
// }

// void write_word(char *mem, unsigned int address, unsigned int data) {
//   int *data_p;
//   data_p = (int*) (mem + address);
//   *data_p = data;
// }
