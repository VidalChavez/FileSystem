#include "FS_library_includes.h"

typedef struct file_system_definitions{
  char signature[64];
  char description[512];
  int total_number_of_data_blocks;
  int total_number_of_inodes;
  int free_blocks;
  int free_inodes;
  int block_size; //in bytes
  int inode_size; //in bytes
  int directory_size; //in blocks
  int directory_entry_size; //in bytes
  int first_data_block; //block number
  int first_inode_block; //block number
  int first_directory_block; //block number
  char inode_Bit_Map[256];
  char reserved[3220];
} File_System_Definitions;

typedef struct inode_definition {
  long int file_size;
  int block_pointers[14];
} Inode_Definition;

typedef struct directory_entry {
  char file_name[60];
  int file_inode_number;
} Directory_Entry;

typedef struct cache_management_structure {
  unsigned char cache_hit_values[MAX_CACHE_BLOCKS];
  int hdd_block_number[MAX_CACHE_BLOCKS];
  int available_cache_blocks;
  int reference_check;
  long int total_cache_hits;
} Cache_Management_Structure;

typedef struct FS_Control_Structure {
  int HDD_storage_fd;
  int SSD_cache_fd;
  char HDD_storage_path[25];
  char SSD_cache_path[25];
  char inode_Bit_Map[256];
  int next_Free_Block; //number of block
  int next_Free_Directory_Entry; //number of directory entry
  int next_Free_Inode_Number; //number of inode
  int unallocated_Inodes_Left;
  int unallocated_Data_Blocks_Left;
  char cache_flag; //indicates if cache is activated
  Cache_Management_Structure CMS;
} FS_Control_Structure;

typedef struct Files_Stored {
  struct Files_Stored *next;
  char file_name[60];
  long int file_size;
  int file_inode_number;
}Files_Stored;
