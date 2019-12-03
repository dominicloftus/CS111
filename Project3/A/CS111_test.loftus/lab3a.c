
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "ext2_fs.h"


int imagefd;
int block_size;
struct ext2_super_block super;
struct ext2_group_desc group;

int getOffset(int block_num){
  return block_num * block_size;
}

void superblockSum(){
  pread(imagefd, &super, sizeof(super), 1024);
  block_size = 1024 << super.s_log_block_size;
  
  fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", super.s_blocks_count,
	  super.s_inodes_count, block_size, super.s_inode_size,
	  super.s_blocks_per_group, super.s_inodes_per_group,
	  super.s_first_ino);
}

void groupSum(){
  int group_offset;
  
  if(block_size == 1024)
    group_offset = block_size * 2;
  else
    group_offset = block_size;

  pread(imagefd, &group, sizeof(group), group_offset);

  //only 1 group assumption
  fprintf(stdout, "GROUP,0,%d,%d,%d,%d,%d,%d,%d\n", super.s_blocks_count,
	  super.s_inodes_count, group.bg_free_blocks_count,
	  group.bg_free_inodes_count, group.bg_block_bitmap,
	  group.bg_inode_bitmap, group.bg_inode_table);

}

void freeBlocks(){
  int block_bit_offset = getOffset(group.bg_block_bitmap);
  char* block_bit = malloc(block_size);
  pread(imagefd, block_bit, block_size, block_bit_offset);
  int block_num = super.s_first_data_block;
  
  for(int i = 0; i < block_size; i++){
    char byte = block_bit[i];
    for(int j = 0; j < 8; j++){
      if(block_num == (int) super.s_blocks_count)
	break;
      if((byte & (1 << j)) == 0)
	fprintf(stdout, "BFREE,%d\n", block_num);
      block_num++;
    }
    if(block_num == (int) super.s_blocks_count)
      break;
  }
  free(block_bit);
}

void dirEntry(int inode_parent, int block_num, int dir_block){
  struct ext2_dir_entry entry;
  int block_offset = getOffset(block_num);
  int index = 0;

  while(index < block_size){
    pread(imagefd, &entry, sizeof(entry), block_offset + index);
    if(entry.inode != 0){
      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", inode_parent,
	      (dir_block * block_size) + index, entry.inode, 
	      entry.rec_len, entry.name_len, entry.name);
    }
    index += entry.rec_len;
  }
}

void indirect(int inode_num, int level, int logical_offset, int block_num,
	      char filetype){
  if(level == 0)
    return;

  int* block_entries = malloc(block_size);
  int block_offset = getOffset(block_num);
  pread(imagefd, block_entries, block_size, block_offset);
  
  for(int i = 0; i < (int) (block_size / sizeof(int)); i++){
    if(block_entries[i] == 0){
      if(level == 3)
	logical_offset += 65536;
      if(level == 2)
	logical_offset += 256;
      if(level == 1)
	logical_offset += 1;
      continue;
    }
    if(level == 1 && filetype == 'd')
      dirEntry(inode_num, block_entries[i], logical_offset); 

    fprintf(stdout, "INDIRECT,%d,%d,%d,%d,%d\n", inode_num, level,
	    logical_offset, block_num, block_entries[i]);
   
    indirect(inode_num, level-1, logical_offset, block_entries[i], filetype);
    if(level == 3)
      logical_offset += 65536;
    else if(level == 2)
      logical_offset += 256;
    else
      logical_offset += 1;
  }
  free(block_entries);

}

void inodeSum(int inode_num){
  struct ext2_inode inode;
  int inode_offset = getOffset(group.bg_inode_table) +
    ((inode_num - 1) * super.s_inode_size);

  pread(imagefd, &inode, super.s_inode_size, inode_offset);
  
  if(inode.i_mode == 0 || inode.i_links_count == 0)
    return;
  
  char filetype = '?';
  if(S_ISLNK(inode.i_mode))
    filetype = 's';
  else if(S_ISREG(inode.i_mode))
    filetype = 'f';
  else if(S_ISDIR(inode.i_mode))
    filetype = 'd';

  struct tm* ctime;
  struct tm* mtime;
  struct tm* atime;
  time_t ct = inode.i_ctime;
  time_t mt = inode.i_mtime;
  time_t at = inode.i_atime;
  ctime = gmtime(&ct);
  mtime = gmtime(&mt);
  atime = gmtime(&at);

  char creport[64], mreport[64], areport[64];
  
  sprintf(creport, "%02d/%02d/%02d %02d:%02d:%02d", (ctime->tm_mon)+1, ctime->tm_mday,
	  (ctime->tm_year % 100), ctime->tm_hour, ctime->tm_min, ctime->tm_sec);
  sprintf(mreport, "%02d/%02d/%02d %02d:%02d:%02d", (mtime->tm_mon)+1, mtime->tm_mday,
          (mtime->tm_year % 100), mtime->tm_hour, mtime->tm_min, mtime->tm_sec);
  sprintf(areport, "%02d/%02d/%02d %02d:%02d:%02d", (atime->tm_mon)+1, atime->tm_mday,
          (atime->tm_year % 100), atime->tm_hour, atime->tm_min, atime->tm_sec);
  
  fprintf(stdout, "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d", inode_num,
	  filetype, (inode.i_mode & 0xFFF), inode.i_uid, inode.i_gid,
	  inode.i_links_count, creport, mreport, areport, inode.i_size,
	  inode.i_blocks);
  
  if((filetype != '?' && inode.i_size > 60) || filetype == 'f' || filetype == 'd'){
    for(int i = 0; i < 15; i++){
      fprintf(stdout, ",%d", inode.i_block[i]);
    }
  }
  fprintf(stdout, "\n");

  if(filetype == 'd'){
    for(int i = 0; i < 12; i++){
      if(inode.i_block[i] != 0)
	dirEntry(inode_num, inode.i_block[i], i);
    }
  }

  if(inode.i_block[12] != 0)
    indirect(inode_num, 1, 12, inode.i_block[12], filetype);
  if(inode.i_block[13] != 0)
    indirect(inode_num, 2, 12 + 256, inode.i_block[13], filetype);
  if(inode.i_block[14] != 0)
    indirect(inode_num, 3, 12 + 256 + 65536, inode.i_block[14], filetype);

  
}

void freeInodes(){
  int inode_bit_offset = getOffset(group.bg_inode_bitmap);
  char* inode_bit = malloc(block_size);
  pread(imagefd, inode_bit, block_size, inode_bit_offset);
  int inode_num = 1;
  
  for(int i = 0; i < block_size; i++){
    char byte = inode_bit[i];
    for(int j = 0; j < 8; j++){
      if(inode_num > (int) super.s_inodes_count)
	break;
      if((byte & (1 << j)) == 0)
	fprintf(stdout, "IFREE,%d\n", inode_num);
      else
	inodeSum(inode_num);
      inode_num++;
    }
    if(inode_num > (int) super.s_inodes_count)
      break;
  }
  free(inode_bit);
}



int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Error: Incorrect usage.\n");
    exit(1);
  }
  if((imagefd = open(argv[1], O_RDONLY)) == -1){
    fprintf(stderr, "Error: Bad image used.\n");
    exit(1);
  }

  superblockSum();
  groupSum();
  freeBlocks();
  freeInodes();
  
}
