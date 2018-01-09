CC=gcc
LDFLAGS=-lpthread
INCLUDE=-Iinclude
GTEST_PATH=../googletest/googletest
GTEST_LIB=$(GTEST_PATH)/make/gtest_main.a
TESTS_INCLUDE=-I$(GTEST_PATH)/include
TEST_FILES=tests/*.cc
CCT=g++
SRC_FILES=src/*.c

all:
	$(CC) $(INCLUDE) $(SRC_FILES) -c
	$(CC) $(INCLUDE) server_main.c server.o -o cloudsrv.elf $(LDFLAGS)
	$(CC) $(INCLUDE) client_main.c client.o -o cloudcli.elf $(LDFLAGS)
	$(CCT) $(TESTS_INCLUDE) $(GTEST_LIB) $(TEST_FILES) *.o -o run_tests.elf $(LDFLAGS)
clean:
	rm -f *.o *.elf
