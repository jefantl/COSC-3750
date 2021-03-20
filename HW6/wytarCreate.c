#include "wytarUtil.h"

void save(char *outFilepath, char *filesToSave[], int filesStart, int filesEnd) {
  
  FILE *archive = fopen(outFilepath, "wb");
  if (archive == NULL) {
      perror(outFilepath);
      return;
  }
  
    
  for (int fileIndex = filesStart; fileIndex < filesEnd; fileIndex++) {
    // convert to last given object
    char* filename = getName(filesToSave[fileIndex]);
    saveObject(archive, filesToSave[fileIndex], filename);
  }
  
  // adding 2 blocks of 0-filled blocks
  char buffer[2*BLOCK_SIZE];
  memset(buffer, '\0', 2*BLOCK_SIZE);
  fwrite(buffer, 1, 2*BLOCK_SIZE, archive);
  
  if (fclose(archive) != 0) {
    perror(outFilepath);
  }
}

void saveObject(FILE *archive, char *inPath, char* filename) {

  struct stat statbuf;
  if (lstat(inPath, &statbuf) == -1) {
    perror(inPath);
    return;
  }

  char objectPath[_PC_PATH_MAX + 256];
  char objectName[_PC_PATH_MAX + 256];
  memset(objectPath, 0, _PC_PATH_MAX + 256);
  memset(objectName, 0, _PC_PATH_MAX + 256);
  if (S_ISDIR(statbuf.st_mode)) {
    strcat(objectPath, inPath);
    strcat(objectPath, "/");
    
    strcat(objectName, filename);
    strcat(objectName, "/");
  } else {
    strncpy(objectPath, inPath, strlen(inPath));
    strncpy(objectName, filename, strlen(filename));
  }

  struct tar_header header;
  memset(&header, '\0', BLOCK_SIZE);
  
  if (strlen(objectName) <= 100) {
    strncpy(header.name, objectName, 100);
  }
  else if (strlen(objectName) <= 255) {
    int prefixSize = strlen(objectName)-100;
    char* pathPtr = objectName;
    strncpy(header.prefix, pathPtr, prefixSize);
    pathPtr += prefixSize;
    strncpy(header.name, pathPtr, 100);
  }
  else {
    return;
  }
  
  sprintf(header.mode, "%07o", statbuf.st_mode);
  
  sprintf(header.uid, "%07o", statbuf.st_uid);
  sprintf(header.gid, "%07o", statbuf.st_gid);

  sprintf(header.mtime, "%011lo", statbuf.st_mtime);

  if (S_ISLNK(statbuf.st_mode)) {
    header.typeflag = SYMTYPE;
    sprintf(header.size, "%011o", 0);
    
    char linkString[256];
    int bytesCopied = readlink(objectPath, linkString, 256);
    if (bytesCopied == -1) {
      perror(objectPath); 
      return;
    } 
    if ( strlen(linkString) > 99) {
      return;
    }
    linkString[bytesCopied] = '\0';
    strncpy(header.linkname, linkString, 100);
  }
  else if (S_ISDIR(statbuf.st_mode)) {
    header.typeflag = DIRTYPE;
    memset(header.linkname, '\0', 100);
    sprintf(header.size, "%011o", 0);
  }
  else if (S_ISREG(statbuf.st_mode)) {
    header.typeflag = REGTYPE;
    memset(header.linkname, '\0', 100);
    sprintf(header.size, "%011lo", statbuf.st_size);
  }

  strncpy(header.magic, TMAGIC, 6);
  strncpy(header.version, TVERSION, 2);
  
  struct passwd *pStruct = getpwuid(statbuf.st_uid);
  strncpy(header.uname, pStruct->pw_name, 32);
  struct group *gStruct = getgrgid(statbuf.st_gid);
  strncpy(header.gname, gStruct->gr_name, 32);
  
  sprintf(header.devmajor, "0000000");
  sprintf(header.devminor, "0000000");

  unsigned int chksum = 0;
  memset(header.chksum, ' ', 8);
  char *p = (char*)&header;
  for (int i = 0; i < BLOCK_SIZE; i++) {
    chksum += *p;
    p++;
  }
  sprintf(header.chksum, "%07o", chksum);
    
  saveHeader(archive, header);
  
  if (S_ISDIR(statbuf.st_mode)) {
    saveDirectory(archive, objectPath, objectName);
  }
  else if (S_ISREG(statbuf.st_mode)) {
    saveFile(archive, objectPath);
  }
}

void saveDirectory(FILE *archive, char *directoryPath, char *directoryName) {

  // then recursivley save nested objects
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
    char *readName = dp->d_name;

    if (strcmp(readName, ".") == 0 || 
        strcmp(readName, "..") == 0) {
      continue;
    }
    
    char objectPath[_PC_PATH_MAX + 256];
    char objectName[_PC_PATH_MAX + 256];
    memset(objectPath, 0, _PC_PATH_MAX + 256);
    memset(objectName, 0, _PC_PATH_MAX + 256);
    strcat(objectPath, directoryPath);
    strcat(objectPath, readName);
    strcat(objectName, directoryName);
    strcat(objectName, readName);
    
    saveObject(archive, objectPath, objectName);
  }
}

void saveFile(FILE *archive, char *filepath) {

  FILE *fp = fopen(filepath, "r");
  if (fp == NULL) {
      perror(filepath);
  } 
  else {
    char buffer[BLOCK_SIZE];
    int bytesRead = fread(buffer, 1, BLOCK_SIZE, fp);
    while (bytesRead == BLOCK_SIZE) {
      if (fwrite(buffer, 1, BLOCK_SIZE, archive) != BLOCK_SIZE) {
        perror(filepath);
        break;
      }
      bytesRead = fread(buffer, 1, BLOCK_SIZE, fp);
    }
    
    if (ferror(fp) == 0) {
      if (bytesRead != 0) {
        if (fwrite(buffer, 1, bytesRead, archive) != bytesRead) {
          perror(filepath);
        }
        memset(buffer, '\0', BLOCK_SIZE);
        if (fwrite(buffer, 1, BLOCK_SIZE-bytesRead, archive) != BLOCK_SIZE-bytesRead) {
          perror(filepath);
        }
      }
    }
    else {
      perror(filepath);
      clearerr(fp);
    }
    // make sure to close the file
    if (fclose(fp) != 0) {
      perror(filepath);
    }
  }
}

void saveHeader(FILE *archive, struct tar_header header) {
  
  if (fwrite(&header, 1, BLOCK_SIZE, archive) != BLOCK_SIZE) {
    perror(header.name);
  }
}

char *getName(char * path) {
  int pathSize = strlen(path);
  path += pathSize;
  for (int i = 0; i < pathSize; i++) {
    if (*path == '/') {
      path++;
      return path;
    }
    path--;
  }
  return path;
}
