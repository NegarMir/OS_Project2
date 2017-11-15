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
  char buf[MAXDATASIZE];
  if (parameter_err(argc)) return 0 ;
  char*  PORT = argv[2] ;
  char* ip_addr = argv[1];
  run(PORT, ip_addr);

  return 0;
}

char* itoa(int i, char b[]) {
  char const digit[] = "0123456789";
  char* p = b;
  if (i < 0) {
    *p++ = '-';
    i *= -1;
  }
  int shifter = i;
  do { //Move to where representation ends
    ++p;
    shifter = shifter / 10;
  } while (shifter);
  *p = '\0';
  do { //Move back, inserting digits as u go
    *--p = digit[i % 10];
    i = i / 10;
  } while (i);
  return b;
}

int parameter_err(int argc) {

  if (argc == 2 || argc == 1) {
    const char msg[] = "\n valid operation server [ip_address] [port number]\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    return 1;
  }
  return 0 ;
}

void send_msg (int indentifier, char* msg)
{

  send(indentifier, msg, strlen(msg) + 1, 0);
}

char recv_msg(int indentifier)
{
  char buff[MAXDATASIZE];
  recv(indentifier, buff, MAXDATASIZE, 0);
  return *buff;
}

void stop() {
  char msg[] = "Server is already stopped.\n";
  if (!active)
    write(STDERR_FILENO, msg, strlen(msg));
      active = 0 ;
}

void on_new_connection(int indentifier) {
  char* msg = "\nNEW CONNECTION : ";
  char tmp[5] = {0x0} ;
  write(STDOUT_FILENO, msg, strlen(msg));
  sprintf(tmp, "%4d", indentifier);
  write(STDOUT_FILENO, tmp, strlen(tmp));
}

void on_standard_input(char line[], int size)
{
  char* msg = "STDIN : " ;
  string  quit = ":q";
  write(STDOUT_FILENO, msg, sizeof(msg));
  write(STDOUT_FILENO, line, size);
  write(STDOUT_FILENO,"\n", 1);
  
  string input = line ;
  if (line == quit ){
    cout<<"stop"<<endl;
    stop();
  }
}

void on_new_message(int sockfd, char buf[], int valread) {

  int numbytes;
  //cout<<"\nvalread "<<valread<<endl;
  char* temp = "\nNEW MESSAGE FROM";
  char id[5] = {0x0} ;
  sprintf(id, "%4d", sockfd);
  write(STDOUT_FILENO, temp, strlen(temp));
  write(STDOUT_FILENO, id, strlen(id));
  write(STDOUT_FILENO, ": ", 2);
  write(STDOUT_FILENO, buf, valread);

}

void on_terminated_connection(int sockfd) {
  char* temp = "\nTERMINATED CONNECTION : ";
  char id[5] = {0x0} ;
  sprintf(id, "%4d", sockfd);
  write(STDOUT_FILENO, temp, strlen(temp));
  write(STDOUT_FILENO, id, strlen(id));
}

void make_p2_ready(){

  int fd1;
    char * myfifo = "myfifo";
    int buf;
    int msg  = -2;
    //string msg = "close";

        /* create the FIFO (named pipe) */
        mkfifo(myfifo, 0666);

        /* write "Hi" to the FIFO */
        fd1 = open(myfifo, O_WRONLY);
        write(fd1, &msg, sizeof(buf));
        close(fd1);

        /* remove the FIFO */
        unlink(myfifo);
}


void run(char* PORT, char* ip_addr) {


    int st;
    int sv[2];
    int pid;

    /*int temp_pid = fork();

    if(temp_pid > 0){

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) < 0) {
        perror("socketpair");
        exit(1);
    }
    wait(&st);
    exit(temp_pid);
  }*/



  pid_t p2_pid;
  p2_pid = fork();



  if (p2_pid > 0) {


    cout << " Enter the directory path" << endl;
    cin >> file_path;
    int opt = 1;
    
    int master_socket , addrlen , new_socket , activity, i , valread , sd;
    vector<int>client_socket;
    int max_sd = STDIN;
    char buf[MAXDATASIZE];
    struct sockaddr_in address;

    fd_set readfds;
    char *message = "#CONNECTION STABLISHED\n";

    /*for (i = 0; i < BACKLOG; i++) {
      client_socket[i] = 0 ;
    }*/

    if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
      write(STDERR_FILENO, "socket failed", 13);
      exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if ( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
      write(STDERR_FILENO, "setsockopt", 10);
      exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_addr);
    int port = atoi(PORT);
    address.sin_port = htons( port );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
      write(STDERR_FILENO, "bind failed\n", 12);
      exit(EXIT_FAILURE);
    }


    write(STDOUT_FILENO, "Listener on port ", 17);
    write(STDOUT_FILENO, PORT, strlen(PORT));
    write(STDOUT_FILENO, "\n", 1);


    if (listen(master_socket, 4) < 0) {

      write(STDOUT_FILENO, "listen", 6);
      exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    write(STDOUT_FILENO, "Waiting for connections ...\n", 29);

    //**//
    client_socket.push_back(master_socket);

    while (active)
    {

      FD_ZERO(&readfds);   //clear the socket set
      FD_SET(/*master_socket*/ STDIN, &readfds);  //add master socket to set


      int max_sd = /*master_socket*/ STDIN;

      //add child sockets to set
      for ( i = 0 ; i < client_socket.size() ; i++) {
        //sd = client_socket[i];  //socket descriptor
        //if (sd > 0) //if valid socket descriptor then add to read list
          FD_SET( /*sd*/ client_socket[i], &readfds);

        //if (sd > max_sd)  //highest file descriptor number, need it for the select function
          max_sd =max(client_socket[i], client_socket[i]) ;
      }


      //activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); //wait for an activity on one of the sockets
      int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

      if ((activity < 0) && (errno != EINTR)) {
        write(STDERR_FILENO, "select error", 12);
        exit(0);
      }
      bool flag = false;

      if (FD_ISSET(STDIN, &readfds)) {

        char buf[MAXDATASIZE];

        int size = read(STDIN_FILENO, buf, sizeof(buf));

        if(size  > 0){
        buf[size -1 ] = '\0';

        on_standard_input(buf, size);
        flag = true;
        //continue;
      }

      }


      pid_t pid;
      int status;
      int fd[2];
      pipe(fd);
      pid = fork();
      
      if (pid > 0) {

        //wait(&status);
        //for (i = 0; i < client_socket.size(); i++)
        //{
        if (FD_ISSET(client_socket[0], &readfds) && !flag)  /*&& !flag*/ //If something happened on the master socket,then its an incoming connection
        {
          if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            write(STDOUT_FILENO, "accept err", 10);
            exit(EXIT_FAILURE);
          }
          on_new_connection(new_socket);

          pid_t temp_pid = fork();

         if(temp_pid > 0){

           
            wait(&st);
            exit(temp_pid);
           }

          if(temp_pid == 0){

           if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) < 0) {
            perror("socketpair");
            exit(1);
            }

           
           close(sv[1]);
          
           make_p2_ready();
           parent(sv[0], new_socket);
           exit(temp_pid);
         }
        
          send_msg(new_socket, message);

          //add new socket to array of sockets
          
          client_socket.push_back(new_socket);

        }

        //else its some IO operation on some other socket

        for (i = 1; i < client_socket.size(); i++)
        {
          memset(buf, 0, sizeof(buf));
          sd = client_socket[i];

          if (FD_ISSET( sd , &readfds)) {

            char buf[MAXDATASIZE];
            memset(buf, 0, sizeof(buf));
            if ((valread = read( sd , buf, 1024)) == 0) {
              on_terminated_connection(sd);
              close( sd );
              client_socket.erase(client_socket.begin() + i);
            }

            //HANDLE MESSAGE
            else {

              on_new_message(sd, buf, valread);
              close(fd[0]);
       
              write(fd[1], buf, valread);
              close(fd[1]);
            }
          }
        }

      }


      else if (pid == 0) {

       // while(1){

        close(fd[1]);
        char buf[MAXDATASIZE];

        
        int size = read(fd[0], buf, sizeof(buf));
       
        string input = buf;
      
        if(size > 0){


    
        int ans = search_dirs(file_path, input);
 
        

        int fd1;
        char * myfifo = "myfifo";

        /* create the FIFO (named pipe) */
        mkfifo(myfifo, 0666);

        /* write "Hi" to the FIFO */
        fd1 = open(myfifo, O_WRONLY);
        write(fd1, &ans, sizeof(ans));
        close(fd1);

        /* remove the FIFO */
        unlink(myfifo);
        //break;   
      }
        
     // }
      exit(pid);
    }
  }

    //is not active anymore
    //message p2 to close
    int fd1;
    char * myfifo = "myfifo";
    int msg  = -1;
    //string msg = "close";

        /* create the FIFO (named pipe) */
        mkfifo(myfifo, 0666);

        /* write "Hi" to the FIFO */
        fd1 = open(myfifo, O_WRONLY);
        write(fd1, &msg, sizeof(buf));
        close(fd1);

        /* remove the FIFO */
        unlink(myfifo);



  }

  if (p2_pid == 0) {

    
    
    int sock_fd ;
        

    int fd2;
    //bool closed = false;
    char * myfifo = "myfifo";
    
    int buf;

    while (1) {

      int buf;
      /* open, read, and display the message from the FIFO */
      fd2 = open(myfifo, O_RDONLY);
      int size = (read(fd2, &buf, sizeof(buf)) );
      

        if(size  > 0){
           if(buf == -1){
            cout<<"closing p2";
            close(fd2);
            break;
        }
        else if(buf == -2){
           
           //sleep(1);
           close(sv[0]);
           sock_fd = child(sv[1]);
           cout<<" got fd :"<<sock_fd<<endl;


        }
        else{

        printf("\n Received: %d\n", buf);
        close(fd2);
       }
    }

    }

    exit(p2_pid);
  }
}



int search_dirs(string file_path, string car_id) {

  int status;
  long long int sum = 0;

  vector<string> dirs_and_files = find_dirs_and_files(file_path);



  for ( int i = 0; i < dirs_and_files.size(); i++ ) {

    int fd[2];
    pipe(fd);
    long long int res = 0;
    pid_t pid = fork();

    if (pid == 0) {

      if (has_suffix(dirs_and_files[i], ".txt")) {

        close(fd[0]);

        res = read_file(dirs_and_files[i], car_id);

        write(fd[1], &res, sizeof(res));
        close(fd[1]);


        exit(pid);
      }


      else {

        close(fd[0]);
        long long int res = search_dirs(dirs_and_files[i], car_id);

        write(fd[1], &res, sizeof(res) );
        close(fd[1]);

        exit(pid);

      }


    }

    if (pid > 0) {
      //cout<<"parent\n";
      wait(&status);
      close(fd[1]);
      long long int input;
      read(fd[0], &input, sizeof(input));
      //cout<<"input"<<input<<endl;
      sum += (input);
      //cout<<"current sum"<<sum<<endl;
      close(fd[0]);

    }

  }

  return sum;


}

vector<string> split(string statement, char delimeter) {

  vector<string> result;
  string token;

  for (int i = 0; i < statement.length(); i++)
    if (statement[i] != delimeter)
      token += statement[i];
    else if (token.length()) {
      result.push_back(token);
      token = "";
    }

  if (token.length())
    result.push_back(token);
  return result;
}

int read_file(string path, string car_id) {

  ifstream myfile;

  //cout<<"path is : "<<path<<endl;
  //cout<<"car id is" << car_id<<endl;
  myfile.open(path.c_str());
  if (!myfile)
    exit(0);
  int res = 0;
  string line;

  while (getline(myfile, line)) {

    vector<string> token = split(line, ' ');

    if (token[0] == car_id)
      res += mystoi(token[2]);
  }
  myfile.close();
  //cout<<"calc "<<res<<endl;
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


vector<string> find_dirs_and_files(string path) {

  dirent* ent;
  DIR* dir;
  vector<string> res;
  struct stat dir_stat;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {


      string file_path = path;
      file_path += "/";
      file_path += ent->d_name;
      if (stat(path.c_str(), &dir_stat) == 0) {

        if ( dir_stat.st_mode & S_IFDIR ) {

          if (strncmp(ent->d_name, "." , sizeof(ent->d_name)) != 0 && strncmp(ent->d_name, "..", sizeof(ent->d_name)) != 0)

          {
            if (strncmp(ent->d_name, ".DS_Store", sizeof(ent->d_name)) != 0)
              res.push_back(file_path);

          }
        }

      }


    }

    closedir(dir);
  }
  return res;
}


ssize_t
sock_fd_write(int sock, char *buf, ssize_t buflen, int fd)
{
    ssize_t     size;
    struct msghdr   msg;
    struct iovec    iov;
    union {
        struct cmsghdr  cmsghdr;
        char        control[CMSG_SPACE(sizeof (int))];
    } cmsgu;
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        printf ("passing fd %d\n", fd);
        *((int *) CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        printf ("not passing fd\n");
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0)
        perror ("sendmsg");
    return size;
}


ssize_t
sock_fd_read(int sock, char *buf, ssize_t bufsize, int *fd)
{
    ssize_t     size;

    if (fd) {
        struct msghdr   msg;
        struct iovec    iov;
        union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        } cmsgu;
        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg (sock, &msg, 0);
        if (size < 0) {
            perror ("recvmsg");
            exit(1);
        }
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET) {
                fprintf (stderr, "invalid cmsg_level %d\n",
                     cmsg->cmsg_level);
                exit(1);
            }
            if (cmsg->cmsg_type != SCM_RIGHTS) {
                fprintf (stderr, "invalid cmsg_type %d\n",
                     cmsg->cmsg_type);
                exit(1);
            }

            *fd = *((int *) CMSG_DATA(cmsg));
            printf ("received fd %d\n", *fd);
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0) {
            perror("read");
            exit(1);
        }
    }
    return size;
}

int 
child(int sock)
{
    int fd;
    char    buf[16];
    ssize_t size;

    sleep(1);
    for (;;) {
        size = sock_fd_read(sock, buf, sizeof(buf), &fd);
        if (size <= 0)
            break;
        printf ("read %d\n", size);
        /*if (fd != -1) {
            write(fd, "hello, world\n", 13);
            close(fd);
        }*/

        return fd;
    }
}

void
parent(int sock, int fd)
{
    ssize_t size;
    int i;
    
    size = sock_fd_write(sock, "1", 1, fd);
    printf ("wrote %d\n", size);
}



