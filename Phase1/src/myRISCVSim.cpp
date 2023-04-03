
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

#include "../include/myRISCVSim.h"
#include <unordered_map>
#include <math.h>
#include <string.h>
#include <string>
#include <iostream>



using namespace std;

// Register file
int X[32];

// memory
 unordered_map<unsigned int,int> instruction_memory;

 unordered_map<unsigned int,int> MEM;


// intermediate datapath and control path signals
 unsigned int instruction_word;
 unsigned int operand1;
 unsigned int operand2;

int alu_result = 0;

int pc = 0, nextpc = 0;
int clk = 0;


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

  X[2] = 0x7ffffff0; // stack pointer
  X[3] = 0x10000000; // global pointer

  MEM.clear();
  instruction_memory.clear();
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
    if(instruction!=0xffffffff){
      instruction_memory[address] = instruction;
    }
    else{
      break;
    }
    
  }
  int data;
  while (fscanf(fp, "%x %x", &address, &data) != EOF)
  {
    MEM[address]=data;
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
    fprintf(fp, "0x%x 0x%x\n", i+0x10000000, MEM[i+0x10000000]);
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
    fprintf(fp, "X%d : 0x%x\n",i,X[i]);
  }
  fclose(fp);

  cout<<endl;
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
  if(instruction_word==0xffffffff) {
    printf("Exiting...\n");
    swi_exit();
  }
  if(!instruction_memory[pc]){
    printf("No instruction at PC 0x%x\n",pc);
    printf("Exiting...\n");
    swi_exit();
  }
  set_instruction_bin(instruction_word);
  nextpc = pc + 4;

  printf("FETCH: Fetched instruction 0x%x from PC 0x%x\n",instruction_word,pc);
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

  printf("DECODE:\nOperation is %c type\n",instruction.type);
  cout<<"opcode:"<<opcode<<endl;
  printf("func3: %d\n",instruction.func3);
  printf("func7: %d\n",instruction.func7);
  printf("rs1: X%d\n",instruction.rs1);
  printf("rs2: X%d\n",instruction.rs2);
  printf("rd: X%d\n",instruction.rd);
  printf("immediate: 0x%x\n",instruction.immediate);

  
}

// executes the ALU operation based on ALUop
void execute()
{
  // instruction is the structure instance
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
    //handling overflow conditions
    bool overflow = false;
    alu_result = operand1 - operand2;
    
    if(operand1>2147483648 && operand2>0)   
    {
      if(alu_result>0)
      {
        overflow=true;
      }
    }
    if(operand1>2147483648 && operand2>2147483648)
    {
      if(alu_result>0)
      {
        overflow=false;
      }
    }
    if (instruction.func3 == 0)  // beq
    {
      name = "BEQ";
      if(alu_result == 0) nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 1) // bne
    {
      name = "BNE";
      if(alu_result != 0) nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 4) // blt
    {
      name = "BLT";
      if((alu_result < 0) || (overflow==true)) nextpc = pc + instruction.immediate;
    }
    else if (instruction.func3 == 5) // bge
    {
      name = "BGE";
      if(alu_result >= 0 && overflow==false) nextpc = pc + instruction.immediate;
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
  cout<<"EXECUTE: Executed "<<instruction.name<<" operation\n";

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
      MEM_result = MEM[alu_result];
    }
  }

  else {
    printf("MEMORY: Memory not accessed\n");
    return;
  }

  printf("MEMORY: Memory accessed for instruction at PC 0x%x having value 0x%x\n",pc,MEM_result);
}
// writes the results back to register file
int wb_result;
void write_back()
{
  bool write = 0;

  if(instruction.rd!=0){
    if (instruction.opcode == 19 || instruction.opcode == 51)    
    {
      wb_result = alu_result; // the MEM and wb buffer registers(storing required data for wb stage)
      write = 1;
    }
    else if (instruction.opcode == 3)
    {                                 // load instruction
      wb_result = MEM_result;         // here mem result stores the MEM[rs1+imm] in sign extended form
      write = 1;
    }                                 
    // nothing to be done for store instruction in writeback stage                         
    else if (instruction.opcode == 111)
    { // jal instruction
      wb_result = pc + 4;
      write = 1;
    }
    else if (instruction.opcode == 103)
    { // jalr
      write = 1;
      wb_result = pc + 4;
    }
    else if (instruction.opcode == 55)
    { // lui
      write = 1;
      wb_result = alu_result;
    }
    else if (instruction.opcode == 23)
    { // auipc
      write = 1;
      wb_result = alu_result;
    }
  }
  if(write){
    X[instruction.rd] = wb_result;
    printf("WRITEBACK: Write 0x%x to X%d\n",wb_result,instruction.rd);

  }
  else printf("WRITEBACK: No Writeback\n");
  clk++;
  pc = nextpc;
  X[0] = 0;//x0 register always zero
  printf("\nClock cycle: %d\n\n\n",clk);

  
}
