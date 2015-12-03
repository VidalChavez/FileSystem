#include "FS_API.h"
#include <Python.h>

static PyObject* List_Disk_Contents() {
  printf("Entering Function: List_Disk_Contents\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  char *result = NULL;
  char buffer[80] = "";
  Files_Stored *head = NULL;
  Files_Stored *aux;
  int size = 0;
  int i = -1;
  int blocks;
  List_All_Files(&FSCS,&head);
  aux = head;
  if (head == NULL) return NULL;
  while (aux != NULL) {
    size += strlen(aux->file_name);
    size += 9; //for both delimiter characters and size
    i++;
    aux = aux->next;
  }
  result = malloc(sizeof(char) * size);
  aux = head;
  while (i) {
    blocks = Calculate_Data_Blocks_Needed_For_Size(aux->file_size);
    blocks += Calculate_Pointer_Blocks_Needed_For_Data_Blocks(blocks);
    sprintf(buffer,"%s%s%07d%s",aux->file_name,DELIMITER_SEPARATOR_FOR_SIZE,blocks,DELIMITER_SEPARATOR_FOR_FILE);
    strcat(result,buffer);
    aux = aux->next;
    i --;
  }
  blocks = Calculate_Data_Blocks_Needed_For_Size(aux->file_size);
  blocks += Calculate_Pointer_Blocks_Needed_For_Data_Blocks(blocks);
  sprintf(buffer,"%s%s%07d",aux->file_name,DELIMITER_SEPARATOR_FOR_SIZE,blocks);
  strcat(result,buffer);
  aux = NULL;
  Free_Files_Stored_List(head);
  head = NULL;
  Write_FSCS_To_File(&FSCS);
  printf("Exiting List_Disk_Contents\n");
  return Py_BuildValue("s", result);
}

static PyObject* Report_Disk_Status() {
  printf("Entering Function: Report_Disk_Status\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  char *result = NULL;
  char *temp = NULL;
  temp = List_Disk_Contents(&FSCS);
  if (temp == NULL) return NULL;
  result = malloc(sizeof(char)*(strlen(temp)+13)); //7 for data blocks 3 for inodes 2 for delimiters 1 for '\0'
  sprintf(result,"%s%s%7d%s%3d",temp,DELIMITER_SEPARATOR_FOR_FS_VALUES,FSCS.unallocated_Data_Blocks_Left,DELIMITER_SEPARATOR_FOR_FS_VALUES,FSCS.unallocated_Inodes_Left);
  free(temp);
  temp = NULL;
  Write_FSCS_To_File(&FSCS);
  printf("Exiting Report_Disk_Status\n");
  return Py_BuildValue("s", result);
}

static PyObject* Report_Cache_Status() {
  printf("Entering Function: Report_Cache_Status\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  char *result = NULL;
  if (FSCS.cache_flag) {
    result = malloc(sizeof(char)*30);
    sprintf(result,"%06d%s%019ld",FSCS.CMS.available_cache_blocks,DELIMITER_SEPARATOR_FOR_SSD_VALUES,FSCS.CMS.total_cache_hits);
  }
  Write_FSCS_To_File(&FSCS);
  return Py_BuildValue("s", result);
}

static PyObject* Delete_File(PyObject* self, PyObject* args) {

  const char* filename;
  if (!PyArg_ParseTuple(args, "s", &filename))
  return NULL;

  printf("Entering Function: Delete_File\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  int file_inode_number = -1;
  int i = 0;
  Inode_Definition inode;
  file_inode_number = File_Exists(&FSCS, filename);
  printf("Inode: %d\n",file_inode_number);
  if (file_inode_number == -1) return -2; //file was not found
  //We get the inode
  Get_Inode(&FSCS,&inode,file_inode_number);
  //We delete all blocks
  for (i = 0; i < 12; i++) {
    if (inode.block_pointers[i] == UNUSED_POINTER_VALUE) break;
    Delete_Data_Block(&FSCS, inode.block_pointers[i]);
  }
  if (inode.block_pointers[12] != UNUSED_POINTER_VALUE) Delete_Indirect_Block(&FSCS, inode.block_pointers[12]);
  if (inode.block_pointers[13] != UNUSED_POINTER_VALUE) Delete_Double_Indirect_Block(&FSCS, inode.block_pointers[13]);
  //We mark inode as unallocated
  FSCS.inode_Bit_Map[file_inode_number] = 0;
  //We wipe out directory entry
  Delete_Directory_Entry_Number (&FSCS,file_inode_number);
  FSCS.unallocated_Inodes_Left++;
  if (FSCS.unallocated_Inodes_Left == 1) {
    FSCS.next_Free_Directory_Entry = file_inode_number;
    FSCS.next_Free_Inode_Number = file_inode_number;
  }
  //Block used for inode in IBS is not modified, we only update inode Bit Map
  Write_FSCS_To_File(&FSCS);
  printf("Leaving Delete File\n");

  return Py_BuildValue("i", file_inode_number);
}

static PyObject* Enter_File(PyObject* self, PyObject* args) {
  const char* origin;
  const char* destination;


  if (!PyArg_ParseTuple(args, "ss", &origin, &destination))
    return NULL;

  printf("Entering Function: Enter_File\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  int origin_fd;
  int size_in_blocks = 0;
  int additional_blocks = 0;
  int inode_number;
  int new_block = 0;
  int new_block_2 = 0;
  int i = 0;
  long int size_in_bytes = 0;
  long int offset = 0;
  long int offset_2 = 0;
  char new_block_read[BLOCK_SIZE];
  Inode_Definition inode;

  //verify inode availability
  printf("unallocated_Inodes_Left: %d\n",FSCS.unallocated_Inodes_Left);
  if (!FSCS.unallocated_Inodes_Left) return -1;
  //verify that file does not exist already in the FS
  if (File_Exists(&FSCS, destination) != -1) {
    //There is already a file named as specified
    // return -8;
    return Py_BuildValue("i", -8);
  }
  //open file
  if ((origin_fd = open(origin, O_RDONLY)) == -1)
    //return -2;
    return Py_BuildValue("i", -2);
  //check size of file
  size_in_bytes = lseek(origin_fd, 0, SEEK_END);
  if (size_in_bytes > MAX_FILE_SIZE)
    //return -9;
    return Py_BuildValue("i", -9);
  printf("Size in bytes: %ld\n",size_in_bytes);
  //calculate how many blocks are needed
  size_in_blocks = Calculate_Data_Blocks_Needed_For_Size(size_in_bytes);
  printf("Size in blocks: %d\n",size_in_blocks);
  //calculate additional blocks needed for direct / indirect block pointers
  additional_blocks = Calculate_Pointer_Blocks_Needed_For_Data_Blocks(size_in_blocks);
  printf("Additional Blocks: %d\n",additional_blocks);
  size_in_blocks += additional_blocks;
  //verify blocks availability
  printf("unallocated_Data_Blocks_Left: %d\n",FSCS.unallocated_Data_Blocks_Left);
  if (FSCS.unallocated_Data_Blocks_Left < size_in_blocks)
    //return -3;
    return Py_BuildValue("i", -3);
  //allocating new inode and new directory entry for new file
  inode_number = Create_New_File_With_Name(&FSCS, destination); //CDTT
  printf("Inode number allocated: %d\n",inode_number);
  if (inode_number == -1)
     //return -4;
     return Py_BuildValue("i", -4);
  //Create_New_File_With_Name() wrote Directory_Entry with name and inode
  //Create_New_File_With_Name() wrote inode with size 0 and pointers with UNUSED_POINTER_VALUE
  //We read the inode written for new file
  Get_Inode(&FSCS,&inode,inode_number);
  //Updating size
  inode.file_size = size_in_bytes;
  //We allocate blocks for file data
  size_in_blocks -= additional_blocks;
  //First, the direct block pointers (12 in total)
  i = 12;
  lseek(origin_fd, 0, SEEK_SET);

  //printf("origin_fd: %d\n",origin_fd);
  //printf("FSCS.HDD_storage_fd: %d\n",FSCS.HDD_storage_fd);
  while (i && size_in_blocks) {
    //printf("i: %d size(blocks): %d\n",i,size_in_blocks);
    read(origin_fd,new_block_read,BLOCK_SIZE);
    new_block = Assign_Block_To_Existing_Or_New_Data_Block(&FSCS, new_block_read);
    printf("1st while - new block - (assign): %d\n",new_block);
    inode.block_pointers[12-i] = new_block;
    size_in_blocks--;
    i--;
  }
  //Second, the indirect block pointer
  if (size_in_blocks) {
    new_block = Allocate_New_Data_Block(&FSCS);
    printf("New block (allocate): %d\n",new_block);
    if (new_block == -1)
    //return -5
      return Py_BuildValue("i", -5);
    Initialize_Data_Pointer_Block(&FSCS, new_block);
    inode.block_pointers[12] = new_block;
    offset = Get_Offset_For_Data_Block(new_block);
    additional_blocks--;
    i = 1024;
    while (i && size_in_blocks) {
      //printf("i: %d size: %d\n",i,size_in_blocks);
      read(origin_fd,new_block_read,BLOCK_SIZE);
      new_block = Assign_Block_To_Existing_Or_New_Data_Block(&FSCS, new_block_read);
      printf("2nd while - new block (assign): %d\n",new_block);
      Set_Fd_At_Offset(offset, &FSCS, 1);
      write(FSCS.HDD_storage_fd,&new_block,sizeof(int));
      offset = lseek(FSCS.HDD_storage_fd, 0, SEEK_CUR);
      size_in_blocks--;
      i--;
    }
  }
  //Finally, the double indirect block pointer
  if (size_in_blocks) {
    new_block = Allocate_New_Data_Block(&FSCS);
    printf("New block (allocate): %d\n",new_block);
    if (new_block == -1)
    //return -6;
        return Py_BuildValue("i", -6);
    Initialize_Data_Pointer_Block(&FSCS, new_block);
    inode.block_pointers[13] = new_block;
    offset_2 = Get_Offset_For_Data_Block(new_block);
    additional_blocks--;
    while (additional_blocks) {
      printf("3rd while - additional blocks: %d\n",additional_blocks);
      new_block = Allocate_New_Data_Block(&FSCS);
      printf("3rd while - new block (allocate): %d\n",new_block);
      if (new_block == -1)
        //return -7;
        return Py_BuildValue("i", -7);
      Initialize_Data_Pointer_Block(&FSCS, new_block);
      Set_Fd_At_Offset(offset_2, &FSCS, 1);
      write(FSCS.HDD_storage_fd,&new_block,sizeof(int));
      offset_2 = lseek(FSCS.HDD_storage_fd, 0, SEEK_CUR);
      offset = Get_Offset_For_Data_Block(new_block);
      additional_blocks--;
      i = 1024;
      while (i && size_in_blocks) {
        //printf("i: %d size: %d\n",i,size_in_blocks);
        read(origin_fd,new_block_read,BLOCK_SIZE);
        new_block = Assign_Block_To_Existing_Or_New_Data_Block(&FSCS, new_block_read);
        printf("4th while - new block (assign): %d\n",new_block);
        Set_Fd_At_Offset(offset, &FSCS, 1);
        write(FSCS.HDD_storage_fd,&new_block,sizeof(int));
        offset = lseek(FSCS.HDD_storage_fd, 0, SEEK_CUR);
        size_in_blocks--;
        i--;
      }
    }
  }
  //updating inode ind IBS
  printf("Updating Inode\n");
  offset = Get_Offset_For_Inode_In_IBS(inode_number);
  Set_Fd_At_Offset(offset, &FSCS, 1);
  write(FSCS.HDD_storage_fd,&inode,sizeof(Inode_Definition));
  Write_FSCS_To_File(&FSCS);
  printf("End of Enter File\n");
  //return 0;
  return Py_BuildValue("i", 0);
}

static PyObject* Extract_File(PyObject* self, PyObject* args){

  char *origin;
  char *destination;

  if (!PyArg_ParseTuple(args, "ss", &origin, &destination))
    return NULL;

  printf("Entering Function: Extract_File\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  int file_inode_number = -1;
  int destination_fd;
  char block[BLOCK_SIZE];
  int i = 0;
  int j = 0;
  long int offset = 0;
  long int remaining_size = 0;
  int size_to_be_written = 0;
  int block_pointers[1024];
  int block_pointers_2[1024];
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  Inode_Definition inode;
  //We get the inode number
  file_inode_number = File_Exists(&FSCS, origin);
  printf("Inode value: %d\n",file_inode_number);
  if (file_inode_number == -1)
    //return -1; //file was not found
    return Py_BuildValue("i", -1);

  //We get the inode of the file
  Get_Inode(&FSCS,&inode,file_inode_number);
  remaining_size = inode.file_size;
  //We open destination file
  printf("Size in bytes: %ld\n",inode.file_size);
  destination_fd = open(destination, O_RDWR | O_CREAT | O_TRUNC, mode);
  //We read all blocks
  lseek(destination_fd, 0, SEEK_SET);
  for (i = 0; i < 12; i++) {
    if (inode.block_pointers[i] == UNUSED_POINTER_VALUE) break;
    Read_Block(&FSCS,inode.block_pointers[i],block);
    if (remaining_size > (long int)BLOCK_SIZE) size_to_be_written = BLOCK_SIZE;
    else size_to_be_written = (int)remaining_size;
    write(destination_fd, block, size_to_be_written);
    printf("Block:%d Size:%d\n",inode.block_pointers[i],size_to_be_written);
    remaining_size -= size_to_be_written;
  }
  if (inode.block_pointers[12] != UNUSED_POINTER_VALUE) {
    //get offset for indirect block
    offset = Get_Offset_For_Data_Block(inode.block_pointers[12]);
    Set_Fd_At_Offset(offset, &FSCS, 1);
    read(FSCS.HDD_storage_fd,block_pointers,sizeof(int)*1024);
    i = 0;
    while ((block_pointers[i] != UNUSED_POINTER_VALUE) && i < 1024) {
      Read_Block(&FSCS,block_pointers[i],block);
      if (remaining_size > (long int)BLOCK_SIZE) size_to_be_written = BLOCK_SIZE;
      else size_to_be_written = (int)remaining_size;
      write(destination_fd, block, size_to_be_written);
      printf("Block:%d Size:%d\n",block_pointers[i],size_to_be_written);
      remaining_size -= size_to_be_written;
      i++;
    }
  }
  if (inode.block_pointers[13] != UNUSED_POINTER_VALUE) {
    //get offset for double indirect block
    offset = Get_Offset_For_Data_Block(inode.block_pointers[13]);
    Set_Fd_At_Offset(offset, &FSCS, 1);
    read(FSCS.HDD_storage_fd,block_pointers_2,sizeof(int)*1024);
    i = 0;
    while ((block_pointers_2[i] != UNUSED_POINTER_VALUE) && i < 1024) {
      offset = Get_Offset_For_Data_Block(block_pointers_2[i]);
      Set_Fd_At_Offset(offset, &FSCS, 1);
      read(FSCS.HDD_storage_fd,block_pointers,sizeof(int)*1024);
      j = 0;
      while ((block_pointers[j] != UNUSED_POINTER_VALUE) && j < 1024) {
        Read_Block(&FSCS,block_pointers[j],block);
        if (remaining_size > (long int)BLOCK_SIZE) size_to_be_written = BLOCK_SIZE;
        else size_to_be_written = (int)remaining_size;
        write(destination_fd, block, size_to_be_written);
        printf("Block:%d Size:%d\n",block_pointers[j],size_to_be_written);
        remaining_size -= size_to_be_written;
        j++;
      }
      i++;
    }
  }
  close(destination_fd);
  Write_FSCS_To_File(&FSCS);
  return Py_BuildValue("i", 0);
}

static PyObject* Get_Deduplication_Status(PyObject* self, PyObject* args) {
  printf("Entering Function: Get_Deduplication_Status\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  int allocated_blocks = MAX_DATA_BLOCKS - FSCS.unallocated_Data_Blocks_Left;
  int total_virtual_blocks_allocated = 0;
  long int offset = 0;
  unsigned short block_count = 0;
  //We start at block zero
  offset = Get_Offset_For_DB_In_CS(0);
  Set_Fd_At_Offset(offset, &FSCS, 1);
  while (allocated_blocks) {
    //We find the next allocated block
    do {
      read(FSCS.HDD_storage_fd,&block_count,sizeof(unsigned short));
      //printf("block count: %d\n",block_count);
    } while(block_count == 0);
    //Allocated blocks have block_count > 0
    total_virtual_blocks_allocated += block_count;
    allocated_blocks--;
  }
  allocated_blocks = MAX_DATA_BLOCKS - FSCS.unallocated_Data_Blocks_Left;
  printf("blocks allocated: %d\n",allocated_blocks);
  printf("duplicated blocks: %d\n",total_virtual_blocks_allocated - allocated_blocks);
  Write_FSCS_To_File(&FSCS);
  // return total_virtual_blocks_allocated - allocated_blocks;
  return Py_BuildValue("i", total_virtual_blocks_allocated - allocated_blocks);
}

static PyObject* Set_Cache_Flag(PyObject* self, PyObject* args) {
  int flag;
  if (!PyArg_ParseTuple(args, "i", &flag))
        return NULL;
  printf("Entering Function: Set_Cache_Flag\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  if (flag == FSCS.cache_flag) {
    printf("No changes are made\n");
    return Py_BuildValue("i", 0);
  }
  if (flag == 1) {
    //Initialize cache
    printf("We enable cache\n");
    Initialize_Cache(&FSCS);
  }
  else {
    //Unmount SSD
    printf("We turn off cache\n");
    close(FSCS.SSD_cache_fd);
  }
  printf("flag: %d\n",flag);
  FSCS.cache_flag = flag;
  Write_FSCS_To_File(&FSCS);
  return Py_BuildValue("i", flag);
}

static PyObject* Set_Deduplication_Flag(PyObject* self, PyObject* args) {
int flag;

  if (!PyArg_ParseTuple(args, "i", &flag))
      return NULL;

  printf("Entering Function: Set_Deduplication_Flag\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  printf("flag: %d\n",flag);
  FSCS.deduplication_flag = flag;
  Write_FSCS_To_File(&FSCS);
  return Py_BuildValue("i", flag);
}

static PyObject* Exit_FS() {
  printf("Entering Function: Exit_FS\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  //Write changes in the bit map and first block values
  Save_Changes(&FSCS);
  //close drive pointers HDD and SDD
  close(FSCS.HDD_storage_fd);
  if (FSCS.cache_flag) close(FSCS.SSD_cache_fd);
  Write_FSCS_To_File(&FSCS);
  return Py_BuildValue("i", flag);
}

static PyObject* Initialize_FSCS(int cache_flag, char *drive, char *cache, int deduplication_flag) {
  int cache_flag;
  char *drive;
  char *cache;
  int deduplication_flag;

   if (!PyArg_ParseTuple(args, "issi", &cache_flag, &drive, &cache, &deduplication_flag))
        return NULL;

  printf("Entering Function: Initialize_FSCS\n");
  /*
  int HDD_storage_fd;
  int SSD_cache_fd;
  unsigned char inode_Bit_Map[256];
  unsigned int next_Free_Block; //number of block
  unsigned int next_Free_Directory_Entry; //number of directory entry
  unsigned int next_Free_Inode_Number; //number of inode
  unsigned int unallocated_Inodes_Left;
  unsigned int unallocated_Data_Blocks_Left;
  char cache_flag; //indicates if cache is activated
  */
  int i = 0;
  int j = 0;
  int result = 0;
  char flag = 0;
  int offset = 0;
  int fd;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  File_System_Definitions first_block;
  FS_Control_Structure FSCS;
  memset(&first_block,0,sizeof(File_System_Definitions));

  //Initializing File Descriptors
  strncpy(FSCS.HDD_storage_path,drive,20);
  strncpy(FSCS.SSD_cache_path,cache,20);
  Mount_HDD(&FSCS);
  FSCS.cache_flag = 0;
  if (cache_flag) {
    FSCS.cache_flag = cache_flag;
    Initialize_Cache(&FSCS);
  }
  FSCS.deduplication_flag = deduplication_flag;

  //Initializing written values
  Set_Fd_At_Offset(0, &FSCS, 1);
  read(FSCS.HDD_storage_fd, &first_block, sizeof(File_System_Definitions));
  //Inode Bit Map
  flag = 0;
  for (i = 0, j = 0; i < 256 ; i++) {
    FSCS.inode_Bit_Map[i] = first_block.inode_Bit_Map[i];
    if (!first_block.inode_Bit_Map[i]) {
      j++;
      if (!flag) {
        flag = 1;
        FSCS.next_Free_Inode_Number = i;
        FSCS.next_Free_Directory_Entry = i;
      }
    }
  }

  FSCS.unallocated_Inodes_Left = first_block.free_inodes;
  if (first_block.free_inodes != j) result --;
  FSCS.unallocated_Data_Blocks_Left = first_block.free_blocks;

  //Initializing next_Free_Block
  if (FSCS.unallocated_Data_Blocks_Left) {
    FSCS.next_Free_Block = MAX_DATA_BLOCKS-1;
    if (Set_Next_Available_Block(&FSCS) != 0) {
      result --;
    }
  }

  fd = open(FSCS_FILE,O_RDWR | O_CREAT, mode);
  write(fd,&FSCS,sizeof(FS_Control_Structure));
  return Py_BuildValue("i", result);
}

int Initialize_FS_In_Drive(char *hdd) {
  printf("Entering Function: Initialize_FS_In_Drive\n");
  int drive_fp = 0;
  int offset = 0;
  int i = 0;
  char block[4096];
  File_System_Definitions first_block;
  drive_fp = open(hdd,O_RDWR);
  if (drive_fp < 0) {
    return -1; //Error code
  }

  /*
    Block size is 4096 Bytes long
    Block pointers are 4 Bytes long (int)
    Inodes are 64 Bytes long
    Directory entries are 64 Bytes long
    Block 0 is for values of the FS; it contains the structure for inode allocation
    Blocks 1 - 4 contain inode structures
    Blocks 5 - 8 contain the directory of the FS
    Blocks 9 - 2568 contain the data blocks control stucture (allocation)
    Blocks 2569 - 5245448 contain the actual data of files (20GB, 5242880 blocks)
  */

  memset(&first_block,0,sizeof(File_System_Definitions));
  strcpy (first_block.signature,SIGNATURE);
  strcpy (first_block.description, DESCRIPTION);
  first_block.total_number_of_data_blocks = 5242880;
  first_block.total_number_of_inodes = NUMBER_OF_INODES;
  first_block.free_blocks = 5242880;
  first_block.free_inodes = NUMBER_OF_INODES;
  first_block.block_size = BLOCK_SIZE; //in bytes
  first_block.inode_size = INODE_SIZE; //in bytes
  first_block.directory_size = DIRECTORY_SIZE; //in blocks
  first_block.directory_entry_size = DIRECTORY_ENTRY_SIZE; //in bytes
  first_block.first_data_block = FIRST_DATA_BLOCK; //block number
  first_block.first_inode_block = FIRST_INODE_BLOCK; //block number
  first_block.first_directory_block = FIRST_DIRECTORY_BLOCK; //block number
  write(drive_fp, &first_block, sizeof(File_System_Definitions));

  //Initializing writes of blocks with value 0 for all addresses
  memset(block,0,sizeof(char)*4096);

  //No need to initialize inode structures, Inode Control Structure in first block is sufficient
  //Initializing Directory Blocks
  offset = Get_Offset_For_Directory_Entry_Number(0);
  lseek(drive_fp, offset, SEEK_SET);

  for(i = 0; i < DIRECTORY_SIZE; i++) {
    //4 blocks
    write(drive_fp, block, sizeof(char)*4096);
  }

  //Initializing Data Blocks in Control Structure
  offset = Get_Offset_For_DB_In_CS(0);
  lseek(drive_fp, offset, SEEK_SET);

  for (i = 0; i < 2560; i++) {
    //2560 blocks
    write(drive_fp, block, sizeof(char)*4096);
  }

  close(drive_fp);
  return 0;
}

long unsigned Read_File_Sequentially(char *filename) {
  printf("Entering Function: Read_File_Sequentially\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  int file_inode_number = -1;
  int i = 0;
  int j = 0;
  int block_pointers[1024];
  int block_pointers_2[1024];
  long int offset = 0;
  clock_t t1,t2;
  char block[BLOCK_SIZE];
  Inode_Definition inode;

  file_inode_number = File_Exists(&FSCS, filename);
  //We get the inode number
  if (file_inode_number == -1) return 0; //file was not found
  //We get the inode of the file
  Get_Inode(&FSCS,&inode,file_inode_number);
  //We read all blocks
  t1 = clock();
  for (i = 0; i < 12; i++) {
    if (inode.block_pointers[i] == UNUSED_POINTER_VALUE) break;
    Read_Block(&FSCS,inode.block_pointers[i],block);
  }
  if (inode.block_pointers[12] != UNUSED_POINTER_VALUE) {
    //get offset for indirect block
    offset = Get_Offset_For_Data_Block(inode.block_pointers[12]);
    Set_Fd_At_Offset(offset, &FSCS, 1);
    read(FSCS.HDD_storage_fd,block_pointers,sizeof(int)*1024);
    i = 0;
    while ((block_pointers[i] != UNUSED_POINTER_VALUE) && i < 1024) {
      Read_Block(&FSCS,block_pointers[i],block);
      i++;
    }
  }
  if (inode.block_pointers[13] != UNUSED_POINTER_VALUE) {
    //get offset for double indirect block
    offset = Get_Offset_For_Data_Block(inode.block_pointers[13]);
    Set_Fd_At_Offset(offset, &FSCS, 1);
    read(FSCS.HDD_storage_fd,block_pointers_2,sizeof(int)*1024);
    i = 0;
    while ((block_pointers_2[i] != UNUSED_POINTER_VALUE) && i < 1024) {
      offset = Get_Offset_For_Data_Block(block_pointers_2[i]);
      Set_Fd_At_Offset(offset, &FSCS, 1);
      read(FSCS.HDD_storage_fd,block_pointers,sizeof(int)*1024);
      j = 0;
      while ((block_pointers[j] != UNUSED_POINTER_VALUE) && j < 1024) {
        Read_Block(&FSCS,block_pointers[j],block);
        j++;
      }
      i++;
    }
  }
  t2 = clock();
  Write_FSCS_To_File(&FSCS);
  return t2 - t1;
}

long unsigned Read_File_Block(int block_number) {
  printf("Entering Function: Read_File_Block\n");
  FS_Control_Structure FSCS;
  Read_FSCS_From_File(&FSCS);
  char block[BLOCK_SIZE];
  clock_t t1,t2;
  t1 = clock();
  Read_Block(&FSCS,block_number,block);
  t2 = clock();
  Write_FSCS_To_File(&FSCS);
  return t2 - t1;
}

static PyMethodDef APIMethods[] =
{
	     {"List_Disk_Contents", List_Disk_Contents, METH_VARARGS, "Greet somebody."},
	     {"Report_Disk_Status", Report_Disk_Status, METH_VARARGS, "ort_Disk_Status somebody."},
	     {"Report_Cache_Status", Report_Cache_Status, METH_VARARGS, "Greet somebody."},
	     {"Delete_File", Delete_File, METH_VARARGS, "Greet somebody."},
	     {"Extract_File", Extract_File, METH_VARARGS, "Greet somebody."},
	     {"Enter_File", Enter_File, METH_VARARGS, "Greet somebody."},
	     {"Get_Deduplication_Status", Get_Deduplication_Status, METH_VARARGS, "Greet somebody."},
	     {"Exit_FS", Exit_FS, METH_VARARGS, "Greet somebody."},
	     {"Initialize_FSCS", Initialize_FSCS, METH_VARARGS, "Greet somebody."},
	     {"Initialize_FS_In_Drive", Initialize_FS_In_Drive, METH_VARARGS, "Greet somebody."},
	     {"Read_File_Sequentially", Read_File_Sequentially, METH_VARARGS, "Greet somebody."},
	     {"Read_File_Block", Read_File_Block, METH_VARARGS, "Greet somebody."},
	     {"Set_Cache_Flag", say_hello, METH_VARARGS, "Greet somebody."},
	     {"Set_Deduplication_Flag", Set_Deduplication_Flag, METH_VARARGS, "Greet somebody."},
	     {NULL, NULL, 0, NULL}
};