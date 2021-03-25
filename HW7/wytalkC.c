#include "socketfun.h"
#include "wytalkUtil.h"

int main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Must give a hostname");
    return 0;
  }
  
  char* hostname = argv[1];
  
  // printf("Connecting to %s...\n", hostname);
  int socketDescriptor = request_connection(hostname, 51100);
  if (socketDescriptor == -1) {
    return 0;
  }
  
  FILE *fp = fdopen(socketDescriptor, "r+");
  if (fp == NULL) {
    perror("Opening socket");
    return 0;
  } 
  
  while (true) {
    if (sendUserInput(fp) != 0) {
      closeSocket(fp);
      break;
    }
    
    char buffer[BUFFER_SIZE];
    if (recieveSocket(fp, buffer) != 0) {
      printf("%s", buffer);
      closeSocket(fp);
      break;
    } 
    else {
      printf("%s", buffer);
    }
  }

  return 0;
}
