
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!fs || !pathname || pathname[0] != '/') return -1;
    
    // copia mutable del path
    char pathcopy[strlen(pathname) + 1];
    strcpy(pathcopy, pathname);

    int inumber = 1; // empieza en la raíz
    char *token = strtok(pathcopy, "/");

    while( token != NULL ){
        struct direntv6 entry;
        if( directory_findname(fs, token, inumber, &entry) < 0 ){
            return -1;  // componente no encontrado
        }
        inumber = entry.d_inumber;
        token = strtok(NULL, "/");
    }
    return inumber;
}
