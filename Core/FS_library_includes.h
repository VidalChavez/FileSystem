#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>

#define BLOCK_SIZE 4096 //in bytes
#define NUMBER_OF_INODES 256 //total number of files supported
#define INODE_SIZE 64 //in bytes
#define DIRECTORY_SIZE 4 //in blocks
#define DIRECTORY_ENTRY_SIZE 64 //in bytes
#define FIRST_DATA_BLOCK 2569 //block number
#define FIRST_INODE_BLOCK 1 //block number
#define FIRST_DIRECTORY_BLOCK 5 //block number
#define FIRST_DATA_BLOCKS_CONTROL_STRUCTURE_BLOCK 9 //free space management initial block
#define MAX_DATA_BLOCKS 5242880
#define MAX_CACHE_BLOCKS 524288
#define MAX_FILE_SIZE 4299210752 //bytes
#define UNUSED_POINTER_VALUE 111111111
#define SIGNATURE "UIA Mexico CDTT MACM VCP 12/2015 OS Storage Tiering 1.0"
#define DESCRIPTION "Sistemas Operativos, Universidad Iberoamericana, Ciudad de Mexico, Fall 2015. Carlos David Trejo Tagle, Miguel Alonso Calderon Monroy, Vidal Chavez Padilla. Storage Tiering Project"
#define DELIMITER_SEPARATOR_FOR_FILE "/"
#define DELIMITER_SEPARATOR_FOR_SIZE ":"
#define DELIMITER_SEPARATOR_FOR_FS_VALUES "+"
#define DELIMITER_SEPARATOR_FOR_SSD_VALUES "+"
