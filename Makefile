CC=g++
CFLAGS=-std=c++11

all : mytree mytime mymtimes mytimeout terminal

terminal: mainterminal.cpp
	$(CC) $(CFLAGS) -o terminal mainterminal.cpp

mytree: mytree.cpp
	$(CC) $(CFLAGS) -o mytree mytree.cpp

mytime: mytime.cpp
	$(CC) $(CFLAGS) -o mytime mytime.cpp

mymtimes: mymtimes.cpp
	$(CC) $(CFLAGS) -o mymtimes mymtimes.cpp

mytimeout: mytimeout.cpp
	$(CC) $(CFLAGS) -o mytimeout mytimeout.cpp

run:
	mytree
	mytime
	mymtimes
	mytimeout
	terminal

clean:
	rm -f mytree mytime mymtimes mytimeout terminal
