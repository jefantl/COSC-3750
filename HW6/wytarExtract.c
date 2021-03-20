#include "wytarUtil.h"


// NOTES:
// have to use chmod to make executables executable when we extract them
// use symlink to create soft links
void extract(char *archivePath) {
  printf("extracting from %s \n",archivePath);

  FILE *fp = fopen(archivePath, "r");
  if (fp == NULL) {
      perror(archivePath);
  } 
  else {
    bool reading = true;
    
    while (reading) {
      struct tar_header header;
      if (fread(&header, 1, BLOCK_SIZE, fp) != BLOCK_SIZE) {
        perror("reading archive");
        return;
      }
      
      // check it is a header by the check-sum
      int givenChksum = octalStringToDecimal(header.chksum);
      int realChksum = 0;
      memset(header.chksum, ' ', 8);
      char *p = (char*)&header;
      for (int i = 0; i < BLOCK_SIZE; i++) {
        realChksum += *p;
        p++;
      }
      if (realChksum == givenChksum) {
        extractObject(fp, header);
      }
    }
  }
  if (fclose(fp) != 0) {
    perror(archivePath);
  }
}

void extractObject(FILE *archivep, struct tar_header header) {
  if (header.typeflag == REGTYPE) {
    extractFile(archivep, header);
  }
  else if (header.typeflag == SYMTYPE) {
    extractLink(archivep, header);
  }
  else if (header.typeflag == DIRTYPE) {
    extractDirectory(archivep, header);
  }
  else {
    printf("object type not recognized, could not extract");
    return;
  }
}

void extractFile(FILE *archivep, struct tar_header header) {
  // try to create object
  char filepath[255];
  bool created = createFile(filepath, header);
  if (!created) {
    return;
  }
  
  FILE *fp = fopen(filepath, "w");
  if (fp == NULL) {
    perror(filepath);
    return;
  }
  
  int writtenBytes = 0;
  int desiredBytes = octalStringToDecimal(header.size);
  while (writtenBytes < desiredBytes) {
    int toWrite = BLOCK_SIZE;
    if (writtenBytes > desiredBytes-512) {
      toWrite = desiredBytes - writtenBytes;
    } 
    char buffer[BLOCK_SIZE];
    memset(buffer, '\0', BLOCK_SIZE);
    
    if (fread(buffer, 1, BLOCK_SIZE, archivep) != BLOCK_SIZE) {
      perror(filepath);
      break;
    }
      
    if (fwrite(buffer, 1, toWrite, fp) != toWrite) {
      perror(filepath);
      break;
    }
    
    writtenBytes += BLOCK_SIZE;
  }
  
  if (fclose(fp) != 0) {
    perror(filepath);
  }
}

void extractLink(FILE *archivep, struct tar_header header) {
  printf("extracting %s \n", header.name);
  if (symlink(header.linkname, header.name) != 0) {
    perror(header.name);
    return;
  }
}

void extractDirectory(FILE *archivep, struct tar_header header) {
  char filepath[255];
  memset(filepath, 0, 255);
  strcat(filepath, header.prefix);
  strcat(filepath, header.name);
  
  printf("extracting %s \n", filepath);
  
  if (mkdir(filepath, octalStringToDecimal(header.mode)) != 0) {
    perror(filepath);
    return;
  }
}

bool createFile(char* filepath, struct tar_header header) {

  memset(filepath, 0, 255);
  if (header.prefix[0] != '\0') { 
    strncat(filepath, header.prefix, 155);
  } 
  if (header.name[99] != '\0') { 
    strncat(filepath, header.name, 100);
  } 
  else {
    strcat(filepath, header.name);
  }

  printf("extracting %s \n", filepath);
  
  // create the file, trying to create necessary directories
  FILE *fp = fopen(filepath, "w");
  if (fp == NULL) {
    if (errno == ENOENT) {
      // need to create directory
      if (mkdir(filepath, 0777 != 0)) {
        perror(filepath);
        return false;
      }
    }
    else {
      perror(filepath);
      return false;
    }
  }
  fp = fopen(filepath, "w");
  if (fp == NULL) {
    perror(filepath);
    return false;
  }
  
  // setting permissions
  int mode = octalStringToDecimal(header.mode);
  if (chmod(filepath, mode) != 0) {
    perror(filepath);
  }
  
  // setting UID, GID
  if (strcmp(header.magic, TMAGIC) == 0) {
    struct passwd *pname = getpwnam(header.uname);
    struct group *pgroup = getgrnam(header.gname);
    if (pgroup != NULL && pname != NULL) {
      if (chown(filepath, pname->pw_uid, pgroup->gr_gid) != 0) {
        perror(filepath);
      }
    }
  }
  if (fclose(fp) != 0) {
    perror(filepath);
  }
  
  return true;
}

int octalStringToDecimal(char* octalString) {
  unsigned int octal = atoi(octalString);
  unsigned int decimal = 0;
  int power = 1;
  for(int i = 0; octal != 0; i++) {

    decimal += (octal%10) * power;
    octal/=10;
    power *= 8;
  }

  return decimal;
}
