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

#define BUFFER_SIZE 1024

int sendUserInput(FILE*);
int recieveSocket(FILE*, char*);
void closeSocket(FILE* fp);
