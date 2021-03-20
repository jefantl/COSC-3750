/*
* wytar.c.txt
* Author: Jason Fantl
* Date: Mar 16, 2021
*
* COSC 3750, Homework 6
*
* This is a simple version of the tar utility.
*
*/

#include "wytarUtil.h"

int main (int argc, char *argv[]) {

  // process flags
  int actionFlag = 0;
  int outFilepathIndex = 0;
  
  int argIndex = 1;
  for (; argIndex < argc; ++argIndex) {
    if (argIndex == outFilepathIndex) {
      continue;
    }
    char *flagChar = argv[argIndex];
    if (*flagChar == '-') {
      flagChar++;
      for (; *flagChar != '\0'; flagChar++) {
        if (*flagChar == 'c') {
          if (actionFlag == -1) {
            printf("Cannot both extract and save\n");
            return 0;
          } else {
            actionFlag = 1;
          }
        } else if (*flagChar == 'x') {
          if (actionFlag == 1) {
            printf("Cannot both extract and save\n");
            return 0;
          } else {
            actionFlag = -1;
          }
        } else  if (*flagChar == 'f') {
          outFilepathIndex = argIndex+1;
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

  // check flags are all valid
  if (actionFlag == 0) {
    printf("Must choose to either extract or save\n");
    return 0;
  }
  if (outFilepathIndex == 0 || outFilepathIndex == argc) {
    printf("A filename must be specified\n");
    return 0;
  }
  
  if (actionFlag == 1) {
    save(argv[outFilepathIndex], argv, argIndex, argc);
  } else {
    extract(argv[outFilepathIndex]);
  }
}
