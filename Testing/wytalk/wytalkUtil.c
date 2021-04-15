/*
* wytalkUtil.c
* Author: Jason Fantl
* Date: May 25, 2021
*
* COSC 3750, Homework 7
*
* Basic functions to send and recieve over sockets.
*
*/

#include "wytalkUtil.h"

int sendUserInput(FILE* fp) {
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  
  if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
    return -1;
  }

  if (fwrite(buffer, 1, strlen(buffer), fp) != strlen(buffer)) {
    perror("Writting to socket");
    return -1;
  }
  return 0;
}

int recieveSocket(FILE* fp, char* buffer) {

  memset(buffer, 0, BUFFER_SIZE);
  char *bufferIndex = buffer;
  
  while (fread(bufferIndex, 1, 1, fp) == 1) {
    if (*bufferIndex == '\n') {
      bufferIndex++;
      *bufferIndex = '\0';
      return 0;
    }
    else {
      bufferIndex++;
    }
  }
  if (ferror(fp) != 0) {
    // this would be a read error
    perror("Reading from socket");
    return -1;
  }
  return -1;
}

void closeSocket(FILE *fp) {
  if (fclose(fp) != 0) {
    perror("Closing socket");
  }
}
