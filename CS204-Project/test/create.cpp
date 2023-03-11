#include <stdio.h>
#include <unordered_map>
#include <math.h>
#include <string.h>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    FILE *fp;
    static unordered_map<unsigned int,int> instruction_memory;
    unsigned int address, instruction;
    address=0;
    //int array[4000];
    fp = fopen("R.txt", "r");
    if (fp == NULL)
    {
        printf("Error opening input R file\n");
        exit(1);
    }
    while (fscanf(fp, "%x", &instruction) != EOF)
    {
        instruction_memory[address] = instruction;
        address=address+4;
    }
    fclose(fp);
    FILE *fp1;
  
  fp1 = fopen("create1.mem", "w");
  if (fp1 == NULL)
  {
    printf("Error opening create1 file for writing\n");
    
  }

  for (int i = 0; i < address; i = i + 4)
  {
    fprintf(fp, "0x%x %08x\n", i, instruction_memory[i]);
  }
  fclose(fp);
}
