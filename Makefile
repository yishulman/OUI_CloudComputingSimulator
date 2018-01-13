CC=gcc
#CFLAGS=-g3
LDFLAGS=-lpthread
INCLUDE=-Iinclude
SRC_FILES=src/*.c
OBJ_FILES=*.o

CCT=g++
GTEST_PATH=../googletest/googletest
GTEST_LIB=$(GTEST_PATH)/make/gtest_main.a
TESTS_INCLUDE=-I$(GTEST_PATH)/include
TEST_FILES=tests/*.cc

all:
	# compile
	$(CC) $(INCLUDE) $(SRC_FILES) $(CFLAGS) -c
	# link
	$(CC) $(INCLUDE) server_main.c $(OBJ_FILES) -o cloudsrv.elf $(LDFLAGS)
	$(CC) $(INCLUDE) job_main.c $(OBJ_FILES) -o run_job.elf $(LDFLAGS)
	$(CC) $(INCLUDE) resource_main.c $(OBJ_FILES) -o run_resource.elf $(LDFLAGS)
	# unit tests
	$(CCT) $(INCLUDE) $(TESTS_INCLUDE) $(GTEST_LIB) $(TEST_FILES) $(OBJ_FILES) -o run_tests.elf $(LDFLAGS)
clean:
	rm -f *.o *.elf
