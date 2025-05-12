#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_ID_LEN 20
#define MAX_PW_LEN 20
#define MAX_MSG_LEN 100
#define MAX_LEADERBOARD_ENTRIES 10

// 메시지 타입 정의
typedef enum {
  // 클라이언트 -> 서버 요청
  MSG_TYPE_REGISTER_REQ,
  MSG_TYPE_LOGIN_REQ,
  MSG_TYPE_SCORE_SUBMIT_REQ,
  MSG_TYPE_LEADERBOARD_REQ,
  MSG_TYPE_LOGOUT_REQ,  // 선택적: 로그아웃 기능

  // 서버 -> 클라이언트 응답
  MSG_TYPE_REGISTER_RESP,
  MSG_TYPE_LOGIN_RESP,
  MSG_TYPE_SCORE_SUBMIT_RESP,
  MSG_TYPE_LEADERBOARD_RESP,
  MSG_TYPE_LOGOUT_RESP,  // 선택적

  MSG_TYPE_ERROR  // 일반 오류 응답
} MessageType;

// 공통 헤더 (모든 메시지의 시작 부분)
typedef struct {
  MessageType type;
  int length;  // type을 제외한 데이터 부분의 길이
} MessageHeader;

// --- 요청 구조체 ---
typedef struct {
  char username[MAX_ID_LEN];
  char password[MAX_PW_LEN];
} RegisterRequest;

typedef struct {
  char username[MAX_ID_LEN];
  char password[MAX_PW_LEN];
} LoginRequest;

typedef struct {
  // char username[MAX_ID_LEN]; // 로그인된 사용자로 간주하거나, 세션 ID 등으로 대체 가능
  int score;
} ScoreSubmitRequest;

// LeaderboardRequest는 특별한 데이터가 없을 수 있음 (단순 요청)
// typedef struct {} LeaderboardRequest;

// --- 응답 구조체 ---
typedef struct {
  int success;  // 0: 실패, 1: 성공
  char message[MAX_MSG_LEN];
} RegisterResponse;

typedef struct {
  int success;  // 0: 실패, 1: 성공
  char message[MAX_MSG_LEN];
  // int user_id; // 성공 시 사용자 ID (선택적)
} LoginResponse;

typedef struct {
  int success;  // 0: 실패, 1: 성공
  char message[MAX_MSG_LEN];
} ScoreSubmitResponse;

typedef struct {
  char username[MAX_ID_LEN];
  int score;
} LeaderboardEntry;

typedef struct {
  int count;  // 실제 리더보드 항목 수
  LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
  char message[MAX_MSG_LEN];  // (예: "리더보드 데이터 없음")
} LeaderboardResponse;

typedef struct {
  int success;
  char message[MAX_MSG_LEN];
} LogoutResponse;  // 선택적

typedef struct {
  char message[MAX_MSG_LEN];
} ErrorResponse;

// 개발자 B가 사용할 auth.h, score_manager.h의 함수 프로토타입 (임시)
// 실제로는 해당 파일에 정의되어야 함
#ifndef AUTH_H_TEMP  // 임시 중복 방지
#define AUTH_H_TEMP
// int register_user_impl(const char* username, const char* password, char* response_msg);
// int login_user_impl(const char* username, const char* password, char* response_msg);
#endif

#ifndef SCORE_MANAGER_H_TEMP  // 임시 중복 방지
#define SCORE_MANAGER_H_TEMP
// int submit_score_impl(const char* username, int score, char* response_msg);
// void get_leaderboard_impl(LeaderboardEntry* entries, int* count, int max_entries);
#endif

#endif  // PROTOCOL_H