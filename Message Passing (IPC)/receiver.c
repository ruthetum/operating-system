#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define BUFFER_SIZE 1024

typedef struct {
  long msgtype;
  int value;
  char buf[BUFFER_SIZE];
} msgbuf;

int main() {
  int key_id;
  msgbuf mybuf;
  long msgtype = 1;
 
  key_id = msgget((key_t) 1234, IPC_CREAT|0666);
  if (key_id == -1) {
    perror("msgget() error");
    exit(0);
  }
  
  while (1) {
    if (msgrcv(key_id, &mybuf, sizeof(msgbuf), 1, 0) == -1) {
      perror("msgrcv() error");
      exit(0);
    }
    
    printf("value : %d\n", mybuf.value);
  }
}