CC = gcc
CFLAGS = -Wall -g -pthread # -pthread는 클라이언트 컴파일에는 필요 없을 수 있으나, network_client.c가 스레드 관련 기능을 쓴다면 유지
LDFLAGS = -lpthread # 서버 링크 시 필요

COMMON_INCLUDE_DIR = common/include
SERVER_INCLUDE_DIR = server/include
CLIENT_INCLUDE_DIR = client/include

# 서버 소스 파일 목록 (실제 존재하는 파일들)
SERVER_REAL_SRC_FILES = main_server.c network_server.c
SERVER_REAL_SRC = $(addprefix server/src/, $(SERVER_REAL_SRC_FILES))

# 개발자 B가 제공할 (또는 임시로 생성될) 서버 소스 파일 목록
SERVER_DUMMY_SRC_FILES = auth.c score_manager.c db_handler.c
SERVER_DUMMY_SRC = $(addprefix server/src/, $(SERVER_DUMMY_SRC_FILES))

# 모든 서버 소스 파일 (실제 + 더미)
SERVER_SRC = $(SERVER_REAL_SRC) $(SERVER_DUMMY_SRC)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
SERVER_EXEC = rain_server

# 클라이언트 파일 정의
CLIENT_NET_SRC = client/src/network_client.c
CLIENT_NET_OBJ = $(CLIENT_NET_SRC:.c=.o)

# 테스트 클라이언트 파일 정의 (새로 추가)
TEST_CLIENT_SRC = client/src/main_test_client.c
TEST_CLIENT_OBJ = $(TEST_CLIENT_SRC:.c=.o)
TEST_CLIENT_EXEC = rain_test_client


# 헤더 파일 정의
HEADERS_TO_CREATE = $(SERVER_INCLUDE_DIR)/auth.h $(SERVER_INCLUDE_DIR)/score_manager.h $(SERVER_INCLUDE_DIR)/db_handler.h
ALL_SERVER_HEADERS = $(HEADERS_TO_CREATE) $(SERVER_INCLUDE_DIR)/network_server.h $(COMMON_INCLUDE_DIR)/protocol.h


all: $(SERVER_EXEC) $(TEST_CLIENT_EXEC) # all 타겟에 테스트 클라이언트 추가

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 테스트 클라이언트 실행 파일 생성 규칙 (새로 추가)
$(TEST_CLIENT_EXEC): $(TEST_CLIENT_OBJ) $(CLIENT_NET_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ # 서버와 달리 -lpthread는 보통 필요 없음

# 일반적인 .o 파일 컴파일 규칙 (서버용)
server/src/%.o: server/src/%.c $(ALL_SERVER_HEADERS)
	$(CC) $(CFLAGS) -I$(COMMON_INCLUDE_DIR) -I$(SERVER_INCLUDE_DIR) -I$(CLIENT_INCLUDE_DIR) -c $< -o $@

# 일반적인 .o 파일 컴파일 규칙 (클라이언트용)
client/src/%.o: client/src/%.c
	$(CC) $(CFLAGS) -I$(COMMON_INCLUDE_DIR) -I$(CLIENT_INCLUDE_DIR) -I$(SERVER_INCLUDE_DIR) -c $< -o $@
# 위 client/src/%.o 규칙은 network_client.h, protocol.h 등에 대한 의존성을 명시적으로 추가하는 것이 더 좋음
# 예: client/src/%.o: client/src/%.c $(CLIENT_INCLUDE_DIR)/network_client.h $(COMMON_INCLUDE_DIR)/protocol.h


# --- 임시 헤더 및 소스 파일 생성 규칙들 (이전과 동일) ---

$(SERVER_INCLUDE_DIR)/auth.h:
	@mkdir -p $(SERVER_INCLUDE_DIR)
	@echo "#ifndef AUTH_H" > $@
	@echo "#define AUTH_H" >> $@
	@echo "#include \"protocol.h\"" >> $@
	@echo "void init_auth_system();" >> $@
	@echo "int register_user_impl(const char* username, const char* password, char* response_msg);" >> $@
	@echo "int login_user_impl(const char* username, const char* password, char* response_msg, char* logged_in_user);" >> $@
	@echo "#endif" >> $@

$(SERVER_INCLUDE_DIR)/score_manager.h:
	@mkdir -p $(SERVER_INCLUDE_DIR)
	@echo "#ifndef SCORE_MANAGER_H" > $@
	@echo "#define SCORE_MANAGER_H" >> $@
	@echo "#include \"protocol.h\"" >> $@
	@echo "void init_score_system();" >> $@
	@echo "int submit_score_impl(const char* username, int score, char* response_msg);" >> $@
	@echo "void get_leaderboard_impl(LeaderboardEntry* entries, int* count, int max_entries);" >> $@
	@echo "#endif" >> $@

$(SERVER_INCLUDE_DIR)/db_handler.h:
	@mkdir -p $(SERVER_INCLUDE_DIR)
	@echo "#ifndef DB_HANDLER_H" > $@
	@echo "#define DB_HANDLER_H" >> $@
	@echo "void init_db();" >> $@
	@echo "// Add DB function prototypes here" >> $@
	@echo "#endif" >> $@

server/src/auth.c: $(SERVER_INCLUDE_DIR)/auth.h
	@mkdir -p server/src
	@echo "/* Dummy auth.c */" > $@
	@echo "#include \"auth.h\"" >> $@
	@echo "#include <stdio.h> // For printf, snprintf" >> $@
	@echo "#include <string.h> // For strncpy, strcmp" >> $@
	@echo "void init_auth_system() { printf(\"[DUMMY AUTH] Init system\\\\n\"); }" >> $@
	@echo "int register_user_impl(const char* u, const char* p, char* m) { snprintf(m, MAX_MSG_LEN, \"Dummy register OK for %s\", u); return 1; }" >> $@
	@echo "int login_user_impl(const char* u, const char* p, char* m, char* lu) { strncpy(lu, u, MAX_ID_LEN-1); lu[MAX_ID_LEN-1]=0; snprintf(m, MAX_MSG_LEN, \"Dummy login OK for %s\", u); return 1; }" >> $@

server/src/score_manager.c: $(SERVER_INCLUDE_DIR)/score_manager.h
	@mkdir -p server/src
	@echo "/* Dummy score_manager.c */" > $@
	@echo "#include \"score_manager.h\"" >> $@
	@echo "#include <stdio.h> // For printf, snprintf" >> $@
	@echo "#include <string.h> // For strncpy" >> $@
	@echo "void init_score_system() { printf(\"[DUMMY SCORE] Init system\\\\n\"); }" >> $@
	@echo "int submit_score_impl(const char* u, int s, char* m) { snprintf(m, MAX_MSG_LEN, \"Dummy score %d submitted for %s\", s, u); return 1; }" >> $@
	@echo "void get_leaderboard_impl(LeaderboardEntry* e, int* c, int me) { *c = 0; if(me > 0) { strncpy(e[0].username, \"dummy\", MAX_ID_LEN-1); e[0].username[MAX_ID_LEN-1]=0; e[0].score = 10; *c = 1;} }" >> $@

server/src/db_handler.c: $(SERVER_INCLUDE_DIR)/db_handler.h
	@mkdir -p server/src
	@echo "/* Dummy db_handler.c */" > $@
	@echo "#include \"db_handler.h\"" >> $@
	@echo "#include <stdio.h> // For printf" >> $@
	@echo "void init_db() { printf(\"[DUMMY DB] Init system\\\\n\"); }" >> $@

clean:
	rm -f $(SERVER_EXEC) $(SERVER_OBJ)
	rm -f $(TEST_CLIENT_EXEC) $(TEST_CLIENT_OBJ) $(CLIENT_NET_OBJ) # 클린 타겟에 테스트 클라이언트 관련 파일 추가
	rm -f $(addprefix server/src/, $(SERVER_DUMMY_SRC_FILES))
	rm -f $(HEADERS_TO_CREATE)

.PHONY: all clean