#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network_client.h"  // 서버 통신 API 헤더
#include "protocol.h"        // 메시지 구조체 등을 위해

#define SERVER_IP "127.0.0.1"  // 로컬에서 서버 실행 시
#define SERVER_PORT 8080

void print_horizontal_line() { printf("--------------------------------------------------\n"); }

void display_leaderboard(const LeaderboardResponse* resp) {
  print_horizontal_line();
  printf("           LEADERBOARD\n");
  print_horizontal_line();
  if (resp->count == 0) {
    printf("Leaderboard is empty or error: %s\n", resp->message);
  } else {
    printf("Rank | Username           | Score\n");
    print_horizontal_line();
    for (int i = 0; i < resp->count; ++i) {
      printf("%-4d | %-18s | %d\n", i + 1, resp->entries[i].username, resp->entries[i].score);
    }
  }
  print_horizontal_line();
}

int main() {
  char choice_buffer[10];
  int choice;
  char username[MAX_ID_LEN];
  char password[MAX_PW_LEN];
  int score_to_submit;

  if (connect_to_server(SERVER_IP, SERVER_PORT) != 0) {
    fprintf(stderr, "Failed to connect to the server. Exiting.\n");
    return 1;
  }
  printf("Successfully connected to the server at %s:%d\n", SERVER_IP, SERVER_PORT);

  while (1) {
    print_horizontal_line();
    printf("Rain Typing Test Client Menu:\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Submit Score (requires login)\n");
    printf("4. View Leaderboard\n");
    printf("5. Logout (if logged in)\n");
    printf("0. Exit\n");
    print_horizontal_line();
    printf("Enter your choice: ");

    if (fgets(choice_buffer, sizeof(choice_buffer), stdin) == NULL) {
      printf("Error reading input. Exiting.\n");
      break;
    }
    choice_buffer[strcspn(choice_buffer, "\n")] = 0;

    if (strlen(choice_buffer) == 0) {
      printf("No input. Please try again.\n");
      continue;
    }

    char* endptr;
    choice = strtol(choice_buffer, &endptr, 10);
    if (*endptr != '\0') {
      printf("Invalid input. Please enter a number.\n");
      continue;
    }

    switch (choice) {
      case 1:  // Register
        printf("Enter username to register: ");
        fgets(username, MAX_ID_LEN, stdin);
        username[strcspn(username, "\n")] = 0;
        printf("Enter password: ");
        fgets(password, MAX_PW_LEN, stdin);
        password[strcspn(password, "\n")] = 0;

        RegisterResponse reg_resp;
        // 여기를 확실히 수정합니다: ®_resp -> ®_resp
        if (send_register_request(username, password, &reg_resp) == 0) {
          printf("Registration %s: %s\n", reg_resp.success ? "successful" : "failed", reg_resp.message);
        } else {
          printf("Error sending registration request.\n");
        }
        break;

      case 2:  // Login
        printf("Enter username to login: ");
        fgets(username, MAX_ID_LEN, stdin);
        username[strcspn(username, "\n")] = 0;
        printf("Enter password: ");
        fgets(password, MAX_PW_LEN, stdin);
        password[strcspn(password, "\n")] = 0;

        LoginResponse login_resp;
        if (send_login_request(username, password, &login_resp) == 0) {
          printf("Login %s: %s\n", login_resp.success ? "successful" : "failed", login_resp.message);
          if (login_resp.success) {
            printf("Logged in as: %s\n", username);
          }
        } else {
          printf("Error sending login request.\n");
        }
        break;

      case 3:  // Submit Score
        printf("Enter score to submit: ");
        char score_buffer[10];
        fgets(score_buffer, sizeof(score_buffer), stdin);
        score_buffer[strcspn(score_buffer, "\n")] = 0;

        score_to_submit = strtol(score_buffer, &endptr, 10);
        if (*endptr != '\0') {
          printf("Invalid score. Please enter a number.\n");
          break;
        }

        ScoreSubmitResponse score_resp;
        if (send_score_submit_request(score_to_submit, &score_resp) == 0) {
          printf("Score submission %s: %s\n", score_resp.success ? "successful" : "failed", score_resp.message);
        } else {
          printf("Error sending score submission request.\n");
        }
        break;

      case 4:  // View Leaderboard
        LeaderboardResponse lb_resp;
        if (send_leaderboard_request(&lb_resp) == 0) {
          display_leaderboard(&lb_resp);
        } else {
          printf("Error sending leaderboard request.\n");
        }
        break;

      case 5:  // Logout
        LogoutResponse logout_resp;
        if (send_logout_request(&logout_resp) == 0) {
          printf("Logout %s: %s\n", logout_resp.success ? "successful" : "failed", logout_resp.message);
        } else {
          printf("Error sending logout request.\n");
        }
        break;

      case 0:  // Exit
        printf("Exiting test client.\n");
        disconnect_from_server();
        return 0;

      default:
        printf("Invalid choice. Please try again.\n");
    }
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // 현재 라인의 나머지 문자 소비
    if (choice != 0) {
      while ((c = getchar()) != '\n' && c != EOF);  // "Press Enter" 대기
    }
  }

  disconnect_from_server();
  return 0;
}