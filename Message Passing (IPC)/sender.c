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
  int count = 0;
 
  key_id = msgget((key_t) 1234, IPC_CREAT|0666);
  if (key_id == -1) {
    perror("msgget() error");
    exit(0);
  }
  
  mybuf.msgtype = 1;
  while (1) {
    mybuf.value = count++;
    printf("value : %d\n", mybuf.value);
    
    if (msgsnd(key_id, &mybuf, sizeof(msgbuf), IPC_NOWAIT) == -1) {
      perror("msgsnd() error");
      exit(0);
    }
    
    sleep(10);
  }
}