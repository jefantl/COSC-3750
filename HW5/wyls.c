/*
* wyls.c.txt
* Author: Jason Fantl
* Date: Mar 02, 2021
*
* COSC 3750, Homework 5
*
* This is a simple version of the ls utility.
*
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>


void printDir(char *directoryPath, bool nFlag, bool hFlag) {

  DIR *dir = opendir (directoryPath);
  if (dir == NULL) {
    perror(directoryPath);
    return;
  }
  
  // clear errno before reading files
  errno = 0;
  
  // loop over files
  struct dirent *dp = readdir (dir);
  for (; dp != NULL; dp = readdir (dir)) {
    // verify and load file
    char *objectName = dp->d_name;
    unsigned char objectType = dp->d_type;
    
    if (strcmp(objectName, ".") == 0 || 
        strcmp(objectName, "..") == 0) {
      continue;
    }
    
    char objectPath[_PC_PATH_MAX + 256];
    memset(objectPath, 0, _PC_PATH_MAX + 256);
    strcat(objectPath, directoryPath);
    strcat(objectPath, "/");
    strcat(objectPath, objectName);

    struct stat statbuf;
    int statFlag = 0;
    if (objectType == DT_LNK) {
      statFlag = lstat(objectPath, &statbuf);
    }
    else if (objectType == DT_DIR || objectType == DT_REG) {
      statFlag = stat(objectPath, &statbuf);
    }
    else {
      printf("%s: object type not supported\n", objectName);
    }
    if (statFlag != 0) {
      perror(objectPath);
      continue;
    }
    
    // get all the file info
    
    // permissions
    char permissionString[] = "----------";
    if (S_ISLNK(statbuf.st_mode)) {
      permissionString[0] = 'l';
    }
    else if (S_ISDIR(statbuf.st_mode)) {
      permissionString[0] = 'd';
    }
    if (statbuf.st_mode & S_IRUSR) {
      permissionString[1] = 'r';
    }
    if (statbuf.st_mode & S_IWUSR) {
      permissionString[2] = 'w';
    }
    if (statbuf.st_mode & S_IXUSR) {
      permissionString[3] = 'x';
    }
    if (statbuf.st_mode & S_IRGRP) {
      permissionString[4] = 'r';
    }
    if (statbuf.st_mode & S_IWGRP) {
      permissionString[5] = 'w';
    }
    if (statbuf.st_mode & S_IXGRP) {
      permissionString[6] = 'x';
    }
    if (statbuf.st_mode & S_IROTH) {
      permissionString[7] = 'r';
    }
    if (statbuf.st_mode & S_IWOTH) {
      permissionString[8] = 'w';
    }
    if (statbuf.st_mode & S_IXOTH) {
      permissionString[9] = 'x';
    }
    
    // owner
    char ownerString[256]; // probably long enough 
    if (nFlag) {
      sprintf(ownerString, "%d", statbuf.st_uid);
    }
    else {
      struct passwd *pStruct = getpwuid(statbuf.st_uid);
      sprintf(ownerString, "%s", pStruct->pw_name);
    }
    
    // group
    char groupString[256]; // probably long enough 
    if (!nFlag) {
      struct group *gStruct = getgrgid(statbuf.st_gid);
      sprintf(groupString, "%s", gStruct->gr_name);
    }
    else {
      sprintf(groupString, "%d", statbuf.st_gid);
    }
    
    // file size
    char sizeString[256];
    if (!hFlag || statbuf.st_size < 1<<10) {
      sprintf(sizeString, "%ld", statbuf.st_size);
    }
    else if (statbuf.st_size < 1<<20) {
      sprintf(sizeString, "%.1fK", (double)statbuf.st_size / (1<<10));
    }
    else if (statbuf.st_size < 1<<30) {
      sprintf(sizeString, "%.1fM", (double)statbuf.st_size / (1<<20));
    }
    else {
      sprintf(sizeString, "%.1fG", (double)statbuf.st_size / (1<<30));
    }
    
    // date and time
    char timeString[256] = "";
    time_t now;
    time(&now);
    double timeDiff = difftime(now, statbuf.st_mtime);
    struct tm *timeinfo = localtime(&statbuf.st_mtime);
    if (timeDiff < 180*24*60*60) {
      strftime(timeString, 256, "%b %e %H:%M", timeinfo);
    } else {
      strftime(timeString, 256, "%b %e %Y", timeinfo);
    }
    
    // print file info
    printf("%s %10s %10s %10s %15s   %-s \n", permissionString, ownerString, groupString, sizeString, timeString, objectName);
  }
  if (errno != 0) {
    perror(directoryPath);
  }
}

int main (int argc, char *argv[]) {

  // process flags
  // ---------------------
  bool hFlag = false;
  bool nFlag = false;
  
  int argIndex = 1;
  for (; argIndex < argc; ++argIndex) {
    char *flagChar = argv[argIndex];
    if (*flagChar == '-') { //this is a flag
      flagChar++;
      for (; *flagChar != '\0'; flagChar++) {
        if (*flagChar == 'n') {
          nFlag = true;
        } else if (*flagChar == 'h') {
          hFlag = true;
        } else {
          printf("Unreconized flag: -%c\n", *flagChar);
          return 0;
        }
      }
    } 
    else { // no more flags
      break;
    }
  }


  // process non-flag arguments
  // ------------------------------
  if (argIndex < argc) { // use given directory path
    printDir(argv[argIndex], nFlag, hFlag);
  } 
  else { // use current directory 
    long size = pathconf(".", _PC_PATH_MAX);
    char directoryPath[size];
    
    if (getcwd(directoryPath, size) == NULL) {
      perror("current working directory");
      return 0;
    }
    printDir(directoryPath, nFlag, hFlag);
  }
}
