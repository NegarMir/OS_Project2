#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

#define BUFSIZE 1024


using namespace std;


int read_file(string path, string car_id);
bool has_suffix(const string& s, const string& suffix);
int mystoi(string number);
string itos(int number);
int search_dirs(string file_path, string car_id);
vector<string> split(string, char);
vector<string> find_dirs_and_files(string path);


int main() {

    string path;
    string carid = "12345";
    cout << "Enter the directory's path:";
    cin >> path;
    //cout << "path is : " << path << endl;
    int fd[2];
    pipe(fd);

        cout<< search_dirs(path,carid) << endl;

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
      cout<<"current sum"<<sum<<endl;
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
  cout<<"\n"<<path<<"   "<< car_id << endl;
  myfile.open(path.c_str());
  if(!myfile)
    exit(0);
  int res = 0;
  string line;
  cout<<"reading"<<endl;
  while(getline(myfile, line)){

    vector<string> token = split(line, ' ');

    if(token[0] == car_id)
      res += mystoi(token[2]);
  }
  myfile.close();
  cout<<"calc"<<res<<endl;
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



