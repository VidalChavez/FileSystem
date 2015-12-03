#include "FS_core_functions.h"

int Mount_HDD(FS_Control_Structure *FSCS);
int Mount_SSD(FS_Control_Structure *FSCS);
int Save_Changes(FS_Control_Structure *FSCS);
int Exit_FS(FS_Control_Structure *FSCS);
int Set_Fd_At_Offset(long int offset, FS_Control_Structure *FSCS, int drive);
int Allocate_New_Data_Block(FS_Control_Structure *FSCS);
int Assign_Block_To_Existing_Or_New_Data_Block(FS_Control_Structure *FSCS, char new_block[BLOCK_SIZE]);
int Set_Next_Available_Block(FS_Control_Structure *FSCS);
int Free_block (FS_Control_Structure *FSCS, int block);
int Update_Data_Block_Count(FS_Control_Structure *FSCS, int block, char operation);
int Delete_Directory_Entry_Number (FS_Control_Structure *FSCS, int entry_number);
int File_Exists(FS_Control_Structure *FSCS, char *file_name);
int Get_Directory_Entry(FS_Control_Structure *FSCS, Directory_Entry *result, int entry_number);
int List_All_Files(FS_Control_Structure *FSCS, Files_Stored **head);
int Get_Inode(FS_Control_Structure *FSCS, Inode_Definition *result, int inode_number);
int Create_New_File_With_Name(FS_Control_Structure *FSCS, char *file_name);
int Allocate_New_Inode_And_Directory_Entry(FS_Control_Structure *FSCS);
int Set_Next_Available_Inode_Number_And_Directory_Entry(FS_Control_Structure *FSCS);
int Free_Files_Stored_List(Files_Stored *head);
int Wipe_Out_Data_Block(FS_Control_Structure *FSCS, int block_number);
int Delete_Data_Block(FS_Control_Structure *FSCS, int block);
int Delete_Indirect_Block(FS_Control_Structure *FSCS, int block);
int Delete_Double_Indirect_Block(FS_Control_Structure *FSCS, int block);
int Initialize_Data_Pointer_Block(FS_Control_Structure *FSCS, int block_number);
int Read_Block(FS_Control_Structure *FSCS, int block_number, char block[BLOCK_SIZE]);
int Update_Cache_Values(FS_Control_Structure *FSCS, int block_number);
int Initialize_Cache(FS_Control_Structure *FSCS);
