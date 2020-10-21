#include <windows.h> // WinAPI 사용 
#include <stdio.h>

int target = 0;

int WINAPI addTarget(LPVOID param) { // 파라미터로 원래 LPVOID 타입을 사용하지만 void * 를 사용해도 같음 
	int limit = *(int *) param;
	int i=0;
	for (i=1; i<=limit; i++) {
		printf("[ADD] target : %d\n", ++target);
	}
	return 0;
}

int WINAPI subtractTarget(LPVOID param) {
	int limit = *(int *) param;
	int i=0;
	for (i=1; i<=limit; i++) {
		printf("[SUBTRACT] target : %d\n", --target);
	}
	return 0;
}

int main() {
	int threadId1;
	int threadId2;
	HANDLE threadHandle1;
	HANDLE threadHandle2;
	int param=100;
	
	// 스레드 생성
	threadHandle1 = CreateThread(NULL, 0, addTarget, &param, 0, &threadId1);
	threadHandle2 = CreateThread(NULL, 0, subtractTarget, &param, 0, &threadId2);
	
	Sleep(3000);
	printf("target : %d\n", target);
}