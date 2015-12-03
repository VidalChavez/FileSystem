#include "FS_high_level_functions.h"

char *List_Disk_Contents(FS_Control_Structure *FSCS);
char *Report_Disk_Status(FS_Control_Structure *FSCS);
char *Report_Cache_Status(FS_Control_Structure *FSCS);
int Delete_File(FS_Control_Structure *FSCS, char *filename);
int Enter_File(FS_Control_Structure *FSCS, char *origin, char *destination);
int Extract_File(FS_Control_Structure *FSCS, char *origin, char *destination);
int Get_Deduplication_Status(FS_Control_Structure *FSCS);
int Set_Cache_Flag(FS_Control_Structure *FSCS, int flag);
int Exit_FS(FS_Control_Structure *FSCS);
int Initialize_FSCS(FS_Control_Structure *FSCS, int cache_flag, char *drive, char *cache);
int Initialize_FS_In_Drive(char *hdd);
long unsigned Read_File_Sequentially(FS_Control_Structure *FSCS, char *filename);
long unsigned Read_File_Block(FS_Control_Structure *FSCS, int block_number);
