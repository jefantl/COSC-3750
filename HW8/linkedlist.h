/*
* linkedlist.h
* Author: Jason Fantl
* Date: April 15, 2021
*
* COSC 3750, Homework 8
*
* header file for linkedlist.c. Creates list of tokens in wyshell.c
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node Node;
typedef struct word Word;

struct node {
  Node *previous, *next;
  char* command;
  Word* arglist;
  int input, output, error;
  char *in_file, *out_file, *err_file;
  int out_file_append, err_file_append;
};

struct word {
  Word *previous, *next;
  char* string;
};

Node* newNode(Node *);
int outputRedirected(Node *);
int inputRedirected(Node *);
int errorRedirected(Node *);
void freeWords(Word *);
void freeNodes(Node *);
Word* addWord(Word *current, char* string);
