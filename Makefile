all:
	gcc server.c -o cloudsrv
	gcc client.c -o cloudcli
clean:
	rm -f *.o cloudsrv cloudcli
