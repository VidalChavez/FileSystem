#include "FS_API.h"

#define ENTER printf("\n");
#define TEST_OUTPUT_FILE "/home/ctrejo/Desktop/test_output_file"

int test_fd;

int main(void) {
  FS_Control_Structure FSCS;
  int cache_flag = 0;
  int deduplication_flag = 0;
  int i = 0;
  int result;
  char *drive = "/dev/sdc";
  char *cache = "/dev/sdb";
  char *temp;
  //char *original_file = "/home/ctrejo/Desktop/archivo01";
  //char *internal_file = "test01";
  //char *external_file = "/home/ctrejo/Desktop/prueba01";
  //mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  //test_fd = open(TEST_OUTPUT_FILE, O_RDWR | O_CREAT | O_TRUNC, mode);

  //printf("Esto es una prueba\n");
  result = Initialize_FS_In_Drive(drive);
  printf("Done with Initialize_FS_In_Drive(%s); returned %d\n",drive,result);

  Initialize_FSCS(cache_flag, drive, cache, deduplication_flag);
  printf("Done with Initialize_FSCS(&FSCS, %d, %s, %s); returned %d\n",cache_flag,drive,cache,result);
  Read_FSCS_From_File(&FSCS);
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

  Set_Cache_Flag(1);


  result = Enter_File("/home/ctrejo/Desktop/archivo01","file01");
  //printf("Done with Enter_File(FSCS,%s,%s)\n",original_file,internal_file);
  result = Enter_File("/home/ctrejo/Desktop/archivo02","file02");
  result = Enter_File("/home/ctrejo/Desktop/archivo03","file03");
  result = Enter_File("/home/ctrejo/Desktop/archivo04","file04");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","file05");
  //result = Enter_File("/home/ctrejo/Desktop/archivo06","test06");

  Set_Deduplication_Flag(1);

  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado01");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado02");
  result = Enter_File("/home/ctrejo/Desktop/archivo04","duplicado03");
  result = Enter_File("/home/ctrejo/Desktop/archivo04","duplicado04");
  result = Enter_File("/home/ctrejo/Desktop/archivo03","duplicado05");
  result = Enter_File("/home/ctrejo/Desktop/archivo03","duplicado06");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado07");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado08");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado09");
  result = Enter_File("/home/ctrejo/Desktop/archivo05","duplicado10");

  Set_Deduplication_Flag(0);

  result = Extract_File("test01", "/home/ctrejo/Desktop/prueba01");
  //printf("Done with Extract_File(FSCS,%s,%s)\n",internal_file,external_file);
  result = Extract_File("test02", "/home/ctrejo/Desktop/prueba02");
  result = Extract_File("file03", "/home/ctrejo/Desktop/prueba03");
  result = Extract_File("file04", "/home/ctrejo/Desktop/prueba04");
  result = Extract_File("test05", "/home/ctrejo/Desktop/prueba05");
  result = Extract_File("test06", "/home/ctrejo/Desktop/prueba06");



  result = Enter_File("/home/ctrejo/Desktop/archivo07","pesadito");

  result = Extract_File("duplicado02", "/home/ctrejo/Desktop/salida_duplicado02");
  //Delete_File("file02");
  Delete_File("duplicado01");
  Delete_File("duplicado03");
  Delete_File("duplicado05");
  Delete_File("duplicado07");
  Delete_File("duplicado06");
  Delete_File("duplicado05");
  Delete_File("duplicado04");
  Delete_File("duplicado03");
  Delete_File("duplicado02");
  Delete_File("duplicado01");
  //result = Extract_File("test05", "/home/ctrejo/Desktop/salida_test05");


  printf("Tiempo de lectura secuencial: %lu\n",Read_File_Sequentially("file05"));
  long int time = Read_File_Block(1024);
  time += Read_File_Block(1024);
  time += Read_File_Block(1024);
  time += Read_File_Block(1024);
  time += Read_File_Block(1024);
  printf("Tiempo de lectura por bloques: %lu", time);


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
  printf("available_cache_blocks: %d\n",FSCS.CMS.available_cache_blocks);
  printf("total_cache_hits: %ld\n",FSCS.CMS.total_cache_hits);
  ENTER

  temp = Report_Cache_Status();
  if (temp != NULL) {
    printf("Cache Status: %s\n",temp);
    free (temp);
    temp = NULL;
  }

  temp = Report_Disk_Status();
  if (temp != NULL) {
    printf("Disk Status: %s\n",temp);
    free(temp);
    temp = NULL;
  }
  printf("Deduplication status: %d\n",Get_Deduplication_Status());

  Exit_FS();
  //close(test_fd);
  printf("Done!\n");
}
