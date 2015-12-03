#include "FS_high_level_functions.h"

//extern int test_fd;

int Mount_HDD(FS_Control_Structure *FSCS){
  printf("Entering Function: Mount_HDD\n");
  //open HDD
  if ((FSCS->HDD_storage_fd = open(FSCS->HDD_storage_path, O_RDWR)) == -1){
    //ERROR
    return -1;
  }
  return 0;
}

int Mount_SSD(FS_Control_Structure *FSCS){
  printf("Entering Function: Mount_SSD\n");
  //open SDD
  if ((FSCS->SSD_cache_fd = open(FSCS->SSD_cache_path, O_RDWR)) == -1){
    //ERROR
    return -1;
  }
  return 0;
}

int Save_Changes(FS_Control_Structure *FSCS){
  printf("Entering Function: Save_Changes\n");
  File_System_Definitions first_block;
  int i = 0;
  //save changes
  //update first block of HDD
  memset(&first_block,0,sizeof(File_System_Definitions));
  strcpy (first_block.signature,SIGNATURE);
  strcpy (first_block.description, DESCRIPTION);
  first_block.total_number_of_data_blocks = MAX_DATA_BLOCKS;
  first_block.total_number_of_inodes = NUMBER_OF_INODES;
  first_block.free_blocks = FSCS->unallocated_Data_Blocks_Left;
  first_block.free_inodes = FSCS->unallocated_Inodes_Left;
  first_block.block_size = BLOCK_SIZE; //in bytes
  first_block.inode_size = INODE_SIZE; //in bytes
  first_block.directory_size = DIRECTORY_SIZE; //in blocks
  first_block.directory_entry_size = DIRECTORY_ENTRY_SIZE; //in bytes
  first_block.first_data_block = FIRST_DATA_BLOCK; //block number
  first_block.first_inode_block = FIRST_INODE_BLOCK; //block number
  first_block.first_directory_block = FIRST_DIRECTORY_BLOCK; //block number
  //update inode Bit Map
  for (i = 0; i < 256; i++) {
    first_block.inode_Bit_Map[i] = FSCS->inode_Bit_Map[i];
  }
  Set_Fd_At_Offset(0, FSCS, 1);
  write(FSCS->HDD_storage_fd, &first_block, sizeof(File_System_Definitions));
  printf("Exiting Save_Changes\n");
  return 0;
}

int Set_Fd_At_Offset(long int offset, FS_Control_Structure *FSCS, int drive) {
  //printf("Entering Function: Set_Fd_At_Offset\n");
  if (drive) {
    //HDD, 1
    lseek(FSCS->HDD_storage_fd, offset, SEEK_SET);
  }
  else {
    //SSD cache, 0
    lseek(FSCS->SSD_cache_fd, offset, SEEK_SET);
  }
  return 0;
}

int Allocate_New_Data_Block(FS_Control_Structure *FSCS) {
  //printf("Entering Function: Allocate_New_Data_Block\n");
  //returns next free data block
  //block numbers go from 0 to 5,242,879
  int block_number = -1;
  if (FSCS->unallocated_Data_Blocks_Left) {
    block_number = FSCS->next_Free_Block;
    //Wipe_Out_Data_Block(FSCS,block_number);
    Update_Data_Block_Count(FSCS,block_number,'+');
    FSCS->unallocated_Data_Blocks_Left--;
    //printf("unallocated_Data_Blocks_Left: %d\n",FSCS->unallocated_Data_Blocks_Left);
    if (FSCS->unallocated_Data_Blocks_Left) Set_Next_Available_Block(FSCS);
  }
  return block_number;
}

int Assign_Block_To_Existing_Or_New_Data_Block(FS_Control_Structure *FSCS, char new_block[BLOCK_SIZE]) {
  //printf("Entering Function: Assign_Block_To_Existing_Or_New_Data_Block\n");
  //Deduplication is implemented here
  int block_number = 0;
  int allocated_blocks = MAX_DATA_BLOCKS - FSCS->unallocated_Data_Blocks_Left;
  long int offset = 0;
  unsigned short block_count = 0;
  char existing_block[BLOCK_SIZE];

  if (FSCS->deduplication_flag) {
    while (allocated_blocks) {
      //printf("Allocated blocks to check: %d\n",allocated_blocks);
      //printf("block_number: %d\n",block_number);
      //We find the next allocated block
      offset = Get_Offset_For_DB_In_CS(0); //start at zero
      Set_Fd_At_Offset(offset, FSCS, 1);
      block_number--; //necessary because of read()
      do {
        block_number++; //necessary because of read()
        read(FSCS->HDD_storage_fd,&block_count,sizeof(unsigned short));
        //printf("block_count: %d\n",block_count);
      } while(block_count == 0);
      //Allocated blocks have block_count > 0
      //printf("Reading block: %d\n",block_number);
      offset = Get_Offset_For_Data_Block(block_number);
      Set_Fd_At_Offset(offset, FSCS, 1);
      read(FSCS->HDD_storage_fd,existing_block,BLOCK_SIZE);
      //Comparing allocated with new block
      if (Blocks_Are_Equal(new_block, existing_block)) {
        //printf("Blocks were the same\n");
        //Update Block Count
        //Duplicate was found
        Update_Data_Block_Count(FSCS,block_number,'+');
        if (FSCS->cache_flag) Update_Cache_Values(FSCS, block_number);
        return block_number;
      }
      block_number++;
      allocated_blocks--;
    }
  }
  //No match was found; New Block is Allocated
  //printf("No blocks were found to match\n");
  block_number = Allocate_New_Data_Block(FSCS);
  printf("block_number (allocate): %d\n",block_number);
  offset = Get_Offset_For_Data_Block(block_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  write(FSCS->HDD_storage_fd,new_block,BLOCK_SIZE);
  //write(test_fd,new_block,BLOCK_SIZE);
  if (FSCS->cache_flag) Update_Cache_Values(FSCS, block_number);
  //printf("exiting with block_number: %d\n",block_number);
  return block_number;
}

int Set_Next_Available_Block(FS_Control_Structure *FSCS) {
  //printf("Entering Function: Set_Next_Available_Block\n");
  long int offset = 0;
  int result = 0;
  unsigned short block_count = 0;
  if (!(FSCS->unallocated_Data_Blocks_Left)) return -2;
  //We find the next unallocated block
  offset = Get_Offset_For_DB_In_CS((FSCS->next_Free_Block)+1);
  Set_Fd_At_Offset(offset, FSCS, 1);
  do {
    FSCS->next_Free_Block ++;
    if (FSCS->next_Free_Block == MAX_DATA_BLOCKS) {
      //Position at Start of Data Blocks Control Structure
      FSCS->next_Free_Block = 0;
      offset = Get_Offset_For_DB_In_CS(FSCS->next_Free_Block);
      Set_Fd_At_Offset(offset, FSCS, 1);
    }
    result = read(FSCS->HDD_storage_fd,&block_count,sizeof(unsigned short));
  } while(block_count != 0);
  //After the loop, FSCS->next_Free_Block will be equal to the next free block in the FS
  return 0;
}

int Free_block (FS_Control_Structure *FSCS, int block){
  printf("Entering Function: Free_block\n");
  int block_count = 0;

  //Decrement count and update
  block_count = Update_Data_Block_Count(FSCS, block, '-');

  if (block_count == 0) {
    //Block is marked as unallocated
    FSCS->unallocated_Data_Blocks_Left++;
    if (FSCS->unallocated_Data_Blocks_Left == 1) {
      //Block is the only unallocated block left
      FSCS->next_Free_Block = block;
    }
  }
  return block_count;
}

int Update_Data_Block_Count(FS_Control_Structure *FSCS, int block, char operation) {
  //printf("Entering Function: Update_Data_Block_Count\n");
  //updates Data Block Control Structure
  //increments or decrements block count according to operation ('+' or '-')
  unsigned short block_count = 0;
  long int offset = Get_Offset_For_DB_In_CS(block);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,&block_count,sizeof(unsigned short));
  //printf("block_count read: %d\n",block_count);
  if (operation == '+') block_count ++;
  else block_count --;
  //Updating
  //printf("block_count to write: %d\n",block_count);
  Set_Fd_At_Offset(offset, FSCS, 1);
  write(FSCS->HDD_storage_fd,&block_count,sizeof(unsigned short));
  return block_count;
}

int Delete_Directory_Entry_Number (FS_Control_Structure *FSCS, int entry_number) {
  printf("Entering Function: Delete_Directory_Entry_Number\n");
  //Wipe out directory entry with 0s
  Directory_Entry format;
  long int offset = Get_Offset_For_Directory_Entry_Number(entry_number);
  memset(&format, 0, sizeof(Directory_Entry));
  Set_Fd_At_Offset(offset, FSCS, 1);
  write(FSCS->HDD_storage_fd,&format,sizeof(Directory_Entry));
  return 0;
}

int File_Exists(FS_Control_Structure *FSCS, char *file_name) {
  printf("Entering Function: File_Exists\n");
  int total_files = NUMBER_OF_INODES - FSCS->unallocated_Inodes_Left;
  int i = 0;
  Directory_Entry entry;
  while (total_files) {
    if (FSCS->inode_Bit_Map[i]) {
      printf("Found allocated entry number %d\n",i);
      Get_Directory_Entry(FSCS, &entry, i);
      if(strcmp(file_name,entry.file_name) == 0) return entry.file_inode_number;
      total_files--;
    }
    i++;
  }
  return -1;
}

int Get_Directory_Entry(FS_Control_Structure *FSCS, Directory_Entry *result, int entry_number) {
  printf("Entering Function: Get_Directory_Entry\n");
  long int offset = Get_Offset_For_Directory_Entry_Number(entry_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,result,sizeof(Directory_Entry));
  return 0;
}

int List_All_Files(FS_Control_Structure *FSCS, Files_Stored **head) {
  printf("Entering Function: List_All_Files\n");
  //  get offset for each inode
  //  read name and size in inode
  int total_files = NUMBER_OF_INODES - FSCS->unallocated_Inodes_Left;
  int i = 0;
  Inode_Definition inode;
  Directory_Entry entry;
  Files_Stored *new_element = NULL;
  Files_Stored *aux = NULL;
  printf("Total Files in FS: %d\n",total_files);
  while (total_files) {
    if (FSCS->inode_Bit_Map[i]) {
      //printf("Found allocated entry number %d\n",i);
      Get_Directory_Entry(FSCS, &entry, i);
      Get_Inode(FSCS,&inode,i);
      if ((*head) == NULL) {
        (*head) = malloc(sizeof(Files_Stored));
        aux = *head;
      }
      else {
        new_element = malloc(sizeof(Files_Stored));
        aux->next = new_element;
        aux = aux->next;
      }
      strcpy(aux->file_name,entry.file_name);
      aux->file_inode_number = entry.file_inode_number;
      aux->file_size = inode.file_size;
      aux->next = NULL;
      total_files--;
      printf("\tName: %s\n\tInode number: %d\n\tSize: %ld\n", aux->file_name, aux->file_inode_number, aux->file_size);
    }
    i++;
  }
  return 0;
}

int Get_Inode(FS_Control_Structure *FSCS, Inode_Definition *result, int inode_number) {
  //printf("Entering Function: Get_Inode\n");
  long int offset = Get_Offset_For_Inode_In_IBS(inode_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,result,sizeof(Inode_Definition));
  return 0;
}

int Create_New_File_With_Name(FS_Control_Structure *FSCS, char *file_name) {
  //printf("Entering Function: Create_New_File_With_Name\n");
  int new_number; //for simplicity of implementation, inode and directory numbers are equal
  int i = 0;
  Directory_Entry new_directory_entry;
  Inode_Definition new_inode;
  long int offset = 0;
  if (FSCS->unallocated_Inodes_Left) {
    new_number = Allocate_New_Inode_And_Directory_Entry(FSCS);
    //printf("new inode number: %d\n",new_number);
    if (new_number == -1) {
      return -1;
    }
    //write new file in directory
    strcpy(new_directory_entry.file_name,file_name);
    new_directory_entry.file_inode_number = new_number;
    printf("file_name: %s inode: %d\n",new_directory_entry.file_name,new_directory_entry.file_inode_number);
    offset = Get_Offset_For_Directory_Entry_Number(new_number);
    Set_Fd_At_Offset(offset, FSCS, 1);
    write(FSCS->HDD_storage_fd,&new_directory_entry,sizeof(Directory_Entry));
    //initialize inode pointers with value UNUSED_POINTER_VALUE and size with 0
    new_inode.file_size = 0;
    for (i = 0; i < 14; i++) new_inode.block_pointers[i] = UNUSED_POINTER_VALUE;
    offset = Get_Offset_For_Inode_In_IBS(new_number);
    Set_Fd_At_Offset(offset, FSCS, 1);
    write(FSCS->HDD_storage_fd,&new_inode,sizeof(Inode_Definition));
    //printf("Returning: %d\n",new_number);
    return new_number;
  }
  else {
    return -1;
  }
}

int Allocate_New_Inode_And_Directory_Entry(FS_Control_Structure *FSCS) {
  printf("Entering Function: Allocate_New_Inode_And_Directory_Entry\n");
  //returns next free inode and directory entry number
  //inode and directory entry numbers go from 0 to 255
  int number = -1;
  if (FSCS->unallocated_Inodes_Left) {
    number = FSCS->next_Free_Inode_Number;
    FSCS->inode_Bit_Map[number] = 1;
    FSCS->unallocated_Inodes_Left--;
    if (FSCS->unallocated_Inodes_Left) Set_Next_Available_Inode_Number_And_Directory_Entry(FSCS);
  }
  return number;
}

int Set_Next_Available_Inode_Number_And_Directory_Entry(FS_Control_Structure *FSCS){
  printf("Entering Function: Set_Next_Available_Inode_Number_And_Directory_Entry\n");
  int i = FSCS->next_Free_Inode_Number;
  while (FSCS->inode_Bit_Map[i]) {
    i++;
    if (i == NUMBER_OF_INODES) i = 0;
  }
  FSCS->next_Free_Inode_Number = i;
  FSCS->next_Free_Directory_Entry = i;
  return i;
}

int Free_Files_Stored_List(Files_Stored *head) {
  printf("Entering Function: Free_Files_Stored_List\n");
  Files_Stored *aux = head;
  if (head == NULL) return 0;
  while (aux->next != NULL) {
    aux = aux->next;
    free(head);
    head = aux;
  }
  free(head);
  head = NULL;
  aux = NULL;
  return 0;
}

int Wipe_Out_Data_Block(FS_Control_Structure *FSCS, int block_number) {
  printf("Entering Function: Wipe_Out_Data_Block\n");
  char block[BLOCK_SIZE];
  long int offset = 0;
  int result = 0;
  memset(block,0,BLOCK_SIZE);
  offset = Get_Offset_For_Data_Block(block_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  result = write(FSCS->HDD_storage_fd,block,BLOCK_SIZE);
  return result;
}

int Delete_Data_Block(FS_Control_Structure *FSCS, int block) {
  printf("Entering Function: Delete_Data_Block\n");
  int result = 0;
  result = Free_block(FSCS, block);
  return result;
}

int Delete_Indirect_Block(FS_Control_Structure *FSCS, int block) {
  printf("Entering Function: Delete_Indirect_Block\n");
  int data_blocks[1024];
  int result = 0;
  int i = 0;
  long int offset = Get_Offset_For_Data_Block(block);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,data_blocks,sizeof(int)*1024);
  for (i = 0; i < 1024; i++) {
    if (data_blocks[i] == UNUSED_POINTER_VALUE) break;
    Delete_Data_Block(FSCS, data_blocks[i]);
  }
  Delete_Data_Block(FSCS,block);
  return 0;
}

int Delete_Double_Indirect_Block(FS_Control_Structure *FSCS, int block) {
  printf("Entering Function: Delete_Double_Indirect_Block\n");
  int indirect_blocks[1024];
  int i = 0;
  int result = 0;
  long int offset = Get_Offset_For_Data_Block(block);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,indirect_blocks,sizeof(int)*1024);
  for (i = 0; i < 1024; i++) {
    if (indirect_blocks[i] == UNUSED_POINTER_VALUE) break;
    Delete_Indirect_Block(FSCS, indirect_blocks[i]);
  }
  Delete_Data_Block(FSCS, block);
  return 0;
}

int Initialize_Data_Pointer_Block(FS_Control_Structure *FSCS, int block_number) {
  printf("Entering Function: Initialize_Data_Pointer_Block\n");
  int block[1024];
  int i = 0;
  int result = 0;
  long int offset = 0;
  for (i = 0; i < 1024; i++) block[i] = UNUSED_POINTER_VALUE;
  offset = Get_Offset_For_Data_Block(block_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  result = write(FSCS->HDD_storage_fd,block,sizeof(int)*1024);
  return result;
}

int Read_Block(FS_Control_Structure *FSCS, int block_number, char block[BLOCK_SIZE]) {
  printf("Entering Function: Read_Block\n");
  long int offset = 0;
  int cache_block_number;
  int fd;
  if (FSCS->cache_flag) {
    cache_block_number = Update_Cache_Values(FSCS, block_number);
    offset = Get_Offset_For_SSD_Block(cache_block_number);
    Set_Fd_At_Offset(offset, FSCS, 0);
    read(FSCS->SSD_cache_fd,block,BLOCK_SIZE);
  }
  else {
    //read from HDD
    offset = Get_Offset_For_Data_Block(block_number);
    Set_Fd_At_Offset(offset, FSCS, 1);
    read(FSCS->HDD_storage_fd,block,BLOCK_SIZE);
  }
  return FSCS->cache_flag;
}

int Update_Cache_Values(FS_Control_Structure *FSCS, int block_number) {
  printf("Entering Function: Update_Cache_Values\n");
  /*
  typedef struct cache_management_structure {
    unsigned char cache_hit_values[MAX_CACHE_BLOCKS];
    int hdd_block_number[MAX_CACHE_BLOCKS];
    int available_cache_blocks;
    int reference_check;
    long int total_cache_hits;
  } Cache_Management_Structure;
  */
  int i = 0;
  int j = 0;
  int flag = 0;
  int cache_block_number = 0;
  int min_value;
  int min_value_position;
  int used_blocks_left_to_check = MAX_CACHE_BLOCKS - FSCS->CMS.available_cache_blocks;
  long int offset;
  char block[BLOCK_SIZE];

  //We look for the block in cache
  for (i = 0; i < used_blocks_left_to_check; i++) {
    if (FSCS->CMS.hdd_block_number[i] == block_number) {
      //Block was found
      FSCS->CMS.cache_hit_values[i]++;
      FSCS->CMS.total_cache_hits++;
      return i;
    }
  }
  cache_block_number = i;
  flag = 1;
  //If we get out of the loop, then the block was not found
  //We now check if there are free blocks left in cache;
  if (!FSCS->CMS.available_cache_blocks) {
    flag = 0;
    //If there is no free block, we choose the block with the smallest count
    //We start search beginning from FSCS->CMS.reference_check
    j = FSCS->CMS.reference_check;
    min_value_position = j;
    min_value = FSCS->CMS.cache_hit_values[j++];
    i = MAX_CACHE_BLOCKS;
    while (i--) {
      if (j == MAX_CACHE_BLOCKS) j = 0;
      if (min_value > FSCS->CMS.cache_hit_values[j]) {
        min_value_position = j;
        min_value = FSCS->CMS.cache_hit_values[j];
      }
      j++;
    }
    cache_block_number = min_value_position;
    FSCS->CMS.reference_check++;
    if (FSCS->CMS.reference_check == MAX_CACHE_BLOCKS) FSCS->CMS.reference_check = 0;
  }
  if (flag) FSCS->CMS.available_cache_blocks --;
  FSCS->CMS.hdd_block_number[cache_block_number] = block_number;
  FSCS->CMS.cache_hit_values[cache_block_number] = 1;
  //We save the block in SSD
  //First, we read the block from HDD
  offset = Get_Offset_For_Data_Block(block_number);
  Set_Fd_At_Offset(offset, FSCS, 1);
  read(FSCS->HDD_storage_fd,block,BLOCK_SIZE);
  //We write the block to cache
  offset = Get_Offset_For_SSD_Block(cache_block_number);
  Set_Fd_At_Offset(offset, FSCS, 0);
  write(FSCS->SSD_cache_fd,block,BLOCK_SIZE);
  return cache_block_number;//cache_block_number
}

  //Al escribir tambien se debe de usar el cache, no naada mas en lectura
  //Aqui tambien hay que escribir en cache el bloque si se acaba de agregar al mismo y no se tenia antes

int Initialize_Cache(FS_Control_Structure *FSCS) {
    printf("Entering Function: Initialize_Cache\n");
    Mount_SSD(FSCS);
    memset(FSCS->CMS.cache_hit_values,0,sizeof(char)*MAX_CACHE_BLOCKS);
    memset(FSCS->CMS.hdd_block_number,-1,sizeof(int)*MAX_CACHE_BLOCKS);
    FSCS->CMS.available_cache_blocks = MAX_CACHE_BLOCKS;
    FSCS->CMS.total_cache_hits = 0;
    FSCS->CMS.reference_check = 0;
    return 0;
}

int Read_FSCS_From_File(FS_Control_Structure *FSCS) {
  int fd;
  fd = open(FSCS_FILE, O_RDWR);
  read(fd,FSCS,sizeof(FS_Control_Structure));
  close(fd);
  return 0;
}

int Write_FSCS_To_File(FS_Control_Structure *FSCS) {
  int fd;
  fd = open(FSCS_FILE, O_RDWR);
  write(fd,FSCS,sizeof(FS_Control_Structure));
  close(fd);
  return 0;
}
