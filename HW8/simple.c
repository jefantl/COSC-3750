 /*************************************
 * simple.c
 * Author: Kim Buckner
 * Date: 8 April 2020
 *
 * Really simple scanner demo for 3750 wyshell project.
 *
 * MAKEFILE NOTE::::  You WILL get warnings about unused functions in the
 * wyscanner.c file when you use -Wall, unless you turn them off with 
 * -Wno-unused-function
 *
 * The values returned by parse_line() are in the wyscanner.h file. I have just
 * made some strings in an array, because I am lazy, to correspond with those. 
 * The "tokens[rtn&96]" is the way I access. You will see that QUOTE_ERROR has
 * a value of 96 in the header file, and so on. 
 ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include"wyscanner.h"
#include"linkedlist.h"


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
    int finishedParsingLine = 0;

    Node *head = newNode(NULL);
    if (head == NULL) {
      continue;
    }
    Node *current = head;
    
    while(!finishedParsingLine){ 
      // that makes it break out when an error or EOL is seen
      
      int finishedParsingCommand = 0;
      int redirectFlag = 0;
      int runInBackground = 0;
      
      while(!finishedParsingCommand && !finishedParsingLine) {
        char* copiedLex = strdup(lexeme);
        if (copiedLex == NULL) {
          printf("error: copying lexeme failed\n");
          break;
        }
        
        // handle errors
        if (runInBackground && rtn != EOL) {
          printf("error: the ampersand must be at the end of line\n");
          finishedParsingLine = 1;
          break;
        }
        if (redirectFlag != 0) { // handle file redirections
        // handle errors
          if (rtn != WORD) {
            printf("error redirecting: must redirect to a file\n");
            finishedParsingLine = 1;
            break;
          }
          if (current->command == NULL) {
            printf("error redirecting: no file to redirect\n");
            finishedParsingLine = 1;
            break;
          }
          switch(redirectFlag) {
            case REDIR_OUT:
              if (current->out_file != NULL) {
                printf("error redirecting: cannot output to multiple files\n");
                finishedParsingLine = 1;
                break;
              }
              current->out_file = copiedLex;
              break;
            case REDIR_IN:
              if (current->in_file != NULL) {
                printf("error redirecting: cannot get input from multiple files\n");
                finishedParsingLine = 1;
                break;
              }
              current->in_file = copiedLex;
              break;
            case APPEND_OUT:
              if (current->out_file != NULL) {
                printf("error redirecting: cannot output to multiple files\n");
                finishedParsingLine = 1;
                break;
              }
              current->out_file = copiedLex;
              current->out_file_append = 1;
              break;
            case REDIR_ERR:
             if (current->err_file != NULL) {
                printf("error redirecting: cannot output errors to multiple files\n");
                finishedParsingLine = 1;
                break;
              }
              current->err_file = copiedLex;
              break;
            case APPEND_ERR:
              if (current->err_file != NULL) {
                printf("error redirecting: cannot output errors to multiple files\n");
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
              }
              else {
                if (current->arglist == NULL) {
                  current->arglist = calloc(1, sizeof(Word));
                  if (current->arglist == NULL) {
                    printf("error: Creating new argument failed");
                    finishedParsingLine = 1;
                    break;
                  }
                  current->arglist->string = copiedLex;
                  }
                else {
                // add new argument
                 addWord(current->arglist, copiedLex);
                }
              }
              break;
            case AMP:
              runInBackground = 1;
              break;
            case REDIR_ERR_OUT:
              if (current->err_file != NULL) {
                printf("error redirecting: cannot output errors to multiple files\n");
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
            
            case SEMICOLON:
              finishedParsingCommand = 1;
              break;
            case EOL:
              finishedParsingLine = 1;
              break;
            default: // file redirection
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
    
    // print the parsed line of input
    Node *tmp = head;
    while (tmp != NULL) {
      if (tmp->command != NULL) {
        printf(":--: %s\n", tmp->command);
        Word *wtmp = tmp->arglist;
        while (wtmp != NULL) {
          printf(" --: %s\n", wtmp->string);
          wtmp = wtmp->next;
        }
        
        if (tmp->in_file != NULL) {
          printf(" < %s\n", tmp->in_file);
        }
        if (tmp->out_file != NULL) {
          if (current->out_file_append) {
            printf(" >> %s\n", tmp->out_file);
          }
          else {
            printf(" > %s\n", tmp->out_file);
          }
        }
        if (tmp->err_file != NULL) {
          if (current->err_file_append) {
            printf(" 2>> %s\n", tmp->err_file);
          } 
          else {
            printf(" 2> %s\n", tmp->err_file);
          }
        }
      }
      
      tmp = tmp->next;
    }
    
    printf(" --: EOL\n");
    
    freeNodes(head);
  }
}
