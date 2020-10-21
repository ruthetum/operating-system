# C로 Multi-Thread 구현

Velog : https://velog.io/@ruthetum/C-Multi-Thread-%EA%B5%AC%ED%98%84-1

### 예제 코드 설명
- 하나의 스레드는 전역 변수 target을 계속 더해주고(target++), 다른 하나의 스레드는 target을 계속 빼주는(target--) 멀티 스레드 예제 코드입니다.
- 스레드 동기화(Thread Synchronization)없이 두 개의 스레드가 경쟁하는 구조입니다.

## 실행 환경
### Windows
- OS : Windows 10

### Linux
- OS : Ubuntu 16.04 LTS
- GCC : 5.4.0 (Ubuntu 5.4.0-6ubuntu1~16.04.12)
- gcc 컴파일 시 <code>-lpthread</code> 옵션 추가

