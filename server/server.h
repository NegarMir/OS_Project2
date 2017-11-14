#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>
#include <sys/time.h> ////FD_SET, FD_ISSET, FD_ZERO macros 
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/wait.h>
#include <sstream>



struct server_info{
	char* ip_addr;
	char* port ; 
	char* file_name;
	char* part;

};

char* itoa (int i, char b[]);
void *get_in_addr(struct sockaddr *sa);
int parameter_err(int);
void run(char* PORT, char* ip_addr);
void send_msg (int indentifier, char* msg);
char recv_msg(int indentifier);
void stop ();
void on_new_connection(int indentifier);
void on_standard_input(char* line);
void on_terminated_connection(int sockfd);
void on_new_message (int indentifier, char[], int valread);
void parse (char [], int sockfd);
int read_file(std::string path, std::string car_id);
bool has_suffix(const std::string& s, const std::string& suffix);
int mystoi(std::string number);
std::string itos(int number);
int search_dirs(std::string file_path, std::string car_id);
std::vector<std::string> split(std::string, char);
std::vector<std::string> find_dirs_and_files(std::string path);
