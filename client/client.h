#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>


void *get_in_addr(struct sockaddr *sa);
char *append_str(char *str1 , char *str2);
int port_ip_err(int argc);
void send_msg(char* message, int sockfd);
void close_sock(int sockfd);
int connect_server (char* ip_addr , char* port);
void get_id();


#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define MAXNOSERVERS 15