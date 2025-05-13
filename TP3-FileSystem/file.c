#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"
#include "unixfilesystem.h"


/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    if( !fs || !buf || inumber < 1 || blockNum < 0 ) return -1;

    struct inode inode;
    if( inode_iget(fs, inumber, &inode) < 0 ) return -1;

    int diskBlock = inode_indexlookup(fs, &inode, blockNum);
    if( diskBlock <= 0 ) return -1;

    if( diskimg_readsector(fs->dfd, diskBlock, buf) < 0 ) return -1;

    int fileSize = inode_getsize(&inode);
    if( fileSize < 0 ) return -1;

    int startByte = blockNum * DISKIMG_SECTOR_SIZE;
    if( startByte >= fileSize ) return 0;

    int bytesLeft = fileSize - startByte;
    return (bytesLeft >= DISKIMG_SECTOR_SIZE) ? DISKIMG_SECTOR_SIZE : bytesLeft;
}