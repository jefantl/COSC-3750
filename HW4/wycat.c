/*
* wycat.c.txt
* Author: Jason Fantl
* Date: Feb 18, 2021
*
* COSC 3750, Homework 4
*
* This is a simple version of the cat utility.
*
*/

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

void printBuffer(char buffer[], int size, char* filepath) {
  int writeFlag = fwrite(buffer, 1, size, stdout);
  if (writeFlag != size) {
    perror(filepath);
  }
}

void printFile(FILE *in, char* filepath) {
  char buffer[BUFFER_SIZE];
  int bytesRead = 0;
  do {
    bytesRead = fread(buffer, 1, BUFFER_SIZE, in);
    // check if the read diddn't fill the buffer
    if (bytesRead != BUFFER_SIZE) {
      // check if we found an error
      int errorFlag = ferror(in);
      if (errorFlag != 0) {
        // this would be a read error
        perror(filepath);
        clearerr(in);
        break;
      }
      else {
        printBuffer(buffer, bytesRead, filepath);
      }
      clearerr(in);
    }
    else {
      printBuffer(buffer, BUFFER_SIZE, filepath);
    }
  } while (bytesRead == BUFFER_SIZE);
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
