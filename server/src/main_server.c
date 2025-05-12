#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network_server.h"  // <<< 이 줄이 매우 중요합니다! handle_client 선언을 가져옵니다.

// 개발자 B가 만들 파일에 대한 헤더 (Makefile에 의해 임시 생성되거나 실제 파일이 필요)
#include "auth.h"
#include "score_manager.h"
// #include "db_handler.h" // main_server.c에서 직접 db_handler 함수를 호출하지 않는다면 불필요

#define PORT 8080
#define MAX_CLIENTS 10

int main() {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_size;
  pthread_t tid;

  // 서버 소켓 생성
  server_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (server_sock == -1) {
    perror("socket() error");
    exit(EXIT_FAILURE);
  }

  // 서버 주소 설정
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(PORT);

  // 주소 재사용 옵션 설정
  int opt = 1;
  setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // 소켓에 주소 바인딩
  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("bind() error");
    close(server_sock);
    exit(EXIT_FAILURE);
  }

  // 연결 요청 대기
  if (listen(server_sock, MAX_CLIENTS) == -1) {
    perror("listen() error");
    close(server_sock);
    exit(EXIT_FAILURE);
  }

  printf("Rain Typing Game Server started on port %d...\n", PORT);

  // 개발자 B의 모듈 초기화 (필요하다면 실제 함수 호출)
  // 예시: init_auth_system();
  // 예시: init_score_system();

  while (1) {
    client_addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_sock == -1) {
      perror("accept() error");
      continue;
    }

    printf("Client connected: %s:%d (socket: %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sock);

    int *p_client_sock = malloc(sizeof(int));
    if (!p_client_sock) {
      perror("malloc for client_sock failed");
      close(client_sock);
      continue;
    }
    *p_client_sock = client_sock;

    // 여기서 handle_client 함수가 사용됩니다.
    if (pthread_create(&tid, NULL, handle_client, (void *)p_client_sock) != 0) {
      perror("pthread_create() error");
      free(p_client_sock);
      close(client_sock);
    }
    pthread_detach(tid);
  }

  close(server_sock);
  return 0;
}