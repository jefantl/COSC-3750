/*
* wytarUtil.h
* Author: Jason Fantl
* Date: Mar 16, 2021
*
* COSC 3750, Homework 6
*
* This is the header for the creat and extract helper files.
* It also contains our header struct
*
*/

#ifndef WYTARUTIL_H
#define WYTARUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <tar.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#define BLOCK_SIZE 512

struct tar_header
{                              /* byte offset */
  char name[100];               /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */
  char gid[8];                  /* 116 */
  char size[12];                /* 124 */
  char mtime[12];               /* 136 */
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
  char pad[12];                  /* 500 */
};

void extract(char *);
void extractObject(FILE *, struct tar_header);
void extractFile(FILE *, struct tar_header);
void extractLink(FILE *, struct tar_header);
void extractDirectory(FILE *, struct tar_header);
bool createFile(char*, struct tar_header);

void save(char*, char*[], int, int);
void saveObject(FILE *, char *, char *);
void saveDirectory(FILE *, char *, char *);
void saveFile(FILE *, char *);
void saveHeader(FILE *, struct tar_header);

int octalStringToDecimal(char* );
char* getName(char*);

#endif

