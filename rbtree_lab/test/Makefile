.PHONY: all test run valgrind clean

CC       := gcc
CPPFLAGS := -I../src -DSENTINEL         # 헤더 경로/매크로는 여기!
CFLAGS   := -std=c11 -Wall -Wextra -g   # 컴파일 경고/디버그
LDFLAGS  :=
LDLIBS   :=

all: test-rbtree

# 실행 타깃
test: test-rbtree
	./test-rbtree

valgrind: test-rbtree
	valgrind ./test-rbtree

# 테스트 실행 파일 링크 규칙 (명시적으로 써 주기)
test-rbtree: test-rbtree.o ../src/rbtree.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

# src 오브젝트는 src 디렉터리의 Makefile로 빌드
../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o

clean:
	rm -f test-rbtree *.o
	-$(MAKE) -C ../src clean
