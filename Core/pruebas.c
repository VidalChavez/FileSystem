#include "FS_API.h"

#define ENTER printf("\n");
#define TEST_OUTPUT_FILE "/home/ctrejo/Desktop/test_output_file"

int test_fd;

int main(void) {
  FS_Control_Structure FSCS;
  int cache_flag = 0;
  int i = 0;
  int result;
  char *drive = "/dev/sdb";
  char *cache = "";
  char *temp;
  char *original_file = "/home/ctrejo/Desktop/archivo01";
  char *internal_file = "test01";
  char *external_file = "/home/ctrejo/Desktop/prueba01";
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  //test_fd = open(TEST_OUTPUT_FILE, O_RDWR | O_CREAT | O_TRUNC, mode);

  //printf("Esto es una prueba\n");
  //result = Initialize_FS_In_Drive(drive);
  //printf("Done with Initialize_FS_In_Drive(%s); returned %d\n",drive,result);

  Initialize_FSCS(&FSCS, cache_flag, drive, cache);
  printf("Done with Initialize_FSCS(&FSCS, %d, %s, %s); returned %d\n",cache_flag,drive,cache,result);

  /*
  typedef struct cache_management_structure {
    unsigned char cache_hit_values[MAX_CACHE_BLOCKS];
    int hdd_block_number[MAX_CACHE_BLOCKS];
    int available_cache_blocks;
    int reference_check;
    long int total_cache_hits;
  } Cache_Management_Structure;
  */

  ENTER
  for (i = 0;i<256;i++) printf("%d",FSCS.inode_Bit_Map[i]);
  ENTER
  printf("HDD_storage_fd: %d\n",FSCS.HDD_storage_fd);
  printf("SSD_cache_fd: %d\n",FSCS.SSD_cache_fd);
  printf("HDD_storage_path: %s\n",FSCS.HDD_storage_path);
  printf("SSD_cache_path: %s\n",FSCS.SSD_cache_path);
  printf("next_Free_Block: %d\n",FSCS.next_Free_Block);
  printf("next_Free_Directory_Entry: %d\n",FSCS.next_Free_Directory_Entry);
  printf("next_Free_Inode_Number: %d\n",FSCS.next_Free_Inode_Number);
  printf("unallocated_Inodes_Left: %d\n",FSCS.unallocated_Inodes_Left);
  printf("unallocated_Data_Blocks_Left: %d\n",FSCS.unallocated_Data_Blocks_Left);
  printf("cache_flag: %d\n",(int)FSCS.cache_flag);
  ENTER

  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo01","test01");
  //printf("Done with Enter_File(FSCS,%s,%s)\n",original_file,internal_file);
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo02", "test02");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo03","test03");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo04","test04");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","test05");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo06","test06");

  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado01");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado02");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado03");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado04");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado05");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado06");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado07");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado08");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado09");
  //result = Enter_File(&FSCS, "/home/ctrejo/Desktop/archivo05","duplicado10");


  //result = Extract_File(&FSCS, "test01", "/home/ctrejo/Desktop/prueba01");
  //printf("Done with Extract_File(FSCS,%s,%s)\n",internal_file,external_file);
  //result = Extract_File(&FSCS, "test02", "/home/ctrejo/Desktop/prueba02");
  //result = Extract_File(&FSCS, "test03", "/home/ctrejo/Desktop/prueba03");
  //result = Extract_File(&FSCS, "test04", "/home/ctrejo/Desktop/prueba04");
  //result = Extract_File(&FSCS, "test05", "/home/ctrejo/Desktop/prueba05");
  //result = Extract_File(&FSCS, "test06", "/home/ctrejo/Desktop/prueba06");

  //result = Extract_File(&FSCS, "duplicado02", "/home/ctrejo/Desktop/salida_duplicado02");
  //Delete_File(&FSCS, "test04");
  //Delete_File(&FSCS, "duplicado10");
  //Delete_File(&FSCS, "duplicado09");
  //Delete_File(&FSCS, "duplicado08");
  //Delete_File(&FSCS, "duplicado07");
  //Delete_File(&FSCS, "duplicado06");
  //Delete_File(&FSCS, "duplicado05");
  //Delete_File(&FSCS, "duplicado04");
  //Delete_File(&FSCS, "duplicado03");
  //Delete_File(&FSCS, "duplicado02");
  //Delete_File(&FSCS, "duplicado01");
  //result = Extract_File(&FSCS, "test05", "/home/ctrejo/Desktop/salida_test05");

  Set_Cache_Flag(&FSCS,1);
  printf("Tiempo de lectura: %lu",Read_File_Sequentially(&FSCS, "test05"));

  ENTER
  for (i = 0;i<256;i++) printf("%d",FSCS.inode_Bit_Map[i]);
  ENTER
  printf("HDD_storage_fd: %d\n",FSCS.HDD_storage_fd);
  printf("SSD_cache_fd: %d\n",FSCS.SSD_cache_fd);
  printf("HDD_storage_path: %s\n",FSCS.HDD_storage_path);
  printf("SSD_cache_path: %s\n",FSCS.SSD_cache_path);
  printf("next_Free_Block: %d\n",FSCS.next_Free_Block);
  printf("next_Free_Directory_Entry: %d\n",FSCS.next_Free_Directory_Entry);
  printf("next_Free_Inode_Number: %d\n",FSCS.next_Free_Inode_Number);
  printf("unallocated_Inodes_Left: %d\n",FSCS.unallocated_Inodes_Left);
  printf("unallocated_Data_Blocks_Left: %d\n",FSCS.unallocated_Data_Blocks_Left);
  printf("cache_flag: %d\n",(int)FSCS.cache_flag);
  ENTER

  temp = Report_Cache_Status(&FSCS);
  if (temp != NULL) {
    printf("Cache Status: %s\n",temp);
    free (temp);
    temp = NULL;
  }

  temp = Report_Disk_Status(&FSCS);
  if (temp != NULL) {
    printf("Disk Status: %s\n",temp);
    free(temp);
    temp = NULL;
  }
  printf("Deduplication status: %d\n",Get_Deduplication_Status(&FSCS));

  Exit_FS(&FSCS);
  //close(test_fd);
  printf("Done!\n");
}
