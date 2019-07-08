all : hashtable.exe

hashtable.exe: hashtable.o rwlock.o
	gcc -o hashtable.exe hashtable.o rwlock.o -Wall -pthread -gdb

hashtable.o: hashtable.c
	gcc -c hashtable.c

#rwlock.exe: rwlock.o
#	gcc -o rwlock.exe rwlock.o -Wall -pthread

rwlock.o: rwlock.c
	gcc -c rwlock.c

clean:
	rm rwlock.o hashtable.o hashtable.exe
	# rm rwlock.o rwlock.exe 
