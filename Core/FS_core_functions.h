#include "FS_structures.h"

long int Get_Offset_For_Data_Block(int data_block_number);
long int Get_Offset_For_SSD_Block(int data_block_number);
long int Get_Offset_For_DB_In_CS(int data_block_number);
long int Get_Offset_For_Inode_In_IBS(int inode_number);
int Calculate_Data_Blocks_Needed_For_Size(long int bytes);
int Calculate_Pointer_Blocks_Needed_For_Data_Blocks(int blocks);
long int Get_Offset_For_Directory_Entry_Number(int entry_number);
int Blocks_Are_Equal(char new_block[BLOCK_SIZE], char existing_block[BLOCK_SIZE]);
