#include "server.h"

#define MAXDATASIZE 1024
#define BACKLOG 10     // how many pending connections queue will hold
#define MAXNOSERVERS 15
int active = 1 ;

std::string file_path;


using namespace std;

int main(int argc, char* argv[])
{


    int counter, numbytes;
    int active = 0 ;
    char buf[MAXDATASIZE];
    if(parameter_err(argc)) return 0 ;
    char*  PORT = argv[2] ;
    char* ip_addr = argv[1];
    run(PORT, ip_addr);

    return 0;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

 int parameter_err(int argc){

    if(argc == 2 || argc == 1){
        const char msg[] = "\n valid operation server [ip_address] [port number]\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return 1;
    }
    return 0 ;
 }

void send_msg (int indentifier, char* msg)
 {

   send(indentifier, msg, strlen(msg)+1, 0);
 }

char recv_msg(int indentifier)
 {
    char buff[MAXDATASIZE];
    recv(indentifier, buff, MAXDATASIZE, 0);
    return *buff;
 }

void stop(){
    char msg[] = "Server is already stopped.\n";
    if(!active)
        write(STDERR_FILENO, msg, strlen(msg));
    active = 0 ;
 }

void on_new_connection(int indentifier){
    char* msg = "\nNEW CONNECTION : ";
    char tmp[5] = {0x0} ;
    write(STDOUT_FILENO, msg, strlen(msg));
    sprintf(tmp,"%4d", indentifier);
    write(STDOUT_FILENO, tmp, strlen(tmp));
 }

void on_standard_input(string line)
{
  string  msg = "STDIN : " ;
  string  quit = ":q";
  cout<<msg<<line<<endl;
  if (line == quit )
    stop();
}

void on_new_message(int sockfd, char buf[], int valread){

    int numbytes;
    cout<<"\nvalread "<<valread<<endl;
    char* temp = "\nNEW MESSAGE FROM";
    char id[5] = {0x0} ;
    sprintf(id,"%4d", sockfd);
    write(STDOUT_FILENO, temp, strlen(temp));
    write(STDOUT_FILENO, id, strlen(id));
    write(STDOUT_FILENO, ": ",2);
    write(STDOUT_FILENO, buf, valread);

}

void on_terminated_connection(int sockfd){
   char* temp = "\nTERMINATED CONNECTION : ";
    char id[5] = {0x0} ;
    sprintf(id,"%4d", sockfd);
    write(STDOUT_FILENO, temp, strlen(temp));
    write(STDOUT_FILENO, id, strlen(id));
}

void parse(char input[],int sockfd){

}


void run(char* PORT, char* ip_addr){



    //pid_t p2_pid;
    //p2_pid = fork();

   //if(p2_pid > 0) {


    cout<<" Enter the directory path"<<endl;
    cin>> file_path;
    int opt = 1;  
    bool active = true;
    int master_socket , addrlen , new_socket , client_socket[30], activity, i , valread , sd;  
    int max_sd;  
    char buf[MAXDATASIZE];
    struct sockaddr_in address;  
         
    fd_set readfds;  
    char *message = "#CONNECTION STABLISHED\n";  
    
    for (i = 0; i < BACKLOG; i++){  
        client_socket[i] = 0;  
    } 
        
   if((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){  
      write(STDERR_FILENO,"socket failed",13);  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){  
        write(STDERR_FILENO,"setsockopt",10); 
        exit(EXIT_FAILURE);  
    }  
    
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = inet_addr(ip_addr);  
    int port = atoi(PORT);
    address.sin_port = htons( port );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0){  
        write(STDERR_FILENO,"bind failed\n",12);  
        exit(EXIT_FAILURE);  
    }  


    write(STDOUT_FILENO,"Listener on port ",17);  
    write(STDOUT_FILENO, PORT, strlen(PORT)); 
    write(STDOUT_FILENO,"\n",1);
    
    
    if (listen(master_socket, 4) < 0){  

        write(STDOUT_FILENO, "listen",6);  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    write(STDOUT_FILENO, "Waiting for connections ...\n", 29); 

    while(active)  
    {   
        
        FD_ZERO(&readfds);   //clear the socket set    
        FD_SET(master_socket, &readfds);  //add master socket to set 
       

        int max_sd = master_socket;  
            
        //add child sockets to set 
        for ( i = 0 ; i < BACKLOG ; i++){  
            sd = client_socket[i];  //socket descriptor 
            if(sd > 0)  //if valid socket descriptor then add to read list 
                FD_SET( sd , &readfds);  
                
            if(sd > max_sd)   //highest file descriptor number, need it for the select function 
                max_sd = sd;  
        }  


       //activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); //wait for an activity on one of the sockets 
         activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
      
        if ((activity < 0) && (errno!=EINTR)){
            write(STDERR_FILENO,"select error", 12);  
        } 

       if (FD_ISSET(STDIN_FILENO, &readfds)){
            cout<<"Here"<<endl;
            string line;
            getline(cin, line);
            on_standard_input(line);
            
       } 
          
          
       // pid_t pid;
        //int fd[2];
       // pipe(fd);
        //pid = fork(); 
        //int status;
        //if(pid > 0){

          //wait(&status);
          if (FD_ISSET(master_socket, &readfds)) //If something happened on the master socket,then its an incoming connection 
            {  
              if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0){  
                 write(STDOUT_FILENO,"accept err", 10);  
                 exit(EXIT_FAILURE);  
                }
                on_new_connection(new_socket);
                send_msg(new_socket, message);
               
              //add new socket to array of sockets 
               for (i = 0; i < BACKLOG; i++)  
               {  
                //if position is empty 
                   if( client_socket[i] == 0 )  
                   {  
                        client_socket[i] = new_socket;  
                        break;  
                   }  
                }
                
           }
            
        //else its some IO operation on some other socket
        
          for (i = 0; i < BACKLOG; i++)  
           {  
              memset(buf, 0, sizeof(buf));
              sd = client_socket[i];  
                
             if (FD_ISSET( sd , &readfds)){  
                    
                    char buf[MAXDATASIZE];
                   memset(buf, 0, sizeof(buf));
                   if ((valread = read( sd , buf, 1024)) == 0){  
                      on_terminated_connection(sd);  
                      close( sd );  
                      client_socket[i] = 0;  
                     }  
                    
                 //HANDLE MESSAGE
                   else{  

                     on_new_message(sd, buf, valread);
                     //close(fd[0]);
                     string message = buf;
                     //cout<<"id "<<message<<endl;
                     cout<<search_dirs(file_path, message)<<endl;
                     //write(fd[1], &message, sizeof(buf));
                     //close(fd[1]);
                  }  
               }  
            }
         
         // }


        //if(pid == 0){

          //close(fd[1]);
          //string input;
          
          //read(fd[0], &input, sizeof(input));
          //cout<<"input is : "<<input<<endl;
          //cout<<input.length()<<endl;
          //close(fd[0]);
          //cout<<"Ans:"<<search_dirs(file_path, input)<<endl;}    
   // }
  }
}
  

  
 




int search_dirs(string file_path,string car_id) {

   int status;
   long long int sum = 0;

   vector<string> dirs_and_files = find_dirs_and_files(file_path);



   for( int i = 0; i < dirs_and_files.size(); i++ ) {

    int fd[2];
    pipe(fd);
    long long int res = 0;
    pid_t pid = fork();

    if(pid == 0){

      if(has_suffix(dirs_and_files[i],".txt")){

        close(fd[0]);

        res = read_file(dirs_and_files[i],car_id);

        write(fd[1], &res, sizeof(res));
        close(fd[1]);
      

       exit(pid);
      }


      else{
   
        close(fd[0]);
        long long int res = search_dirs(dirs_and_files[i],car_id);

        write(fd[1], &res, sizeof(res) );
        close(fd[1]);

        exit(pid);

      }


    }

     if(pid > 0){
      //cout<<"parent\n";
      wait(&status);
      close(fd[1]);
      long long int input;
      read(fd[0], &input, sizeof(input));
      //cout<<input<<endl;
      sum += (input);
      //cout<<"current sum"<<sum<<endl;
      close(fd[0]);

    }

}

return sum;


}

vector<string> split(string statement, char delimeter){

    vector<string> result;
    string token;

    for(int i=0; i<statement.length(); i++)
        if(statement[i] != delimeter)
            token += statement[i];
        else if(token.length()) {
            result.push_back(token);
            token = "";
        }

    if(token.length())
        result.push_back(token);
    return result;
}

int read_file(string path, string car_id){

  ifstream myfile;
  //cout<<"path is : "<<path<<endl;
  myfile.open(path.c_str());
  if(!myfile)
    exit(0);
  int res = 0;
  string line;

  while(getline(myfile, line)){

    vector<string> token = split(line, ' ');

    if(token[0] == car_id)
      res += mystoi(token[2]);
  }
  myfile.close();
  ///cout<<"calc "<<res<<endl;
  return res;
}


int mystoi(string number) {

  int n;
  stringstream ss;
  ss << number;
  ss >> n;
  return n;
}

bool has_suffix(const string& s, const string& suffix)
{
    return (s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

string itos(int number) {
  ostringstream oss;
  oss << number;
  return oss.str();
}


vector<string> find_dirs_and_files(string path){

   dirent* ent;
   DIR* dir;
   vector<string> res;
   struct stat dir_stat;
   if ((dir = opendir (path.c_str())) != NULL) {
   while ((ent = readdir (dir)) != NULL) {


      string file_path = path;
      file_path+= "/";
      file_path += ent->d_name;
            if (stat(path.c_str(), &dir_stat) == 0) {

                if ( dir_stat.st_mode & S_IFDIR ) {

                    if (strncmp(ent->d_name, "." , sizeof(ent->d_name)) != 0 && strncmp(ent->d_name, "..", sizeof(ent->d_name)) != 0)

                    {
                      if(strncmp(ent->d_name, ".DS_Store", sizeof(ent->d_name))!= 0)
                      res.push_back(file_path);

                    }
                }

            }


        }

       closedir(dir);
}
  return res;
}


