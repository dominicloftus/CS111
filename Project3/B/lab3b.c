

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* csvfd;
long filesize;
char* csv_file;

struct superblock{
  int num_blocks;
  int num_inodes;
  int block_size;
  int inode_size;
  int blocks_per_group;
  int inodes_per_group;
  int first_inode;
};

struct group{
  int group_num;
  int num_blocks;
  int num_inodes;
  int free_blocks;
  int free_inodes;
  int block_bitmap;
  int inode_bitmap;
  int first_inode_block;
};

struct inode{
  int inode_num;
  char file_type;
  int mode;
  int owner;
  int group;
  int link_count;
  char creation[32];
  char last_mod[32];
  char last_access[32];
  int file_size;
  int num_512_blocks;
  int blocks[15];
};

struct dir_entry{
  int parent_inode;
  int logical_offset;
  int inode_num;
  int entry_len;
  int name_len;
  char name[256];
};

struct indirect{
  int inode_num;
  int indirect_level;
  int logical_offset;
  int block_num;
  int ref_block_num;
};
 

  
struct superblock super;
struct group groups;
struct inode inodes[1024];
struct dir_entry dir_entries[4096];
struct indirect indirects[4096];
int free_blocks[4096];
int free_inodes[4096];
int n_groups = 0;
int n_inodes = 0;
int n_dir_entries = 0;
int n_indirects = 0;
int n_free_blocks = 0;
int n_free_inodes = 0;


int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Error: Incorrect usage.\n");
    exit(1);
  }
  if((csvfd = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Error: Bad csv file used.\n");
    exit(1);
  }
  /*
  inodes = malloc(sizeof(struct inode));
  dir_entries = malloc(sizeof(struct dir_entry));
  indirects = malloc(sizeof(struct indirect));
  free_blocks = malloc(sizeof(int));
  free_inodes = malloc(sizeof(int));
  */
  fseek(csvfd, 0, SEEK_END);
  filesize = ftell(csvfd);
  rewind(csvfd);

  csv_file = malloc(filesize);
  fread(csv_file, sizeof(char), filesize, csvfd);
  
  char field_type[16];
  field_type[0] = '\0';
  
  for(int i = 0; i < filesize; i++){
    
    if(csv_file[i] != ','){ //read what type of csv line it is
      char temp[2];
      temp[0] = csv_file[i];
      temp[1] = '\0';
      strcat(field_type, temp);
    }
    else{ //process line based on csv type read and read data into struct
      i++;
      int field_num = 1;
      char field[64];
      field[0] = '\0';
      if(strcmp(field_type, "SUPERBLOCK") == 0){
	while(1){
	  if(csv_file[i] != ',' && csv_file[i] != '\n'){
	    char temp[2] = {csv_file[i], '\0'};
	    strcat(field, temp);
	  }
	  else{
	    switch(field_num){
	    case 1:
	      super.num_blocks = atoi(field);
	      break;
	    case 2:
	      super.num_inodes = atoi(field);
	      break;
	    case 3:
              super.block_size = atoi(field);
              break;
	    case 4:
              super.inode_size = atoi(field);
              break;
	    case 5:
              super.blocks_per_group = atoi(field);
              break;
	    case 6:
              super.inodes_per_group = atoi(field);
              break;
	    case 7:
              super.first_inode = atoi(field);
              break;
	    }
	    field_num++;
	    field[0] = '\0';
	  }
	  if(csv_file[i] == '\n')
	    break;
	  i++;
	}
      }
      
      if(strcmp(field_type, "GROUP") == 0){
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
          else{ 
            switch(field_num){
	    case 1:
              groups.group_num = atoi(field);
	      break;
            case 2:
              groups.num_blocks = atoi(field);
              break;
            case 3:
              groups.num_inodes = atoi(field);
              break;
            case 4:
              groups.free_blocks = atoi(field);
              break;
            case 5:
              groups.free_inodes = atoi(field);
              break;
            case 6:
              groups.block_bitmap = atoi(field);
              break;
            case 7:
              groups.inode_bitmap = atoi(field);
              break;
	    case 8:
              groups.first_inode_block = atoi(field);
              break;
            }
            field_num++;
            field[0] = '\0';
          }
          if(csv_file[i] == '\n')
            break;
          i++;
        }
      }
      if(strcmp(field_type, "INODE") == 0){
	//inodes = realloc(inodes, (sizeof(inodes) + sizeof(struct inode)));
	n_inodes++;
	int blocks_ind = 0;
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
          else{ 
            switch(field_num){
	    case 1:
              inodes[n_inodes-1].inode_num = atoi(field);
	      break;
            case 2:
	      inodes[n_inodes-1].file_type = field[0];
              break;
            case 3:
              inodes[n_inodes-1].mode = atoi(field);
              break;
            case 4:
              inodes[n_inodes-1].owner = atoi(field);
              break;
            case 5:
              inodes[n_inodes-1].group = atoi(field);
              break;
            case 6:
              inodes[n_inodes-1].link_count = atoi(field);
              break;
            case 7:
              inodes[n_inodes-1].creation[0] = '\0';
	      strcat(inodes[n_inodes-1].creation, field);
              break;
	    case 8:
              inodes[n_inodes-1].last_mod[0] = '\0';
              strcat(inodes[n_inodes-1].last_mod, field);
              break;
	    case 9:
              inodes[n_inodes-1].last_access[0] = '\0';
              strcat(inodes[n_inodes-1].last_access, field);
              break;
	    case 10:
              inodes[n_inodes-1].file_size = atoi(field);
              break;
            
	    case 11:
              inodes[n_inodes-1].num_512_blocks = atoi(field);
              break;
            
	    default:
	      inodes[n_inodes-1].blocks[blocks_ind] = atoi(field);
	      blocks_ind++;
              break;
            
            }
            field_num++;
            field[0] = '\0';
          }
          if(csv_file[i] == '\n')
            break;
          i++;
        }
      }

      if(strcmp(field_type, "DIRENT") == 0){
	//dir_entries = realloc(dir_entries,
	//		      (sizeof(dir_entries) + sizeof(struct dir_entry)));
	n_dir_entries++;
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
          else{ 
            switch(field_num){
	    case 1:
              dir_entries[n_dir_entries-1].parent_inode = atoi(field);
	      break;
            case 2:
              dir_entries[n_dir_entries-1].logical_offset = atoi(field);
              break;
            case 3:
              dir_entries[n_dir_entries-1].inode_num = atoi(field);
              break;
            case 4:
              dir_entries[n_dir_entries-1].entry_len = atoi(field);
              break;
            case 5:
              dir_entries[n_dir_entries-1].name_len = atoi(field);
              break;
            case 6:
              dir_entries[n_dir_entries-1].name[0] = '\0';
	      strcat(dir_entries[n_dir_entries-1].name, field);
	      break;
            }
            field_num++;
            field[0] = '\0';
          }
          if(csv_file[i] == '\n')
            break;
          i++;
        }
      }

      if(strcmp(field_type, "INDIRECT") == 0){
	//indirects = realloc(indirects,
	//		      (sizeof(indirects) + sizeof(struct indirect)));
	n_indirects++;
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
          else{ 
            switch(field_num){
	    case 1:
              indirects[n_indirects-1].inode_num = atoi(field);
	      break;
            case 2:
              indirects[n_indirects-1].indirect_level = atoi(field);
              break;
            case 3:
              indirects[n_indirects-1].logical_offset = atoi(field);
              break;
            case 4:
              indirects[n_indirects-1].block_num = atoi(field);
              break;
            case 5:
              indirects[n_indirects-1].ref_block_num = atoi(field);
              break;
            }
            field_num++;
            field[0] = '\0';
          }
          if(csv_file[i] == '\n')
            break;
          i++;
        }
      }

      if(strcmp(field_type, "BFREE") == 0){
	//free_blocks = realloc(free_blocks, (sizeof(free_blocks) + sizeof(int)));
	n_free_blocks++;
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
	  else{
	    free_blocks[n_free_blocks-1] = atoi(field);
	    break;
	  }
	  i++;
	}
	field[0] = '\0';
      }

      if(strcmp(field_type, "IFREE") == 0){
        //free_inodes = realloc(free_inodes, (sizeof(free_inodes) + sizeof(int)));
        n_free_inodes++;
        while(1){
          if(csv_file[i] != ',' && csv_file[i] != '\n'){
            char temp[2] = {csv_file[i], '\0'};
            strcat(field, temp);
          }
          else{
            free_inodes[n_free_inodes-1] = atoi(field);
	    break;
          }
	  i++;
        }
        field[0] = '\0';
      }
      field_type[0] = '\0';
    }
  }

  
  struct block_ref{
    int num_refs;
    int inode_num;
    char level[64];
    int off;
  };
  
  struct block_ref* block_refs; //keep track of first block referenced for duplicates
                                //and number of times referenced
  block_refs = malloc(sizeof(struct block_ref) * (super.num_blocks + 1));
  
  for(int i = 0; i < super.num_blocks; i++){
    block_refs[i].num_refs = 0;
    block_refs[i].level[0] = '\0';
  }
  
  int first_data_block = groups.first_inode_block +
    ((super.num_inodes * super.inode_size)/super.block_size);

  if(((super.num_inodes * super.inode_size)%super.block_size) != 0)
    first_data_block++;

  
  for(int i = 0; i < n_inodes; i++){ //check inode blocks
    for(int j = 0; j < 15; j++){
      int offset = j;
      char ind_level[64];
      ind_level[0] = '\0';
      if(j == 12){
	offset = 12;
	strcat(ind_level, "INDIRECT BLOCK");
      }
      else if(j == 13){
	offset = 12 + 256;
	strcat(ind_level, "DOUBLE INDIRECT BLOCK");
      }
      else if(j == 14){
	offset = 12 + 256 + 65536;
	strcat(ind_level, "TRIPLE INDIRECT BLOCK");
      }
      else
	strcat(ind_level, "BLOCK");
	
      if(inodes[i].blocks[j] < 0 || inodes[i].blocks[j] > super.num_blocks-1){
	fprintf(stdout, "INVALID %s %d IN INODE %d AT OFFSET %d\n",
		ind_level, inodes[i].blocks[j], inodes[i].inode_num, offset);
      }
      else if(inodes[i].blocks[j] < first_data_block && inodes[i].blocks[j] != 0){
	fprintf(stdout, "RESERVED %s %d IN INODE %d AT OFFSET %d\n",
                ind_level, inodes[i].blocks[j], inodes[i].inode_num, offset);
      }
      else if(inodes[i].blocks[j] != 0){
	if(block_refs[inodes[i].blocks[j]].num_refs > 0){
	  if(block_refs[inodes[i].blocks[j]].num_refs == 1){ //get first referenced
	    fprintf(stdout, "DUPLICATE %s %d IN INODE %d AT OFFSET %d\n",
		    block_refs[inodes[i].blocks[j]].level,
		    inodes[i].blocks[j], block_refs[inodes[i].blocks[j]].inode_num,
		    block_refs[inodes[i].blocks[j]].off);
	  }
	  fprintf(stdout, "DUPLICATE %s %d IN INODE %d AT OFFSET %d\n",
		  ind_level, inodes[i].blocks[j], inodes[i].inode_num, offset);
	}
	else{
	  strcat(block_refs[inodes[i].blocks[j]].level, ind_level);
	  block_refs[inodes[i].blocks[j]].inode_num = inodes[i].inode_num;
	  block_refs[inodes[i].blocks[j]].off = offset;
	}
	block_refs[inodes[i].blocks[j]].num_refs += 1;
      }
    }
  }

  for(int i = 0; i < n_indirects; i++){ //check indirect entries
    int offset = indirects[i].logical_offset;
    char ind_level[64];
    ind_level[0] = '\0';
    if(indirects[i].indirect_level == 2){
      strcat(ind_level, "INDIRECT BLOCK");
    }
    else if(indirects[i].indirect_level == 3){
      strcat(ind_level, "DOUBLE INDIRECT BLOCK");
    }
    else
      strcat(ind_level, "BLOCK");
    
    if(indirects[i].ref_block_num < 0 ||
       indirects[i].ref_block_num > super.num_blocks-1){
      fprintf(stdout, "INVALID %s %d IN INODE %d AT OFFSET %d\n",
	      ind_level, indirects[i].ref_block_num, indirects[i].inode_num, offset);
    }
    else if(indirects[i].ref_block_num < first_data_block &&
	    indirects[i].ref_block_num != 0){
      fprintf(stdout, "RESERVED %s %d IN INODE %d AT OFFSET %d\n",
	      ind_level, indirects[i].ref_block_num, indirects[i].inode_num, offset);
    }
    else if(indirects[i].ref_block_num != 0){
      if(block_refs[indirects[i].ref_block_num].num_refs > 0){
	if(block_refs[indirects[i].ref_block_num].num_refs == 1){
	  fprintf(stdout, "DUPLICATE %s %d IN INODE %d AT OFFSET %d\n",
		  block_refs[indirects[i].ref_block_num].level,
		  indirects[i].ref_block_num,
		  block_refs[indirects[i].ref_block_num].inode_num,
		  block_refs[indirects[i].ref_block_num].off);
	}
	fprintf(stdout, "DUPLICATE %s %d IN INODE %d AT OFFSET %d\n",
		ind_level, indirects[i].ref_block_num, indirects[i].inode_num, offset);
      }
      else{
	strcat(block_refs[indirects[i].ref_block_num].level, ind_level);
	block_refs[indirects[i].ref_block_num].inode_num = indirects[i].inode_num;
	block_refs[indirects[i].ref_block_num].off = offset;
      }
      block_refs[indirects[i].ref_block_num].num_refs += 1;
    }
  }
  //check for allocated blocks in free list and add free blocks to block_refs
  //to check for unreferenced blocks
  for(int i = 0; i < n_free_blocks; i++){
    if(block_refs[free_blocks[i]].num_refs > 0){
      fprintf(stdout, "ALLOCATED BLOCK %d ON FREELIST\n", free_blocks[i]);
    }
    block_refs[free_blocks[i]].num_refs += 1;
  }

  for(int i = first_data_block; i < super.num_blocks; i++){
    if(block_refs[i].num_refs == 0){
      fprintf(stdout, "UNREFERENCED BLOCK %d\n", i);
    }
  }


  int* inode_refs;
  inode_refs = malloc(sizeof(int) * (super.num_inodes + 1));

  for(int i = 0; i < super.num_inodes + 1; i++)
    inode_refs[i] = 0;

  
  for(int i = 0; i < n_inodes; i++){
    inode_refs[inodes[i].inode_num] += 1;
  }

  //add free inodes to inode_refs to check for unallocated and not on free list next
  for(int i = 0; i < n_free_inodes; i++){
    if(inode_refs[free_inodes[i]] > 0){
      fprintf(stdout, "ALLOCATED INODE %d ON FREELIST\n", free_inodes[i]);
    }
    inode_refs[free_inodes[i]] += 1;
  }

  for(int i = 11; i < super.num_inodes; i++){
    if(inode_refs[i] == 0){
      fprintf(stdout, "UNALLOCATED INODE %d NOT ON FREELIST\n", i);
    }
  }
  //remove free inodes from inode_refs
  for(int i = 0; i < n_free_inodes; i++){
    inode_refs[free_inodes[i]] -= 1;
  }

  int* inode_links; //count link numbers to all allocated inodes
  inode_links = malloc(sizeof(int) * (super.num_inodes + 1));

  for(int i = 0; i < super.num_inodes; i++)
    inode_links[i] = 0;

  for(int i = 0; i < n_dir_entries; i++){
    if(dir_entries[i].inode_num < 1 || dir_entries[i].inode_num > super.num_inodes){
      fprintf(stdout, "DIRECTORY INODE %d NAME %s INVALID INODE %d\n",
	      dir_entries[i].parent_inode, dir_entries[i].name,
	      dir_entries[i].inode_num);
    }
    else if(inode_refs[dir_entries[i].inode_num] == 0){
      fprintf(stdout, "DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d\n",
              dir_entries[i].parent_inode, dir_entries[i].name,
              dir_entries[i].inode_num);
    }
    else if(strcmp(dir_entries[i].name, "\'..\'") == 0){ //check for bad parent dir
      if(dir_entries[i].inode_num != 2){
        fprintf(stdout, "DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n",
                dir_entries[i].parent_inode, dir_entries[i].name,
                dir_entries[i].inode_num, 2);
      }
      inode_links[dir_entries[i].inode_num] += 1;
    }
    else if(strcmp(dir_entries[i].name, "\'.\'") == 0){ //check for bad current dir
      if(dir_entries[i].parent_inode != dir_entries[i].inode_num){
	fprintf(stdout, "DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n",
		dir_entries[i].parent_inode, dir_entries[i].name,
		dir_entries[i].inode_num, dir_entries[i].parent_inode);
      }
      inode_links[dir_entries[i].inode_num] += 1;
    }
    else{
      inode_links[dir_entries[i].inode_num] += 1;
    }
  }

  for(int i = 0; i < n_inodes; i++){ //check link counts
    if(inodes[i].link_count != inode_links[inodes[i].inode_num]){
      fprintf(stdout, "INODE %d HAS %d LINKS BUT LINKCOUNT IS %d\n",
	      inodes[i].inode_num, inode_links[inodes[i].inode_num],
	      inodes[i].link_count);
    }
  }
  
  

  
  
}
