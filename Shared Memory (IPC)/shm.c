#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  int shmid;
  int pid;
  
  int * cal_num;
  void * shared_memory = (void *) 0;
  
  // To create shared memory
  shmid = shmget((key_t) 1234, sizeof(int), 0666|IPC_CREAT);
  
  // To check error of shmid() method
  if (shmid == -1) {
    perror("shmget failed : "); // print error msg
    exit(0);
  }
  
  printf("shmid return value : %d\n", shmid);
  
  // To attach process memory for using shared memory
  shared_memory = shmat(shmid, (void *)0, 0);
  if (shared_memory == (void *) -1) {
    perror("shmat failed : ");
    exit(0);
  }
  
  cal_num = (int *) shared_memory;
  pid = fork();
  
  if (pid == 0) {
    // child process
    shmid = shmget((key_t) 1234, sizeof(int), 0);
    
    if (shmid == -1) {
      perror("shmget failed : ");
      exit(0);
    }
    shared_memory = shmat(shmid, (void *) 0,  0666|IPC_CREAT);
    if (shared_memory == (void *)-1) {
      perror("shmat failed : ");
      exit(0);
    }
    cal_num = (int *) shared_memory;
    *cal_num = 1;
    
    while(1) {
      *cal_num = *cal_num + 1;
      printf("child %d\n", *cal_num);
      sleep(1);
    }
  } else if (pid>0) {
    // parent process
    while(1) {
      sleep(1);
      printf("*cal_num : %d\n", *cal_num);
    }
  }
}
