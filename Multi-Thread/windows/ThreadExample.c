#include <windows.h> // WinAPI 사용  
#include <stdio.h>

int target = 0;

int WINAPI addTarget(void * param) { // 파라미터로 원래 LPVOID 타입을 사용하지만 void * 를 사용해도 같음
	int limit = *(int *) param;
	int i=0;
	for (i=1; i<=limit; i++) {
		target += i;
		printf("target : %d [+%d]\n", target, i);
	}
	return 0;
}

int main() {
	int threadId;
	HANDLE threadHandle;
	int param=10;
	
	// ������ ����
	threadHandle = (HANDLE) _beginthreadex(NULL, 0, addTarget, &param, 0, &threadId);
	
	if (threadHandle != NULL) {
		WaitForSingleObject(threadHandle, INFINITE);
		CloseHandle(threadHandle);
		printf("target : %d\n", target);
	}
}
