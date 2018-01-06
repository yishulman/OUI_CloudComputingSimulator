CC=gcc
LDFLAGS=-lpthread

all:
	$(CC) server.c -o cloudsrv $(LDFLAGS)
	$(CC) client.c -o cloudcli $(LDFLAGS)
clean:
	rm -f *.o cloudsrv cloudcli
