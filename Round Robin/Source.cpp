#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string>
#include <queue>

#define PROCESS_NUM 10
#define QUANTUM 0.01
#define _CRT_SECURE_NO_WARNINGS

using namespace std;

// process 구조체
typedef struct {
  long pid;
  float cpu_burst;
  float io_burst;
} PROCESS;

typedef struct {
  long status;
  float burst;
} PCB;

typedef struct {
  long msgtype;
  PCB pcb;
} MSG;

int main() {
  // pid 담을 배열
  PROCESS child[PROCESS_NUM];
  int count = 0;
  
  for (int i=0; i<PROCESS_NUM; i++) {
    // 자식 프로세스 생성
    long newProcess = fork();
    
    child[i].pid = newProcess;
    child[i].cpu_burst = rand() % 20 + 1; // CPU burst : 1~30 
//     child[i].io_burst = rand() % 10 + 1; // IO burst : 1~10
    
    // 자식 프로세스일 경우 무한 루프 실행
    if (newProcess == 0) {
      int key_id;
      long msgtype = getpid(); // 메세지 타입은 자식 프로세스 인덱스로 설정
      float cpu_burst = child[i].cpu_burst;
      
      MSG msg;
      msg.msgtype = msgtype;
      
      // 메세지 생성 및 에러 핸들링
      key_id = msgget((key_t) 1243, IPC_CREAT|0666);
      if (key_id == -1) {
        perror("msgget() error");
        exit(0);
      }

      do {
        // 자식 프로세스 로직
        // 부모에게 메세지 올 때까지 대기
        if (msgrcv(key_id, &msg, sizeof(PCB), msgtype, 0) != -1) {
          if (cpu_burst > QUANTUM) { // 남아 있는 burst time 이 quantum보다 큰 경우
            cpu_burst -= QUANTUM;
            sleep(QUANTUM);
//             printf("[%ld] 남은 Burst : %.2f\n", msgtype, cpu_burst);

            msg.pcb.status = 1;
            msg.pcb.burst = QUANTUM;
            msgsnd(key_id, &msg, sizeof(PCB), IPC_NOWAIT);

          } else { // // 남아 있는 burst time 이 quantum보다 작은 경우
            cpu_burst = 0;
            sleep(cpu_burst);
            printf("\t\t[%ld] 자식 프로세스 종료\n", msgtype);

            msg.pcb.status = 0;
            msg.pcb.burst = cpu_burst;
            msgsnd(key_id, &msg, sizeof(PCB), IPC_NOWAIT);
          }
        }
      } while(cpu_burst > 0);
      
      exit(0); // 자식 프로세스 정상 종료
    }
  }
  
  // 파일 작성
  FILE *fp = fopen("log.txt", "a");
  
  // 작업 queue 생성
  queue<long> readyQueue;
  queue<long> queueForPrint;
  float rec_burst[PROCESS_NUM];
  float completion_time[PROCESS_NUM];
  float total_time = 0;
  char s1[50];
  
  printf("[List of Processes]\n\n");
  for (long j=0; j<PROCESS_NUM; j++) {
    readyQueue.push(j);
    printf("PID : \t%ld\n", child[j].pid);
    printf("Arrival Time : \t%ld\n", j);
    printf("Burst Time : \t%.2lf\n\n", child[j].cpu_burst);
    rec_burst[j] = child[j].cpu_burst;
  }

  int key_id;
  MSG msg;
  
  // 메세지 생성
  key_id = msgget((key_t) 1243, IPC_CREAT|0666);
  if (key_id == -1) {
    perror("msgget() error");
    exit(0);
  }
  
  // 10개의 자식 프로세스 생성 후 부모 프로세스 로직
  do {
    count++;
    long ready = readyQueue.front();  // 큐의 다음 순서 값 추출
    readyQueue.pop(); //dequeue
    msg.msgtype = child[ready].pid;  // msgtype 해당 자식 프로세스 pid 값으로 설정
    
    // 자식 프로세스에게 메시지
    msgsnd(key_id, &msg, sizeof(PCB), IPC_NOWAIT);
   
    
    if (msgrcv(key_id, &msg, sizeof(PCB), child[ready].pid, 0) != -1) {
      // burst가 남은 경우 다시 큐에 enqueue
      if (msg.pcb.status > 0) {
        if (child[ready].cpu_burst > 0) {
          readyQueue.push(ready);
          child[ready].cpu_burst -= QUANTUM;
          total_time += QUANTUM;
        }
      } else {
        total_time += child[ready].cpu_burst;
        child[ready].cpu_burst = 0;
        completion_time[ready] = total_time;
      }
      
      if (child[ready].cpu_burst <= 0) {
          total_time += (child[ready].cpu_burst + QUANTUM);
          child[ready].cpu_burst = 0;
          completion_time[ready] = total_time;
      }

      // 출력 수 조절
      if (count % 100 == 0) {
        // queue print(consile)
        int index = 0;
        do {
          printf("[%ld] <- ", child[readyQueue.front()].pid);
          queueForPrint.push(readyQueue.front());
          // write text file
          sprintf(s1, "%ld <- ", child[readyQueue.front()].pid);
          fputs(s1, fp);
          readyQueue.pop();
          if (readyQueue.size() == 1) {
            printf("[%ld]\n", child[readyQueue.front()].pid);
            queueForPrint.push(readyQueue.front());
            // write text file
            sprintf(s1, "%ld\n", child[readyQueue.front()].pid);
            fputs(s1, fp);
            readyQueue.pop();
          }
        } while (!readyQueue.empty());

        index = 0;
        do {
          readyQueue.push(queueForPrint.front());
          queueForPrint.pop();
        } while(!queueForPrint.empty());
        }
    }
  } while(!readyQueue.empty());
  
  
  float sum_comp = 0;
  float sum_wait = 0;
  printf("\n\n[List of Processes]\n\n");
  fputs("\n\n[List of Processes]\n\n", fp);
  printf("PID\t\tBurst Time\tCompletion Time\t\tWating Time\n\n");
  fputs("PID\t\tBurst Time\tCompletion Time\t\tWating Time\n\n", fp);
  for (long m=0; m<PROCESS_NUM; m++) {
    printf("%ld\t\t%.2lf\t\t%.2lf\t\t\t%.2lf\n\n", child[m].pid, rec_burst[m], completion_time[m], completion_time[m]-rec_burst[m]);
    sprintf(s1, "%ld\t\t", child[m].pid);
    fputs(s1, fp);
    sprintf(s1, "%.2lf\t\t", rec_burst[m]);
    fputs(s1, fp);
    sprintf(s1, "%.2lf\t\t", completion_time[m]);
    fputs(s1, fp);
    sprintf(s1, "%.2lf\n\n", completion_time[m]-rec_burst[m]);
    fputs(s1, fp);
    sum_comp += completion_time[m];
    sum_wait += (completion_time[m]-rec_burst[m]);
  }
  printf("Avg Wating Time : %.2f\n", sum_wait/PROCESS_NUM);
  sprintf(s1, "Avg Wating Time : %.2f\n", sum_wait/PROCESS_NUM);
  fputs(s1, fp);
  printf("Avg Completion Time : %.2f\n\n", sum_comp/PROCESS_NUM);
  sprintf(s1, "Avg Completion Time : %.2f\n\n", sum_comp/PROCESS_NUM);
  fputs(s1, fp);
  
  fclose(fp);
  return 0;
}