#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h> // memcpy
#include "inode.h"
#include "diskimg.h"
#include "unixfilesystem.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if( !fs || !inp || inumber < 1 ){
        return -1;
    }
    
    const int INODES_PER_BLOCK = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int inode_index = inumber - 1;

    int block_num = INODE_START_SECTOR + (inode_index / INODES_PER_BLOCK);
    int offset_in_block = inode_index % INODES_PER_BLOCK;

    struct inode block[INODES_PER_BLOCK];

    if( diskimg_readsector(fs->dfd, block_num, block) == -1 ){
        return -1; // error reading the sector
    }

    memcpy( inp, &block[offset_in_block], sizeof(struct inode) );
    return 0;
}


/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if( !fs || !inp || blockNum < 0 ) return -1;

    if( (inp->i_mode & ILARG) == 0 ){
        if( blockNum >= 8 ) return -1;
        return inp->i_addr[blockNum];
    }

    // Large file: 0–1791 indirect, 1792–(1792+256*256-1) double indirect
    if( blockNum < 7 * 256 ){
        int indir_block = blockNum / 256;
        int indir_offset = blockNum % 256;

        uint16_t block[256];
        int sector = inp->i_addr[indir_block];
        if( sector == 0 ) return -1;

        if( diskimg_readsector(fs->dfd, sector, block) == -1 ) return -1;
        return block[indir_offset];
    }

    // Double indirect (last pointer, i_addr[7])
    blockNum -= 7 * 256;
    if( blockNum >= 256 * 256 ) return -1;

    int outer_index = blockNum / 256;
    int inner_index = blockNum % 256;

    uint16_t outer_block[256];
    int outer_sector = inp->i_addr[7];
    if( outer_sector == 0 || diskimg_readsector(fs->dfd, outer_sector, outer_block) == -1 ) return -1;

    int indir_sector = outer_block[outer_index];
    if( indir_sector == 0 ) return -1;

    uint16_t indir_block[256];
    if( diskimg_readsector(fs->dfd, indir_sector, indir_block) == -1 ) return -1;

    return indir_block[inner_index];
}


int inode_getsize(struct inode *inp) {
    if( !inp ) return -1;
    return ((inp->i_size0 << 16) | inp->i_size1); 
}