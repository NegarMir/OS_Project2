all: client

client: client.o
	g++ client.o -o client && echo success
client.o: client.cpp client.h
	g++ -c client.cpp && echo success
clean:
	rm *.o client
