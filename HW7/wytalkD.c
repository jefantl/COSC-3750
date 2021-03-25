#include "wytalkUtil.h"
#include "socketfun.h"


int main()
{
  char hostname[HOST_NAME_MAX + 1];
  gethostname(hostname, HOST_NAME_MAX + 1);
  if (hostname == NULL) {
    perror("Fetching hostname");
    return 0;
  }
  
  int socketDescriptor = serve_socket(hostname, 51100);
  if (socketDescriptor == -1) {
    return 0;
  }
  
  int fd = accept_connection(socketDescriptor);
  if (fd == -1) {
    return 0;
  }
  
  FILE *fp = fdopen(fd, "r+");
  if (fp == NULL) {
    perror("Opening socket");
    return 0;
  } 
  
  
  while (true) {
    char buffer[BUFFER_SIZE];
    if (recieveSocket(fp, buffer) != 0) {
      printf("%s", buffer);
      closeSocket(fp);
      break;
    } 
    else {
      printf("%s", buffer);
    }
    
    if (sendUserInput(fp) != 0) {
      closeSocket(fp);
      break;
    }
  }
}
