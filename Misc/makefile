# TODO: disable debugger

all : server.exe client.exe

server.exe: hashtable.o rwlock.o server.o
	gcc -o server.exe server.o hashtable.o rwlock.o -Wall -pthread -gdb

server.o: server.c
	gcc -c server.c

#hashtable.exe: hashtable.o rwlock.o
#	gcc -o hashtable.exe hashtable.o rwlock.o -Wall -pthread -gdb

hashtable.o: hashtable.c
	gcc -c hashtable.c

#rwlock.exe: rwlock.o
#	gcc -o rwlock.exe rwlock.o -Wall -pthread

rwlock.o: rwlock.c
	gcc -c rwlock.c

client.exe: client.c
	gcc -o client.exe client.c -Wall -pthread

clean:
	rm rwlock.o hashtable.o server.exe client.exe server.o
	# rm rwlock.o rwlock.exe 
