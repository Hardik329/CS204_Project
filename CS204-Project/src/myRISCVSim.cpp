
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


//Register file
static unsigned int X[32];
//flags
//memory
map<unsigned int,int> instruction_memory;

static unsigned char MEM[4000];
// map<unsigned int,int> data_memory;
static unsigned int instruction_memory[4000];



// map<int,int> MEM;

//intermediate datapath and control path signals
static unsigned int instruction_word;
static unsigned int operand1;
static unsigned int operand2;

int pc=0,nextpc=0;

struct instruction_set{

    char instruction_bin[32];
    int rs1[5],rs2[5];
    int rd[5];
    char opcode[7];
    int immediate[20];
    int func3[3], func7[7];
    char type;

} instruction;

void run_riscvsim() {
  while(1) {
    fetch();
    decode();
    execute();
    mem();
    write_back();
  }
}

void set_instruction_bin(int a){
  int i=0;
  while(a){
    instruction.instruction_bin[i]=a%2+'0';
    a/=2;
    i++;
  }
}

// it is used to set the reset values
//reset all registers and memory content to 0
void reset_proc() {
  for(int i=0;i<32;i++) X[i]=0;
  for(int i=0;i<1000;i++) MEM[i]=0;
  for(int i=0;i<1000;i++) instruction_memory[i]=0;
}

//load_program_memory reads the input memory, and populates the instruction 
// memory
void load_program_memory(char *file_name) {
  FILE *fp;
  unsigned int address, instruction;
  fp = fopen(file_name, "r");
  if(fp == NULL) {
    printf("Error opening input mem file\n");
    exit(1);
  }
  while(fscanf(fp, "%x %x", &address, &instruction) != EOF) {
    address/=4;
    // write_word(MEM, address, instruction);
    instruction_memory[address]=instruction;
  }
  fclose(fp);
}

//writes the data memory in "data_out.mem" file
void write_data_memory() {
  FILE *fp;
  unsigned int i;
  fp = fopen("data_out.mem", "w");
  if(fp == NULL) {
    printf("Error opening dataout.mem file for writing\n");
    return;
  }
  
  for(i=0; i < 4000; i = i+4){
    fprintf(fp, "%x %x\n", i, MEM[i]);
  }
  fclose(fp);
}

//should be called when instruction is swi_exit
void swi_exit() {
  write_data_memory();
  exit(0);
}



//reads from the instruction memory and updates the instruction register
void fetch() {
    instruction_word = instruction_memory[pc];
    set_instruction_bin(instruction_word);
    nextpc=pc+4;
}

//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode() {
  char* ans;
  for(int i=0;i<7;i++) instruction.opcode[i]=instruction.instruction_bin[i];
  char* opcode=instruction.opcode;
  for(int i=7;i<=11;i++) instruction.rd[i-7]=instruction.instruction_bin[i];
  for(int i=15;i<=19;i++) instruction.rs1[i-15]=instruction.instruction_bin[i];
  for(int i=20;i<=24;i++) instruction.rs2[i-20]=instruction.instruction_bin[i];
  for(int i=12;i<=14;i++) instruction.func3[i-12]=instruction.instruction_bin[i];
  for(int i=25;i<=31;i++) instruction.func7[i-25]=instruction.instruction_bin[i];


  
  if(strcmp(opcode,"0110011")==0) instruction.type='R';
  else if(strcmp(opcode,"0010011")==0 || strcmp(opcode,"0000011")==0 || strcmp(opcode,"1100111")==0) instruction.type='I';
  else if(strcmp(opcode,"0100011")==0) instruction.type='S';
  else if(strcmp(opcode,"1100011")==0) instruction.type='B';
  else if(strcmp(opcode,"1101111")==0) instruction.type='J';
  else if(strcmp(opcode,"0110111")==0 || strcmp(opcode,"0010111")==0) instruction.type='U';
 



}

//executes the ALU operation based on ALUop
void execute()
{
  // i is the structure instace from decode
  switch (i.opcode)
  { // distributing with respect to different fun3
  case 51:
  { // opcode of R type returns 51 decimal value
    if (i.fun3 == 0 && i.fun7 == 0)
    {
      alu_result = i.rs1 + i.rs2;
    }
    else if (i.fun3 == 0 && i.fun7 == 32)
    { // fun7 for sub is 32
      alu_result = i.rs1 - i.rs2;
    }
    else if (i.func3 == 4)
    {
      alu_result = i.rs1 ^ i.rs2;
    }
    else if (i.func3 == 6)
    {
      alu_result = i.rs1 | i.rs2;
    }
    else if (i.func3 == 7)
    {
      alu_result = i.rs1 & i.rs2;
    }
    else if (i.func3 == 1)
    {
      alu_result = i.rs1 << i.rs2;
    }
    else if (i.func3 == 5 && i.func7 == 0)
    {
      int m = (unsigned)i.rs1;
      alu_result = m >> i.rs2;
    }
    else if (i.func3 == 5 && i.func7 == 32)
    {
      alu_result = i.rs1 >> i.rs2;
    }
    break;
  }

  case 19: // I format(immidiate adressing)
  {
    if (i.fun3 == 0 && i.fun7 == 0)
    {
      alu_result = i.rs1 + i.imm;
    }
    else if (i.func3 == 4)
    {
      alu_result = i.rs1 ^ i.imm;
    }
    else if (i.func3 == 6)
    {
      alu_result = i.rs1 | i.imm;
    }
    else if (i.func3 == 7)
    {
      alu_result = i.rs1 & i.rs2;
    }
  }
  case 3://I format with register adressing(TH load instructions)
  {
    alu_result=i.r1+i.imm;
  }
  case 35://store instructions
  {
    alu_result=i.rs1+i.imm;
  }
  case 99://branch instructions
  {
    alu_result=i.rs1-i.rs2;
  }

  }
}
//perform the memory operation
void mem() {

}
//writes the results back to register file
void write_back() {

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
