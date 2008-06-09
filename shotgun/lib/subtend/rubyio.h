#ifndef __RUBYIO_H
#define __RUBYIO_H

#include <stdio.h>
#include <errno.h>

typedef struct OpenFile {
    FILE *f;			/* stdio ptr for read/write */
    FILE *f2;			/* additional ptr for rw pipes */
    int mode;			/* mode flags */
    int pid;			/* child's pid (for pipes) */
    int lineno;			/* number of lines read */
    char *path;			/* pathname for file */
    void (*finalize)(struct OpenFile*,int); /* finalize proc */
} OpenFile;

#define GetReadFile(fptr) ((fptr)->f)
#define GetWriteFile(fptr) (((fptr)->f2) ? (fptr)->f2 : (fptr)->f)

#endif