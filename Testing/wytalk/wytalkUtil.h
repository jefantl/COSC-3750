/*
* wytalkUtil.h
* Author: Jason Fantl
* Date: May 25, 2021
*
* COSC 3750, Homework 7
*
* Header for wytalkUtil.c. Basic functions to send and recieve over sockets.
*
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int sendUserInput(FILE*);
int recieveSocket(FILE*, char*);
void closeSocket(FILE*);
