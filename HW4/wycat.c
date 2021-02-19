/*
* wycat.c.txt
* Author: Jason Fantl
* Date: Feb 18, 2021
*
* COSC 3750, Homework 4
*
* This is the a simple version of the cat utility
*
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 16

void printBuffer(char buffer[], int size, char* filepath) {
  int writeFlag = fwrite(buffer, size, 1, stdout);
  if (writeFlag != 1) {
    perror(filepath);
  }
}

void printFile(FILE *in, char* filepath) {
  char buffer[BUFFER_SIZE];
  int readFlag = 0;
  do {
    memset(buffer, 0, BUFFER_SIZE);

    readFlag = fread(buffer, BUFFER_SIZE, 1, in);
    // check if the read diddn't fill the buffer
    if (readFlag != 1) {
      // check if we reached end of file
      int eofFlag = feof(in);
      clearerr(in);
      if (eofFlag != 0) {
        // find where eof was and print up to that
        for (int i = 0; i < BUFFER_SIZE; i++) {
          if (buffer[i] == 0) {
            printBuffer(buffer, i, filepath);
            break;
          }
        }
      }
      else {
        // this would be a read error
        perror(filepath);
        break;
      }
    }
    else {
      printBuffer(buffer, BUFFER_SIZE, filepath);
    }
  } while (readFlag == 1);
}

void printUserInput() {
  clearerr(stdin);
  printFile(stdin, "user input"); 
}

void printFileInput(char *filepath) {
  FILE *fp = fopen(filepath, "r");
  if (fp == NULL) {
      perror(filepath);
  } 
  else {
    printFile(fp, filepath);
    // make sure to close the file
    int closeFlag = fclose(fp);
    if (closeFlag != 0) {
      perror(filepath);
    }
  }
}

int main (int argc, char *argv[]) {
  if (argc == 1) {
    printUserInput();
  } else {
    for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-") == 0) {
        printUserInput();
      } 
      else {
        printFileInput(argv[i]);
      }
    }
  }
}
