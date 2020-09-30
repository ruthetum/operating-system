#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// 핸들러 함수 정의
void handlerFunc(int sig) {
  printf("handlerFunc() 호출됨\n");
  // SIGINT : 키보드 인터럽트
  // SIG_DFL : 기본 행동 수행, 메인 함수가 하던 일을 계속 수행하고 한 번 더 SIGINT가 오는 경우 프로세스 종료
  signal(SIGINT, SIG_DFL);
}

int main() {
  // SIGINT(키보드 인터럽트) 신호를 받으면 handlerFunc 수행
  signal(SIGINT, handlerFunc);

  int count = 0;
  while(1) {
    printf("count : %d\n", count++);
    sleep(1);
  }
  exit(0);
}