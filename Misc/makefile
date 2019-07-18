# TODO: disable debugger

all : server.exe client.exe

server.exe: hashtable.o rwlock.o server.o
	gcc -o server.exe server.o hashtable.o rwlock.o -Wall -pthread -gdb

server.o: server.c buffer.h
	gcc -c server.c

hashtable.o: hashtable.c
	gcc -c hashtable.c

rwlock.o: rwlock.c
	gcc -c rwlock.c

client.exe: client.c buffer.h
	gcc -o client.exe client.c -Wall -pthread

clean:
	rm rwlock.o hashtable.o server.exe client.exe server.o
 
