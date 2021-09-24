#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ext2.h"

#define BASE_OFFSET 1024
#define EXT2_BLOCK_SIZE 1024
//#define IMAGE "/tmp/tmp.VvqCTLrmye/image.img"
// #define IMAGE "image_big.img"
typedef unsigned char bmap;
#define __NBITS (8 * (int) sizeof (bmap))
#define __BMELT(d) ((d) / __NBITS)
#define __BMMASK(d) ((bmap) 1 << ((d) % __NBITS))
#define BM_SET(d, set) ((set[__BMELT (d)] |= __BMMASK (d)))
#define BM_CLR(d, set) ((set[__BMELT (d)] &= ~__BMMASK (d)))
#define BM_ISSET(d, set) ((set[__BMELT (d)] & __BMMASK (d)) != 0)

unsigned int block_size = 0;
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*block_size)
#define MAX_FILES 100
#include <vector>
#include <algorithm>
//#include <string>


struct ext2_super_block super;
struct ext2_group_desc group;
bmap *bitmap;
bmap *inodebitmap;
int fd;

using namespace std;

typedef struct RFile{
    struct ext2_inode inode;
    int no;
    vector<int> ind_blocks;
    vector<int> data_blocks;
} RFile;


static void read_inode(int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode);




struct by_date { 
    bool operator()(RFile const &a, RFile const &b) const { 
        return a.inode.i_dtime > b.inode.i_dtime;
    }
};

struct by_inodenumber {
    bool operator()(RFile const &a, RFile const &b) const {
        return a.no < b.no;
    }
};



static void read_inode(int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
{
	lseek(fd, BLOCK_OFFSET(group->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */


static void write_inode(int inode_no, const struct ext2_group_desc *group, const struct ext2_inode *inode)
{
        lseek(fd, BLOCK_OFFSET(group->bg_inode_table)+(inode_no-1)*sizeof(struct ext2_inode), SEEK_SET);
        write(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */




void* read_block(int block_no){
    void *ind_block = malloc(block_size);
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    read(fd, ind_block, block_size);
    return ind_block;
}

void write_block(int block_no, void *block, int size){
    lseek(fd, BLOCK_OFFSET(block_no), SEEK_SET);
    write(fd, block, size);

}

int push_ind(int ind_no, vector<int>& inds, vector<int>& data_blocks){
    if(BM_ISSET(ind_no - 1, bitmap)){
        return 0;
    }
    inds.push_back(ind_no);
    void *ind_block = read_block(ind_no);
    void *temp = ind_block;
    for(int i = 0, id = *((int *)ind_block); i < block_size/sizeof(int) && id; i++, ind_block += sizeof(int), id = *((int *)ind_block)){
        if(BM_ISSET(id - 1, bitmap)){
            free(temp);
            return 0;
        }
        data_blocks.push_back(id);
    }
   free(temp);
   return 1;
}

int push_d_ind(int d_ind_no, vector<int>& ind_blocks, vector<int>& data_blocks){
    if(BM_ISSET(d_ind_no - 1, bitmap)){
        return 0;
    }
    ind_blocks.push_back(d_ind_no);
    void *d_ind_block = read_block(d_ind_no);
    void *temp = d_ind_block;
    for(int i = 0,  id = * (int *) d_ind_block; i < block_size/sizeof(int) && id; i++, d_ind_block += sizeof(int),  id = * (int *) d_ind_block){
        int res = push_ind(id, ind_blocks, data_blocks);
        if(!res){
         free(temp);
         return 0;
       }
    }
    free(temp);
    return 1;
}

int push_tri_ind(int tri_ind_no, vector<int>& ind_blocks, vector<int>& data_blocks){
    if(BM_ISSET(tri_ind_no - 1, bitmap)){
        return 0;
    }
    ind_blocks.push_back(tri_ind_no);
    void *tri_ind_block = read_block(tri_ind_no);
    void *temp = tri_ind_block;
    for(int i = 0, id = * (int *) tri_ind_block; i < block_size / sizeof(int) && id; i++, tri_ind_block += sizeof(int), id = * (int *) tri_ind_block){
        int res = push_d_ind(id, ind_blocks, data_blocks);
        if(!res){
           free(temp);
           return 0;
        }
    }
    free(temp); 
    return 1;
}




void get_blocks(const struct ext2_inode *inode, vector<int>& ind_blocks, vector<int>& data_blocks){
    for(int j = 0; j < 12 && inode->i_block[j]; j++){
        if(BM_ISSET((inode->i_block[j] - 1), bitmap)){
            data_blocks.clear();ind_blocks.clear();
            return;
        }else{
            data_blocks.push_back(inode->i_block[j]);
        }
    }

    if(inode->i_block[12]){
        int res = push_ind(inode->i_block[12], ind_blocks, data_blocks);
        if(!res){
            data_blocks.clear();ind_blocks.clear();
            return;
        }
    }
    if(inode->i_block[13]){
       int res = push_d_ind(inode->i_block[13], ind_blocks, data_blocks);
       if(!res){
         data_blocks.clear();ind_blocks.clear();
         return;
       }
    }

    if(inode->i_block[14]){
          int res = push_tri_ind(inode->i_block[14], ind_blocks, data_blocks);
       if(!res){
         data_blocks.clear();ind_blocks.clear();
         return;
       }

    }
}


void write_file(char* filename, int written);

void scan_inodes(const int fd, const struct ext2_group_desc *group, const struct ext2_super_block *super){
    vector<RFile> deleted;

int index = 0;
    for(int i = 12; i <= super->s_inodes_count; i++){
        struct ext2_inode inode;
        read_inode(i, group, &inode);
        if(inode.i_dtime > 0 && S_ISREG(inode.i_mode)){ //deleted inode IS A REGULAR FILE
            RFile rf;
            rf.inode = inode;
            rf.no = i;
            deleted.push_back(rf);
            printf("file%02d %u %d\n", index + 1, inode.i_dtime, inode.i_blocks/(2<< super->s_log_block_size));
            index++;
        }
    }


    sort(deleted.begin(), deleted.end(), by_date());

  for(int i = 0; i < deleted.size(); i++){
          get_blocks(&deleted[i].inode, deleted[i].ind_blocks, deleted[i].data_blocks);
          if(deleted[i].data_blocks.size() > 0 ||deleted[i].ind_blocks.size() > 0){

              const vector<int>& inds = deleted[i].ind_blocks;
              const vector<int>& data = deleted[i].data_blocks;
              for(int j = 0; j < inds.size(); j++){
                  BM_SET((inds[j] - 1), bitmap);
              }
              for(int j = 0; j < data.size(); j++){
                  BM_SET((data[j] - 1), bitmap);
              }

              lseek(fd, BLOCK_OFFSET(group->bg_block_bitmap), SEEK_SET);
              write(fd, bitmap, block_size);

              struct ext2_inode& in = deleted[i].inode;
              in.i_flags = 0;
              in.i_dtime = 0;
              in.i_links_count = 1;
              in.i_mode = EXT2_S_IFREG | EXT2_S_IRUSR;

              write_inode(deleted[i].no, group, &in);


              BM_SET(deleted[i].no - 1, inodebitmap);
              lseek(fd, BLOCK_OFFSET(group->bg_inode_bitmap), SEEK_SET);
              write(fd, inodebitmap, block_size);


          }
    }

    sort(deleted.begin(), deleted.end(), by_inodenumber());

    printf("###\n");
    for(int j = 0; j < deleted.size(); j++){
      //  printf("NO IS %d\n", deleted[j].no);
        if(deleted[j].ind_blocks.size() > 0 || deleted[j].data_blocks.size() > 0){
              char fname[7];
              sprintf(fname, "file%02d", j + 1);
              write_file(fname, deleted[j].no);
              printf("%s\n",fname);
        }
    }



}

// FILE NAME IS FILEXX = 7 BYTES
void write_file(char* filename, int written){
    static int total_written = 0;

    struct ext2_inode inode;
    read_inode(11, &group, &inode);

    struct ext2_dir_entry newEntry;
    newEntry.inode = written;
    newEntry.name_len = 6;
    newEntry.file_type = 1;
    memcpy(newEntry.name, filename, 6);


    if(total_written == 0){ // FIRST BLOCK FIRST
        void* block;
        block = read_block(inode.i_block[0]);

        struct ext2_dir_entry *entry;
        entry = (struct ext2_dir_entry *)( block + 12);// WE ARE ON THE '..' ENTRY
        entry->rec_len = 12; // MODIFY THE LAST ITEM'S REC_LEN 

        newEntry.rec_len = block_size - 24;

        memcpy(block + 12, entry, 12);
        memcpy(block + 24, &newEntry, 16);
        write_block(inode.i_block[0], block, block_size);
        free(block);

    }else if(total_written >= 1 && total_written <= (block_size - 24) / 16){ // 62*16 = 992 < 1000 -> FIRST BLOCK
        void* block;
        block = read_block(inode.i_block[0]);

        struct ext2_dir_entry *entry;
        entry = (struct ext2_dir_entry*) (block + 24 + (total_written - 1) * 16);
        entry->rec_len = 16;

        newEntry.rec_len = (block_size - 24) - 16*total_written;

        memcpy(block + 24 + (total_written - 1) * 16, entry, 16);
        memcpy(block + 24 + total_written * 16, &newEntry, 16); 
        write_block(inode.i_block[0], block, block_size);
        free(block);
    }else if(total_written == (block_size - 24) / 16 + 1){ // second block FIRST
        void* block;
        block = read_block(inode.i_block[1]);

        struct ext2_dir_entry *entry;

        newEntry.rec_len = block_size;

        memcpy(block, &newEntry, 16);
        write_block(inode.i_block[1], block, block_size);
        free(block);
    }else{
        void* block;
        block = read_block(inode.i_block[1]);

        struct ext2_dir_entry *entry;
        entry = (struct ext2_dir_entry*) (block  + (total_written - 64) * 16);
        entry->rec_len = 16;

        newEntry.rec_len = block_size - 16*(total_written - 63);

        memcpy(block  +  (total_written - 64) * 16, entry, 16);
        memcpy(block  +  (total_written - 63) * 16, &newEntry, 16);
        write_block(inode.i_block[1], block, block_size);
        free(block);
    }

    total_written++;
}



int main(int argc, char* argv[])
{

    if ((fd = open(argv[1], O_RDWR)) < 0) {
        perror(argv[1]);
        exit(1);
    }

    // read super-block
    lseek(fd, BASE_OFFSET, SEEK_SET);
    read(fd, &super, sizeof(super));
    if (super.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "Not a Ext2 filesystem\n");
        exit(1);
    }
    block_size = 1024 << super.s_log_block_size;



    // read group descriptor
    lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
    read(fd, &group, sizeof(group));


    
    // read block bitmap
    bitmap = (bmap*)malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_block_bitmap), SEEK_SET);
    read(fd, bitmap, block_size);

    //read inode bitmap
    inodebitmap = (bmap*) malloc(block_size);
    lseek(fd, BLOCK_OFFSET(group.bg_inode_bitmap), SEEK_SET);
    read(fd, inodebitmap, block_size);

    scan_inodes(fd, &group, &super);

    free(bitmap);
    free(inodebitmap);

    close(fd);
    return 0;
}
