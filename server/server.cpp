#include <iostream>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>

#define BUFSIZE 1024

using namespace std;
string file_path;

void run(char* ip_addr, char* port);
void on_new_message(int sockfd, char buff[], int valread);
void send_message(int sockfd);
void on_terminated_connection(int sockfd);
void parse(char input[],int sockfd);
void calculate_total_penalty(char id[]);




int main(int argc, char* argv[])
{
    if( argc != 3)
        {
            cout<< "valid operation : [ip address] [port number]\n";
            exit(-1);
        }
    else
    {
        cout<<"Enter the file's address:";
        cin>>file_path;

        char* Port = argv[2];
        char* ip_addr = argv[1];
        run(argv[1], argv[2]);
    }

}


void run(char* ip, char* port)
{
    int portNum = atoi(port);
    //int ip_addr = atoi
    int sockfd, new_socket;
    bool isExit = false;
    char buffer[BUFSIZE];

    struct sockaddr_in server_addr;
    socklen_t size;

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        cout << "CONNECTION NOT STABLISHED\n" << endl;
        exit(1);
    }



    cout << "SOCKET CREATED" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);




    if ((bind(sockfd, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0)
    {
        cout << "SOCKET BIND EXCEPTION" << endl;
        exit(-1);
    }

    size = sizeof(server_addr);
    cout << "WAITING FOR CONNECTION" << endl;


    if(listen(sockfd, 1) < 0)
        cout<<"SOCKET LISTEN EXCEPTION"<< endl;


    int clientCount = 1;


    while (1){

        new_socket = accept(sockfd ,(struct sockaddr *)&server_addr,&size);
        if (new_socket < 0)
            {
              cout << "SOCKET ACCEPT EXCEPTION\n" << endl;
              exit(-1);
            }

        else
            cout << "NEW CONNECTION : " << new_socket<<endl;



        strcpy(buffer, "#CONNECTION STABLISHED\n");
        int valread;
        if ((valread = read( new_socket , buffer, 1024)) == 0){

                    on_terminated_connection(new_socket);
                    close(new_socket);
                }

                //HANDLE MESSAGE
                else
                {
                    on_new_message(new_socket, buffer, valread);
                }


}

}

void recv_message(int sockfd){


}

void on_terminated_connection(int sockfd){

    cout<<"TERMINATED_CONNECTION : "<<sockfd<<endl;
    return ;
}

void on_new_message(int sockfd, char buff[], int valread){

    cout<<"NEW MESSAGE FROM"<< sockfd <<" : "<<buff;

    parse(buff, sockfd);

}

void parse(char input[],int sockfd){

     char *token = strtok(input, ",");
     char id[BUFSIZE];
     if(!strcmp(token,"carid"))
     {
        token = strtok(NULL, ",");
        memcpy(id, token ,strlen(token));
        cout<<"id is:"<<id;
        calculate_total_penalty(id);
        
     }

}


void calculate_total_penalty(char id[]){



}




