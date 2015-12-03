#include "FS_core_functions.h"

long int Get_Offset_For_Data_Block(int data_block_number){
  //Data Blocks are numbered from 0 to 5,242,879 (0 - 20GB)
  //printf("Entering Function: Get_Offset_For_Data_Block\n");
  return ((long int)data_block_number + (long int)FIRST_DATA_BLOCK) * (long int)BLOCK_SIZE;
}

long int Get_Offset_For_SSD_Block(int data_block_number) {
  //SSD Blocks are numbered from 0 to 524,287 (0 - 2GB)
  //printf("Entering Function: Get_Offset_For_SSD_Block\n");
  return (long int)data_block_number * (long int)BLOCK_SIZE;
}

long int Get_Offset_For_DB_In_CS(int data_block_number) {
  //get offset for data block in block control structure
  //printf("Entering Function: Get_Offset_For_DB_In_CS\n");
  long int block_offset_in_CS = 0;
  int data_blocks_per_block = BLOCK_SIZE / sizeof(unsigned short);
  int block_modulo = 0;
  int start = FIRST_DATA_BLOCKS_CONTROL_STRUCTURE_BLOCK * BLOCK_SIZE; //in bytes
  block_modulo = data_block_number / data_blocks_per_block;
  block_offset_in_CS = (block_modulo * BLOCK_SIZE) + ((data_block_number - (block_modulo * data_blocks_per_block)) * sizeof(unsigned short)) + start;
  return block_offset_in_CS;
}

long int Get_Offset_For_Inode_In_IBS(int inode_number) {
  //get offset for inode in inode blocks structure
  //First inode is inode number 0
  //printf("Entering Function: Get_Offset_For_Inode_In_IBS\n");
  long int block_offset_in_IBS = 0;
  int start = FIRST_INODE_BLOCK * BLOCK_SIZE;
  int inodes_per_block = BLOCK_SIZE / sizeof(Inode_Definition);
  int block_modulo = inode_number / inodes_per_block;
  block_offset_in_IBS = (block_modulo * BLOCK_SIZE) + ((inode_number - (block_modulo * inodes_per_block)) * sizeof(Inode_Definition)) + start;
  return block_offset_in_IBS;
}

int Calculate_Data_Blocks_Needed_For_Size(long int bytes) {
  //calculate number of blocks needed for a file of bytes size
  //bytes are obtained by doing bytes = lseek(fd, 0, SEEK_END);
  //printf("Entering Function: Calculate_Data_Blocks_Needed_For_Size\n");
  int blocks = 0;
  blocks = (int)(bytes / (long int)BLOCK_SIZE);
  if (bytes > ((long int)blocks * (long int)BLOCK_SIZE)) {
    blocks ++;
  }
  return blocks;
}

int Calculate_Pointer_Blocks_Needed_For_Data_Blocks(int blocks) {
  //printf("Entering Function: Calculate_Pointer_Blocks_Needed_For_Data_Blocks\n");
  int pointers_per_block = 1024;
  int additional = 0;
  //First 12 blocks require no additional blocks
  if (blocks <= 12) {
    return 0;
  }
  blocks -= 12;
  //The next 1024 blocks require only one additional block for storing pointers
  if (blocks <= pointers_per_block) {
    return 1;
  }
  blocks -= pointers_per_block;
  //We will add +1 at the end for the indirect block of block pointers
  //We will add +1 at the end for the double indirect block of indirect block pointers
  //We now add 1 additional for every 1024 blocks
  additional = blocks / pointers_per_block;
  if (blocks > additional * pointers_per_block) {
    additional++;
  }
  additional ++; //for the block number 12 of the inode structure
  additional ++; //for the block number 13 of the inode structure
  return additional;
}

long int Get_Offset_For_Directory_Entry_Number(int entry_number) {
  //printf("Entering Function: Get_Offset_For_Directory_Entry_Number\n");
  //get offset for directory entry in directory blocks
  //First directory entry is entry number 0
  long int offset = 0;
  int start = FIRST_DIRECTORY_BLOCK * BLOCK_SIZE;
  int directory_entries_per_block = BLOCK_SIZE / sizeof(Directory_Entry);
  int block_modulo = entry_number / directory_entries_per_block;
  offset = (block_modulo * BLOCK_SIZE) + ((entry_number - (block_modulo * directory_entries_per_block)) * sizeof(Directory_Entry)) + start;
  return offset;
}

int Blocks_Are_Equal(char new_block[BLOCK_SIZE], char existing_block[BLOCK_SIZE]) {
  int i = 0;
  while(i < BLOCK_SIZE) {
    if (new_block[i] != existing_block[i]) return 0;
    i++;
  }
  return 1;
}
