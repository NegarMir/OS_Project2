all: server

server: server.o
	g++  server.o -o server && echo success

server.o : server.cpp server.h
	g++ -c  server.cpp  && echo success 

clean:
	rm *.o server 
