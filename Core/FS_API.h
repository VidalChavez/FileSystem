#include "FS_high_level_functions.h"

char *List_Disk_Contents();
char *Report_Disk_Status();
char *Report_Cache_Status();
int Delete_File(char *filename);
int Enter_File(char *origin, char *destination);
int Extract_File(char *origin, char *destination);
int Get_Deduplication_Status();
int Set_Cache_Flag(int flag);
int Set_Deduplication_Flag(int flag);
int Exit_FS();
int Initialize_FSCS(int cache_flag, char *drive, char *cache, int deduplication_flag);
int Initialize_FS_In_Drive(char *hdd);
long unsigned Read_File_Sequentially(char *filename);
long unsigned Read_File_Block(int block_number);
