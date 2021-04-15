/*
* wyshell.c
* Author: Jason Fantl
* Date: April 15, 2021
*
* COSC 3750, Homework 8
*
* A tokenizer for a basic shell
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include"wyscanner.h"
#include "linkedlist.h"


int main()
{

  int rtn;
  char *rpt;
  char buf[1024];
  
  while(1) {
    printf("$> ");
    rpt=fgets(buf,256,stdin);
    if(rpt == NULL) {
      if(feof(stdin)) {
        return 0;
      }
      else {
        perror("fgets from stdin");
        return 1;
      }
    }
    
    rtn=parse_line(buf);

    // vars to track parsing of input line

    Node *head = newNode(NULL);
    if (head == NULL) {
      continue;
    }
    Node *current = head;
    
    int finishedParsingLine = 0;
    int pipeFlag = 0;
    int runInBackground = 0;
    
    while(!finishedParsingLine){ 
      // that makes it break out when an error or EOL is seen
      
      int finishedParsingCommand = 0;
      int redirectFlag = 0;

      
      while(!finishedParsingCommand && !finishedParsingLine) {
        char* copiedLex = strdup(lexeme);
        if (copiedLex == NULL) {
          printf("error: copying lexeme failed\n");
          break;
        }
        
        if (runInBackground && rtn != EOL) {
          printf("error: the ampersand must be at the end of line\n");
          finishedParsingLine = 1;
          break;
        }
        
        if (pipeFlag) {
          if (rtn != WORD) {
            printf("Must pipe to a command\n");
            finishedParsingLine = 1;
            break;
          }
          if (current->previous == NULL) {
            printf("No command to redirect\n");
            finishedParsingLine = 1;
            break;
          }
          if (outputRedirected(current->previous)) {
            printf("Ambiguous output\n");
            finishedParsingLine = 1;
            break;
          }
          if (inputRedirected(current)) {
            printf("Ambiguous input\n");
            finishedParsingLine = 1;
            break;
          }
          
          current->input = PIPE;
          current->previous->output = PIPE; // replace later with real pipe data
          
          pipeFlag = 0;
        }
        
        if (redirectFlag) {
          printf(" --: %s\n", copiedLex);
          // handle errors
          if (rtn != WORD) {
            printf("Must redirect to a file\n");
            finishedParsingLine = 1;
            break;
          }
          if (current->command == NULL) {
            printf("No file to redirect\n");
            finishedParsingLine = 1;
            break;
          }
          switch(redirectFlag) {
            case REDIR_OUT:
              if (outputRedirected(current)) {
                printf("Ambiguous output\n");
                finishedParsingLine = 1;
                break;
              }
              current->out_file = copiedLex;
              break;
            case REDIR_IN:
              if (inputRedirected(current)) {
                printf("Ambiguous input\n");
                finishedParsingLine = 1;
                break;
              }
              current->in_file = copiedLex;
              break;
            case APPEND_OUT:
              if (outputRedirected(current)) {
                printf("Ambiguous output\n");
                finishedParsingLine = 1;
                break;
              }
              current->out_file = copiedLex;
              current->out_file_append = 1;
              break;
            case REDIR_ERR:
             if (errorRedirected(current)) {
                printf("Ambiguous error output\n");
                finishedParsingLine = 1;
                break;
              }
              current->err_file = copiedLex;
              break;
            case APPEND_ERR:
              if (errorRedirected(current)) {
                printf("Ambiguous error output\n");
                finishedParsingLine = 1;
                break;
              }
              current->err_file = copiedLex;
              current->err_file_append = 1;
              break;
            default:
              printf("%d not yet implemented\n", redirectFlag);
          }
          redirectFlag = 0;
        } 
        else { // then handle new commands and arguments
          switch(rtn) {
            case WORD:
              if (current->command == NULL) {
                current->command = copiedLex;
                printf(":--: %s\n", copiedLex);
              }
              else {
                if (current->arglist == NULL) {
                  current->arglist = calloc(1, sizeof(Word));
                  if (current->arglist == NULL) {
                    printf("error: creating new argument failed");
                    finishedParsingLine = 1;
                    break;
                  }
                  current->arglist->string = copiedLex;
                  }
                else {
                // add new argument
                 addWord(current->arglist, copiedLex);
                }
                printf(" --: %s\n", copiedLex);
              }
              break;
            case AMP:
              printf(" &\n");
              runInBackground = 1;
              break;
            case REDIR_ERR_OUT:
              printf(" 2>&1\n");
              if (errorRedirected(current)) {
                printf("Ambiguous error output\n");
                finishedParsingLine = 1;
                break;
              }
              current->err_file = current->out_file;
              break;
            case ERROR_CHAR:
              printf("error character: %d\n",error_char);
              finishedParsingLine = 1;
              break;
            case SYSTEM_ERROR:
              perror("system error\n");
              return 1;
            case QUOTE_ERROR:
              printf("quote error\n");
              finishedParsingLine = 1;
              break;
            
            case PIPE:
              printf(" |\n");
              finishedParsingCommand = 1;
              pipeFlag = 1;
              break;
            case SEMICOLON:
              printf(" ;\n");
              finishedParsingCommand = 1;
              break;
            case EOL:
              printf(" --: EOL\n");
              finishedParsingLine = 1;
              break;
            default: // file redirection
              switch(rtn) {
                case REDIR_OUT:
                  printf(" >\n");
                  break;
                case REDIR_IN:
                  printf(" <\n");
                  break;
                case APPEND_OUT:
                  printf(" >>\n");
                  break;
                case REDIR_ERR:
                  printf(" 2>\n");
                  break;
                case APPEND_ERR:
                  printf(" 2>>\n");
                  break;
              }
              redirectFlag = rtn;
          }
        }
      
        rtn=parse_line(NULL);
      }
      
      // chain the commands
      if (!finishedParsingLine) {
      // edge case: an empty command after a valid command
        if (current->command == NULL && current->previous != NULL) {
          current = current->previous;
          free(current->next);
          current = newNode(current);
        }
        // edge case: first command is empty
        else if(current->command == NULL && current->previous == NULL) {
          // do nothing, no chain
        } 
        // normal case
        else {
          current = newNode(current);
        }
      }
    }
    
    freeNodes(head);
  }
}
