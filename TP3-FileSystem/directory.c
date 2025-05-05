#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include "unixfilesystem.h"
#include "direntv6.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name, int dirinumber, struct direntv6 *dirEnt) {
  if( !fs || !name || !dirEnt || dirinumber < 1 ) return -1;

  char block[DISKIMG_SECTOR_SIZE];
  int blockNum = 0;

  while( 1 ){
    int bytes = file_getblock(fs, dirinumber, blockNum, block);
    if (bytes < 0) return -1; // Error al leer bloque
    if (bytes == 0) break;  // Fin del archivo

    int numEntries = bytes / sizeof(struct direntv6);
    struct direntv6 *entry = (struct direntv6 *)block;

    for( int i = 0; i < numEntries; i++ ){
      if( strncmp(entry[i].d_name, name, sizeof(entry[i].d_name)) == 0 ){
        *dirEnt = entry[i];  // Copiar la entrada encontrada
        return 0;
      }
    }

    blockNum++;
  }

  return -1;  // No se encontró el nombre
}