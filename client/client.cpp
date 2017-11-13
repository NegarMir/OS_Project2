#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#define BUFSIZE 1024
using namespace std;

void run(char* ip, int portNum);
void on_new_message(int sockfd);
void send_message(int sockfd);

int main(int argc, char* argv[])
{
    
    if(argc != 3){

            cout<<"valid operation : [ip address] [port]"<<endl;
            exit(-1);
        }


    int portNum = atoi(argv[2]); 
    char* ip = argv[1];

    run(ip,portNum);
    
    
}


void run(char* ip, int portNum){

    bool isExit = false;
    char buffer[BUFSIZE];
    int client;
   

    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);


    if (client < 0) 
    {
        cout << "Error establishing socket" << endl;
        exit(1);
    }

    cout << "=> Socket client has been created..." << endl;


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);



    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        cout << "=> Connection to the server port number: " << portNum << endl;

    on_new_message(client);
    send_message(client);





    close(client);
    return ;
}

void on_new_message(int sockfd){

    char buffer[BUFSIZE];

    if((recv(sockfd, buffer, BUFSIZE, 0))<0){
        cout <<"RECV ERROR\n";
        exit(-1);
    }
    else 
        cout<<"NEW MESSAGE FROM SERVER : "<<buffer<<endl;

    return ;


}


void send_message(int sockfd){

    char buffer[BUFSIZE];
    cout<<"Enter your car's id:";
    cin>>buffer;
    if((send(sockfd, buffer, BUFSIZE, 0))<0)
        {
            cout<<"SENDING FAILED"<<endl;
            exit(-1);
        }
    return;


}