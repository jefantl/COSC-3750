/*
* linkedlist.c
* Author: Jason Fantl
* Date: April 15, 2021
*
* COSC 3750, Homework 8
*
* functions that create, free, and in general handle a list data structure
*
*/

#include"linkedlist.h"

Node* newNode(Node *current) {
  Node *nextNode;
  
  nextNode = calloc(1, sizeof(Node));
  if (nextNode == NULL) {
    printf("error: Creating new node failed");
    return NULL;
  }
  
  // default values
  nextNode->input = STDIN_FILENO;
  nextNode->output = STDOUT_FILENO;
  nextNode->error = STDOUT_FILENO;
  
  if (current != NULL) {
    nextNode->previous = current;
    current->next = nextNode;
  }
  return nextNode;
}

int outputRedirected(Node *n) {
  return (n->output != STDOUT_FILENO || n->out_file != NULL);
}

int inputRedirected(Node *n) {
  return (n->input != STDIN_FILENO || n->in_file != NULL);
}

int errorRedirected(Node *n) {
  return (n->error != STDOUT_FILENO || n->err_file != NULL);
}

void freeWords(Word *head) {
  if (head != NULL) {
    freeWords(head->next);
    free(head);
  }
}

void freeNodes(Node *head) {
  if (head != NULL) {
    freeWords(head->arglist);
    freeNodes(head->next);
    free(head);
  }
}

Word* addWord(Word *current, char* string) {
  Word *nextWord = calloc(1, sizeof(Word));
  if (nextWord == NULL) {
    printf("error: Creating new word failed");
  }
  nextWord->string = string;
  
  if (current != NULL) {
      // find last arg in list
    Word *tmpWord = current;
    while (tmpWord->next != NULL) {
      tmpWord = tmpWord->next;
    }
    
    nextWord->previous = tmpWord;
    tmpWord->next = nextWord;
  }
  
  return nextWord;
}
