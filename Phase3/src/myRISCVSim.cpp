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
// #include <iostream>
// #include <stdio.h>
#include <cstdlib>
#include <time.h>

int target_pc;

using namespace std;

int stalls = 0;
bool kuchto = false;

int cycles = 0;
int instructions = 0;
float cpi;
int dataTransfer = 0;
int alu_count = 0;
int controlInstructions = 0;
int totalStalls = 0;
int dataHazards = 0;
int controlHazards = 0;
int branchMiss = 0;
int stalls_data = 0;
int stalls_control = 0;

unordered_map<int, pair<int, bool>> BTB;

int I_cold_miss = 0, I_capacity_miss = 0, I_conflict_miss = 0, I_hits = 0;
int D_cold_miss = 0, D_capacity_miss = 0, D_conflict_miss = 0, D_hits = 0;
int I_access = 0, D_access = 0;

// vector<int> block;
struct block
{
  vector<pair<int, int>> instructions; // pc,instruction_word
  int tag;
  int time;
  int freq;
};

unordered_map<int, unordered_map<int, bool>> MCT_I;
unordered_map<int, unordered_map<int, bool>> MCT_D;

unordered_map<int, unordered_map<int, struct block>> blocks; // index,tag->block
// unordered_map<int,vector<int>> direct_map;
// unordered_map<int,vector<vector<int>>> set_associative;
// unordered_map<int,vector<vector<int>>> fully_associative;

int rep_policy;

// unordered_map<int, queue<struct block>> FIFO;
// unordered_map<int,vector<priority_queue<pair<int,struct block>>,greater<pair<int,struct block>priority_queue<pair<int,struct block>>> LFU;
// unordered_map<int, priority_queue<pair<int, struct block>, vector<pair<int, struct block>>, greater<pair<int, struct block>>>> LFU;
// unordered_map<int, priority_queue<pair<int, struct block>>> LRU;

unordered_map<int, list<struct block>> instruction_cache;
unordered_map<int, list<struct block>> data_cache;
// unordered_map<int,list<struct block>> LRU;
// unordered_map<int,list<struct block>> LFU;

int cache_size;     // in KB
int block_size;     // in bytes
int cache_type = 0; // 1,2,3 -> DM, FA, SA
int set_ways;
int cache_sets;

int tag_bits;

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

bool KnobForPrintingRegister = true;
bool KnobForPipeline = 1;
bool KnobForPrintingPipelineInfo = false;
bool KnobforSpecificPipelineInfo = false;
bool KnobforDataForwarding = false;

struct instruction_set no_op;
queue<struct FD_register_template> FD_reg;
queue<struct DE_register_template> DE_reg;
queue<struct EM_register_template> EM_reg;
queue<struct MW_register_template> MW_reg;

struct FD_register_template
{
  int pc;
  char instruction_bin[32];

} FD_register;

struct DE_register_template
{

  int pc;
  struct instruction_set instruction;

} DE_register;

struct EM_register_template
{

  int pc;
  int operand2;
  struct instruction_set instruction;
  int alu_result;

} EM_register;

struct MW_register_template
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
bool flag = 0;
bool isLoadStoreHazard = false;

int forwardedStalls = 0;
int instNumber;

int factor;
int block_offset, way_bits, tag_offset;

int getTag(int pc)
{
  return pc >> (tag_offset + 2);
}

int getIndex(int pc)
{
  int mask = (1 << way_bits) - 1;
  return (pc >> block_offset) & mask;
}

void update(unordered_map<int, list<block>> &cache, struct block &temp, int index, int size)
{
  switch (rep_policy)
  {
  case 1:
  {
    // LRU
    for (auto block : cache[index])
    {
      if (block.tag == temp.tag)
        temp.time = size - 1;
      else
        block.time = max(block.time - 1, 0);
    }
    break;
  }
  case 4:
  {
    // LFU
    temp.freq++;
    break;
  }
  }
}

void add(unordered_map<int, list<block>> &cache, struct block &temp, int index, int size)
{

  if (cache[index].size() < size)
  {
    temp.time = size - 1;
    temp.freq = 1;
    cache[index].push_back(temp);

    return;
  }

  switch (rep_policy)
  {
  case 1:
  {
    // LRU

    for (auto itr = cache[index].begin(); itr != cache[index].end(); itr++)
    {
      auto block = *itr;
      if (block.time == 0)
      {
        cache[index].erase(itr);
        break;
      }
    }
    add(cache, temp, index, size);

    break;
  }
  case 2:
  {
    // FIFO
    cache[index].pop_front();
    add(cache, temp, index, size);
    break;
  }
  case 3:
  {
    // Random
    int random = rand() % size;
    auto itr = cache[index].begin();
    advance(itr, random);
    cache[index].erase(itr);

    add(cache, temp, index, size);
    break;
  }
  case 4:
  {
    // LFU
    int freq = INT_MAX;
    auto mn = cache[index].begin();
    for (auto itr = cache[index].begin(); itr != cache[index].end(); itr++)
    {
      auto block = *itr;
      if (block.freq < freq)
      {
        freq = block.freq;
        mn = itr;
      }

      cache[index].erase(itr);
      break;
    }
    add(cache, temp, index, size);
    break;
  }
  }
}

void run_riscvsim()
{

  if (!KnobForPipeline)
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
    flag = 1;
    pc = -1;
    DE_reg.pop();
    fetch();
    decode();
    execute();
    mem();
    write_back();
    pc = 0;

    flag = 0;

    int x;
    if (KnobforSpecificPipelineInfo)
    {

      cout << "Enter instruction number for pipeline info: ";
      cin >> instNumber;
    }
    while (1)
    {
      cout << "Enter 1 for run, 0 for step: \n";
      cin >> x;
      fetch();
      decode();
      execute();
      mem();
      write_back();
      if (KnobForPrintingRegister)
      {
        FILE *fp;
        fp = fopen("Register_file.mc", "w");
        for (int i = 0; i < 32; i++)
        {
          fprintf(fp, "X%d : 0x%x\n", i, X[i]);
        }
        fclose(fp);
      }
      if (KnobForPrintingPipelineInfo)
      {
        FILE *fp = fopen("PipelineInfo.txt", "w");
        fprintf(fp, "Cycle %d\n", cycles);
        fprintf(fp, "\n\nFD_reg: pc=%d\n", FD_register.pc);
        fprintf(fp, "DE_reg: pc=%d, rd=%d\n", DE_register.pc, DE_register.instruction.rd);
        fprintf(fp, "EM_reg: pc=%d, rd=%d\n", EM_register.pc, EM_register.instruction.rd);
        fprintf(fp, "MW_reg: pc=%d, rd=%d\n\n\n", MW_register.pc, MW_register.instruction.rd);
        fclose(fp);
      }
      if (KnobforSpecificPipelineInfo and instNumber == pc)
      {
        FILE *fp = fopen("PipelineInfo.txt", "w");
        fprintf(fp, "Cycle %d\n", cycles);
        fprintf(fp, "\n\nFD_reg: pc=%d\n", FD_register.pc);
        fprintf(fp, "DE_reg: pc=%d, rd=%d\n", DE_register.pc, DE_register.instruction.rd);
        fprintf(fp, "EM_reg: pc=%d, rd=%d\n", EM_register.pc, EM_register.instruction.rd);
        fprintf(fp, "MW_reg: pc=%d, rd=%d\n\n\n", MW_register.pc, MW_register.instruction.rd);
        fclose(fp);
      }
      if (x == 1)
        break;
    }
    while (1)
    {
      fetch();
      decode();
      execute();
      mem();
      write_back();
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
  srand(time(0));
  cout << "Enter cache size (in KB): ";
  cin >> cache_size;
  cout << "\nEnter cache block size (in B): ";
  cin >> block_size;
  cout << "\n1. Direct mapped\n2. Fully associative\n3. Set associative\nEnter type of cache: ";
  cin >> cache_type;
  if (cache_type == 3)
  {
    cout << "Enter number of ways for Set associative cache: ";
    cin >> set_ways;
    cout << "\n";
  }
  else
  {
    set_ways = 1;
  }

  cache_sets = (cache_size * 1024) / (block_size * set_ways);

  if (cache_type == 2 || cache_type == 3)
  {
    cout << "1. LRU\n2. FIFO\n3. Random\n4. LFU\nEnter replacement policy: ";
    cin >> rep_policy;
  }
  factor = block_size / 4;
  block_offset = log2(factor);
  way_bits = log2(cache_sets);
  tag_bits = 32 - (block_offset + way_bits);
  tag_offset = 32 - tag_bits;

  if (cache_type == 2)
  {
    // FA
    tag_bits = 32 - block_offset;
    tag_offset = block_offset;
    way_bits = 0;
  }

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
      int tag = getTag(address);
      int index = getIndex(address);

      blocks[index][tag].instructions.push_back({address, instruction});
      blocks[index][tag].tag = tag;
    }
    else
    {
      int c = 5;
      int temp = address;
      while (c--)
      {
        instruction_memory[temp] = 0x00000033;
        temp += 4;
      }
      break;
    }
  }
  int data;
  while (fscanf(fp, "%x %x", &address, &data) != EOF)
  {
    MEM[address] = data;
    int tag = getTag(address);
    int index = getIndex(address);
    blocks[index][tag].instructions.push_back({address, data});
    blocks[index][tag].tag = tag;
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
  // fp = fopen("BTB.mc", "w");
  // for (auto i : BTB)
  // {
  //   fprintf(fp, "%d %d %d\n", i.first, i.second.first, i.second.second);
  // }
  // fclose(fp);
  fp = fopen("stats.txt", "w");
  cycles -= 7;
  alu_count = instructions;
  cpi = (float)(cycles) / (float)instructions;
  fprintf(fp, "Total number of cycles: %d\nTotal instructions executed: %d\nCPI: %f\nNumber of Data-transfer (load and store) instructions executed: %d\nNumber of ALU instructions executed: %d\nNumber of Control instructions executed: %d\nNumber of stalls/bubbles in the pipeline: %d\nNumber of data hazards: %d\nNumber of control hazards: %d\nNumber of branch mispredictions: %d\nNumber of stalls due to data hazards: %d\nNumber of stalls due to control hazards: %d\n", cycles, instructions, cpi, dataTransfer, alu_count, controlInstructions, totalStalls, dataHazards, controlHazards, branchMiss, stalls_data, stalls_control);
  fclose(fp);

  fp = fopen("cache_stats.txt", "w");
  int I_misses = I_cold_miss + I_conflict_miss + I_capacity_miss;
  fprintf(fp, "I$:\n\nNumber of accesses: %d\nNumber of hits: %d\nNumber of misses: %d\nNumber of cold misses: %d\nNumber of conflict misses: %d\nNumber of capacity misses: %d\nNumber of memory stalls: %d\n\n\n", I_access, I_hits, I_misses, I_cold_miss, I_conflict_miss, I_capacity_miss, I_misses);

  int D_misses = D_cold_miss + D_conflict_miss + D_capacity_miss;
  fprintf(fp, "D$:\n\nNumber of accesses: %d\nNumber of hits: %d\nNumber of misses: %d\nNumber of cold misses: %d\nNumber of conflict misses: %d\nNumber of capacity misses: %d\nNumber of memory stalls: %d\n", D_access, D_hits, D_misses, D_cold_miss, D_conflict_miss, D_capacity_miss, D_misses * 20);
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

bool isStall = false;

int instruction_stalls = 0, data_stalls = 0;

int instruction_cache_miss_stalls = 0;

// reads from the instruction memory and updates the instruction register

bool cold_miss_flag = false;
void fetch()
{

  cycles++;
  if (stalls > 0)
    return;

  nextpc = pc + 4;
  if (nextpc == 3)
    nextpc = 0;

  if (BTB.find(pc) != BTB.end() and BTB[pc].second)
  {
    nextpc = BTB[pc].first;
    cout << nextpc << endl;
  }
  if (pc != -1)
  {
    I_access++;
    int tag = getTag(pc);
    int index = getIndex(pc);

    struct block curr_block = blocks[index][tag];

    if (!MCT_I[index][tag])
    {
      I_cold_miss++;
      instruction_cache_miss_stalls = 1;
      instruction_stalls++;

      cout << "Cold miss!\nFetching data from main memory...\n";
      if (cache_type == 1)
      {
        update(instruction_cache, curr_block, index % cache_sets, 1);
        add(instruction_cache, curr_block, index % cache_sets, 1);
      }
      else if (cache_type == 2)
      {
        update(instruction_cache, curr_block, 0, cache_sets);
        add(instruction_cache, curr_block, 0, cache_sets);
      }
      else
      {

        update(instruction_cache, curr_block, index % cache_sets, set_ways);
        add(instruction_cache, curr_block, index % cache_sets, set_ways);
      }

      instruction_word = instruction_memory[pc];
    }

    else
    {
      if (cache_type == 1)
      { // Direct map
        if (instruction_cache[index % cache_sets].empty())
        {
          add(instruction_cache, curr_block, index % cache_sets, 1);
          instruction_word = instruction_memory[pc];
        }
        else
        {
          if (instruction_cache[index % cache_sets].front().tag != tag)
          {
            cout << "Conflict miss!\nFetching data from main memory...\n";
            I_conflict_miss++;
            instruction_cache[index % cache_sets].front() = curr_block;
            instruction_cache_miss_stalls = 1;
            instruction_stalls++;
            instruction_word = instruction_memory[pc];
          }
          else
          {
            cout << "Cache hit!\n";
            I_hits++;
            for (auto inst : instruction_cache[index % cache_sets].front().instructions)
            {
              if (inst.first == pc)
              {
                instruction_word = inst.second;
                break;
              }
            }
          }
        }
      }

      else if (cache_type == 2)
      { // FA
        // TODO
        bool found = false;
        update(instruction_cache, curr_block, 0, cache_sets);
        for (auto block : instruction_cache[0])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            I_hits++;
            for (auto inst : block.instructions)
            {
              cout << inst.first << ": " << inst.second << endl;
              if (inst.first == pc)
              {
                instruction_word = inst.second;
                cout << "Instruction found !\n";
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Capacity miss!\n";
          add(instruction_cache, curr_block, 0, cache_sets);
          instruction_word = instruction_memory[pc];
        }
      }

      else if (cache_type == 3)
      { // SA

        update(instruction_cache, curr_block, index % cache_sets, set_ways);
        bool found = false;
        for (auto block : instruction_cache[index % cache_sets])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            I_hits++;
            for (auto inst : block.instructions)
            {
              if (inst.first == pc)
              {
                instruction_word = inst.second;
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Conflict miss!\n";
          add(instruction_cache, curr_block, index % cache_sets, set_ways);
          instruction_word = instruction_memory[pc];
        }
      }
    }
    MCT_I[index][tag] = true;
  }
  else
  {
    instruction_word = 0x33;
  }

  set_instruction_bin(instruction_word);
  if (instruction_cache_miss_stalls > 0)
  {
    cout << "Instruction miss stalls = 1\n";
    instruction_cache_miss_stalls = 0;
  }

  // instruction_word = instruction_memory[pc];

  if (instruction_word == 0x7fffffff)
  {
    printf("Exiting...\n");
    swi_exit();
  }
  if (!instruction_memory[pc])
  {
    if (!FD_reg.empty())
    {
      return;
    }

    printf("No instruction at PC 0x%x\n", pc);
    printf("Exiting...\n");

    swi_exit();
  }

  printf("FETCH: Fetched instruction 0x%x from PC 0x%x\n", instruction_word, pc);

  if (KnobForPipeline)
  {
    FD_register.pc = pc;

    for (int i = 0; i < 32; i++)
    {
      FD_register.instruction_bin[i] = instruction.instruction_bin[i];
    }
    if (!kuchto || (FD_reg.front().pc == -1))
    {
      FD_reg.push(FD_register);
    }
    if (kuchto)
      kuchto = false;
  }
}
// reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode()
{

  if (KnobForPipeline)
  {

    FD_register = FD_reg.front();
    if (stalls == 0)
    {
      isStall = false;
    }
    if (!flag and stalls == 0)
    {
      FD_reg.pop();
    }
    for (int i = 0; i < 32; i++)
    {
      instruction.instruction_bin[i] = FD_register.instruction_bin[i];
    }
  }

  instruction.opcode = bin2dec(0, 6);
  instruction.rd = bin2dec(7, 11);
  instruction.rs1 = bin2dec(15, 19);
  instruction.rs2 = bin2dec(20, 24);
  instruction.func3 = bin2dec(12, 14);
  instruction.func7 = bin2dec(25, 31);

  if (KnobForPipeline)
  {
    if (!DE_reg.empty())
    {

      DE_register = DE_reg.back();
    }

    if (!EM_reg.empty())
    {
      EM_register = EM_reg.back();
    }
    if (!MW_reg.empty())
    {
      MW_register = MW_reg.back();
    }
    isStall = false;

    if (KnobforDataForwarding)
    {
      if (instruction.opcode == 51 and DE_register.instruction.opcode == 3 and (DE_register.instruction.rd == instruction.rs1 || DE_register.instruction.rd == instruction.rs2))
      {
        forwardedStalls++;
        stalls = 1;
      }
    }
    else
    {

      if (stalls == 0 && DE_register.instruction.rd != 0 && instruction.opcode != 55 && instruction.opcode != 23 && instruction.opcode != 103 && instruction.opcode != 111)
      {
        if (((instruction.rs1 == DE_register.instruction.rd || (instruction.rs2 == DE_register.instruction.rd && instruction.opcode != 19)) && DE_register.instruction.rd != 0 && (DE_register.instruction.opcode != 3 && DE_register.instruction.opcode != 35 && DE_register.instruction.opcode != 99)) || (DE_register.instruction.rd == instruction.rs2 && instruction.opcode == 35 && DE_register.instruction.opcode == 3))
        {
          stalls = 3;
          isStall = true;
        }
        else if (((instruction.rs1 == EM_register.instruction.rd || instruction.rs2 == EM_register.instruction.rd) && EM_register.instruction.rd != 0 && (EM_register.instruction.opcode != 3 && EM_register.instruction.opcode != 35 && EM_register.instruction.opcode != 99)) || (EM_register.instruction.rd == instruction.rs2 && instruction.opcode == 35 && EM_register.instruction.opcode == 3))
        {

          stalls = 2;
          isStall = true;
        }
        else if (((instruction.rs1 == MW_register.instruction.rd || instruction.rs2 == MW_register.instruction.rd) && MW_register.instruction.rd != 0 && (MW_register.instruction.opcode != 3 && MW_register.instruction.opcode != 35 && MW_register.instruction.opcode != 99)) || (MW_register.instruction.rd == instruction.rs2 && instruction.opcode == 35 && MW_register.instruction.opcode == 3))
        {
          stalls = 1;
          isStall = true;
        }
      }
    }
    totalStalls += stalls;
    stalls_data += stalls;
    if (stalls > 0)
      dataHazards++;
    cout << "stalls: " << stalls << endl;

    if (stalls > 0)
    {

      if (isStall and FD_reg.size() == 1)
      {
        struct FD_register_template ty = FD_reg.front();
        FD_reg.pop();
        struct FD_register_template newpush;
        for (int i = 0; i < 32; i++)
          newpush.instruction_bin[i] = instruction.instruction_bin[i];
        newpush.pc = FD_register.pc;
        FD_reg.push(newpush);
        FD_reg.push(ty);
      }
      isStall = false;

      DE_register.instruction = no_op;
      DE_register.pc = -1;
      DE_reg.push(DE_register);
      stalls--;
      if (stalls == 0)
        kuchto = true;

      return;
    }
    else
    {
      DE_register.pc = FD_register.pc;
      DE_register.instruction = instruction;
    }
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

  if (KnobForPipeline)
  {
    DE_register.instruction = instruction;
    DE_register.pc = FD_register.pc;
    DE_reg.push(DE_register);
    cout << "DE_reg back pc=" << DE_reg.back().pc << endl;
  }
}

// executes the ALU operation based on ALUop
void execute()
{

  int curr_pc = pc;
  bool taken = false;
  // instruction is the structure instance
  if (KnobForPipeline)
  {
    DE_register = DE_reg.front();
    if (!flag)
      DE_reg.pop();
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
    cout << name << " X" << instruction.rd << " X" << instruction.rs1 << " X" << instruction.rs2 << endl;
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
    cout << name << " X" << instruction.rd << " X" << instruction.rs1 << " " << instruction.immediate << endl;

    break;
  }

  case 3: // I format with register adressing(TH load instructions)  lb,lh,lw
  {
    dataTransfer++;
    name = "LOAD";
    alu_result = operand1 + instruction.immediate;
    cout << name << " X" << instruction.rd << " " << instruction.immediate << "(X" << instruction.rs1 << ")" << endl;

    break;
  }
  case 35: // store instructions sb,sh,sw
  {
    dataTransfer++;
    name = "STORE";
    alu_result = operand1 + instruction.immediate;
    cout << name << " X" << instruction.rs2 << " " << instruction.immediate << "(X" << instruction.rs1 << ")" << endl;

    break;
  }
  case 99: // branch instructions  beq,bne,bge,blt
  {

    // handling overflow conditions
    controlInstructions++;
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
        taken = true,
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 1) // bne
    {
      name = "BNE";
      if (alu_result != 0)
        taken = true,
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 4) // blt
    {
      name = "BLT";
      if ((alu_result < 0) || (overflow == true))
        taken = true,
        nextpc = curr_pc + instruction.immediate;
    }
    else if (instruction.func3 == 5) // bge
    {
      name = "BGE";
      if (alu_result >= 0 && overflow == false)
        taken = true,
        nextpc = curr_pc + instruction.immediate;
    }
    cout << name << " X" << instruction.rs1 << " X" << instruction.rs2 << "taken: " << taken << endl;
    if (BTB.find(curr_pc) != BTB.end())
    {
      cout << "BTB: " << BTB[curr_pc].second << endl;
    }
    else
    {
      cout << "BTB: " << 0 << endl;
    }

    break;
  }

  case 111: // jal
  {
    name = "JAL";
    nextpc = curr_pc + instruction.immediate;
    taken = true;

    break;
  }
  case 103: // jalr
  {
    name = "JALR";
    alu_result = operand1 + instruction.immediate;
    nextpc = alu_result;
    taken = true;
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
  cout << "taken: " << taken << endl;

  switch (instruction.opcode)
  {
  case 99:
  case 103:
  case 111:
  {
    if (BTB.find(curr_pc) == BTB.end())
      BTB[curr_pc] = {nextpc, false};
    if (BTB[curr_pc].second != taken)
    {
      if (taken)
        nextpc = BTB[curr_pc].first;
      else
        nextpc = curr_pc + 4;

      pc = nextpc;

      BTB[curr_pc].second = taken;

      cout << nextpc << endl;
      cout << "LAST" << endl;
      cout << "taken: " << taken << endl;

      // DE_register.instruction = no_op;
      // DE_register.pc = -1;
      // FD_register.pc = -1;

      // if (taken)
      //   stalls = 0;
      if (FD_reg.size() > 1 && taken)
        FD_reg.pop();
      struct DE_register_template &temp = DE_reg.front();
      temp.instruction = no_op;
      temp.pc = -1;
      struct FD_register_template &temp2 = FD_reg.front();
      temp2.pc = -1;
      for (int i = 0; i < 32; i++)
        temp2.instruction_bin[i] = no_op.instruction_bin[i];

      totalStalls += 2;
      branchMiss++;
      controlHazards++;
      stalls_control += 2;
    }
    // BTB[curr_pc] = {nextpc, taken};
  }
  }

  cout << "EXECUTE: Executed " << instruction.name << " operation\n";
  if (KnobForPipeline)
  {
    EM_register.instruction = instruction;
    EM_register.alu_result = alu_result;
    EM_register.operand2 = operand2;
    EM_register.pc = curr_pc;
    EM_reg.push(EM_register);
  }
}
// perform the memory operation
int MEM_result = 0;

int data_cache_miss_stalls = 0;

void mem()
{
  bool access = true;
  int curr_pc = pc;

  if (KnobForPipeline)
  {
    EM_register = EM_reg.front();
    if (!flag)
      EM_reg.pop();
    instruction = EM_register.instruction;
    alu_result = EM_register.alu_result;
    curr_pc = EM_register.pc;
  }

  int tag = getTag(alu_result);
  int index = getIndex(alu_result);

  struct block curr_block = blocks[index][tag];
  int cache_result;

  if (instruction.opcode == 3) // load
  {
    D_access++;

    if (!MCT_D[index][tag])
    {
      cout << "Cold miss!\nFetching data from main memory...\n";
      if (cache_type == 1)
      {
        update(data_cache, curr_block, index % cache_sets, 1);
        add(data_cache, curr_block, index % cache_sets, 1);
      }
      else if (cache_type == 2)
      {
        update(data_cache, curr_block, 0, cache_sets);
        add(data_cache, curr_block, 0, cache_sets);
      }
      else
      {

        update(data_cache, curr_block, index % cache_sets, set_ways);
        add(data_cache, curr_block, index % cache_sets, set_ways);
      }
      cache_result = MEM[alu_result];
      D_cold_miss++;
      data_cache_miss_stalls += 20;
    }
    else
    {
      if (cache_type == 1)
      { // Direct map
        if (data_cache[index % cache_sets].empty())
        {

          add(data_cache, curr_block, index % cache_sets, 1);
          cache_result = MEM[alu_result];
        }
        else
        {
          if (data_cache[index % cache_sets].front().tag != tag)
          {
            cout << "Conflict miss!\nFetching data from main memory...\n";
            D_conflict_miss++;
            data_cache[index % cache_sets].front() = curr_block;
            data_cache_miss_stalls += 20;
            cache_result = MEM[alu_result];
          }
          else
          {
            cout << "Cache hit!\n";
            D_hits++;
            for (auto data : data_cache[index % cache_sets].front().instructions)
            {
              if (data.first == pc)
              {
                cache_result = MEM[alu_result];
                break;
              }
            }
          }
        }
      }

      else if (cache_type == 2)
      { // FA
        // TODO
        bool found = false;
        update(data_cache, curr_block, 0, cache_sets);
        for (auto block : data_cache[0])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            D_hits++;

            for (auto data : block.instructions)
            {
              if (data.first == alu_result)
              {
                cache_result = data.second;
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Capacity miss!\n";
          add(data_cache, curr_block, 0, cache_sets);
          cache_result = MEM[alu_result];
        }
      }

      else if (cache_type == 3)
      { // SA

        update(data_cache, curr_block, index % cache_sets, set_ways);
        bool found = false;
        for (auto block : data_cache[index % cache_sets])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            D_hits++;

            for (auto data : block.instructions)
            {
              if (data.first == alu_result)
              {
                cache_result = data.second;
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Conflict miss!\n";
          add(data_cache, curr_block, index % cache_sets, set_ways);
          cache_result = MEM[alu_result];
        }
      }
    }

    MCT_D[index][tag] = true;
  }

  else if (instruction.opcode == 35) // store
  {
    D_access++;

    if (!MCT_D[index][tag])
    {
      cout << "Cold miss!\nPerforming Write through to main memory...\n";

      D_cold_miss++;
    }
    else
    {
      if (cache_type == 1)
      { // Direct map
        if (data_cache[index % cache_sets].empty())
        {
          // do nothing
        }
        else
        {
          if (data_cache[index % cache_sets].front().tag != tag)
          {
            cout << "Conflict miss!\nPerforming Write through to main memory...\n";
            D_conflict_miss++;
          }
          else
          {
            cout << "Cache hit!\n";
            D_hits++;

            // TODO
            // how to get the desired instruction from a block only from its pc
            for (auto data : data_cache[index % cache_sets].front().instructions)
            {
              if (data.first == alu_result)
              {
                // cache_result = MEM[alu_result];
                data.second = X[instruction.rs2];
                break;
              }
            }
          }
        }
      }

      else if (cache_type == 2)
      { // FA
        // TODO
        bool found = false;
        for (auto block : data_cache[0])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            D_hits++;

            for (auto data : block.instructions)
            {
              if (data.first == alu_result)
              {
                data.second = X[instruction.rs2];
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Capacity miss!\nPerforming Write through to main memory...\n";
        }
      }

      else if (cache_type == 3)
      { // SA

        bool found = false;
        for (auto block : data_cache[index % cache_sets])
        {
          if (block.tag == tag)
          {
            found = true;
            cout << "Cache hit!\n";
            D_hits++;

            for (auto data : block.instructions)
            {
              if (data.first == alu_result)
              {
                data.second = X[instruction.rs2];
                break;
              }
            }
            break;
          }
        }
        if (!found)
        {
          cout << "Conflict miss!\nPerforming Write through to main memory...\n";
        }
      }
    }
    MCT_D[index][tag] = true;
  }

  if (instruction.opcode == 3)
  { // mem for load instruction

    if (instruction.func3 == 0)
    { // lb
      unsigned int temp = 0;
      if (cache_result < 0)
      { // 2 compliment representation of cache_result
        temp = pow(2, 32) + cache_result;
      }
      else
      {
        temp = cache_result;
      }
      int array[32];
      for (int i = 0; i < 32; i++)
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
    { // lh
      unsigned int temp = 0;
      if (cache_result < 0)
      { // 2 compliment representation of cache_result
        temp = pow(2, 32) + cache_result;
      }
      else
      {
        temp = cache_result;
      }
      int array[32];
      for (int i = 0; i < 32; i++)
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
      // lw
      MEM_result = cache_result;
      cout << "MEM_result: " << MEM_result << endl
           << endl;
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
        temp = pow(2, 32) + X[instruction.rs2];
      }
      else
      {
        temp = X[instruction.rs2];
      }
      int array[32];
      for (int i = 0; i < 32; i++)
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
      for (int i = 0; i < 32; i++)
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
      for (int i = 0; i < 32; i++)
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
      for (int i = 0; i < 32; i++)
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
    printf("MEMORY: Memory not accessed\nPC = %d\n", curr_pc);
    access = 0;
  }

  if (access)
    printf("MEMORY: Memory accessed for instruction at PC 0x%x having value 0x%x\n", curr_pc, MEM_result);

  if (KnobForPipeline)
  {
    MW_register.instruction = instruction;
    MW_register.MEM_result = MEM_result;
    MW_register.pc = EM_register.pc;
    MW_register.alu_result = EM_register.alu_result;
    MW_reg.push(MW_register);
  }
}

// writes the results back to register file
int result;

void write_back()
{
  int curr_pc = pc;
  if (KnobForPipeline)
  {
    MW_register = MW_reg.front();
    if (!flag)
      MW_reg.pop();
    instruction = MW_register.instruction;
    MEM_result = MW_register.MEM_result;
    alu_result = MW_register.alu_result;
    curr_pc = MW_register.pc;
  }

  // printf("Instruction rd: %d\n", instruction.rd);

  bool check = 1;
  for (int i = 0; i < 32; i++)
  {
    if (instruction.instruction_bin[i] != no_op.instruction_bin[i])
      check = 0;
  }
  if (curr_pc != -1 || !check)
    instructions++;

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
    printf("WRITEBACK: Write 0x%x to X%d\nPC = %d\n", result, instruction.rd, MW_register.pc);
  }
  else
  {
    printf("No Writeback for PC = %d\n", MW_register.pc);
  }
  if (nextpc == 3)
    nextpc = 0;

  if (stalls == 0 and !kuchto)
  {
    pc = nextpc;
  }

  FD_register = FD_reg.back();
  DE_register = DE_reg.back();
  EM_register = EM_reg.back();
  MW_register = MW_reg.back();
  cout << "\n\n\n";
}