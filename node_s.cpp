#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <string.h>
#include<string>
#include<thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>



using namespace std;
char filename[100000];
int c;

class node
{
    int PORT;

    public:
    node(int p){
        PORT = p;
    }

    void server(){
        int sockfd, ret;
        struct sockaddr_in serverAddr;

        int newSocket;
        struct sockaddr_in newAddr;

        socklen_t addr_size;

        char buffer[1024];
        pid_t childpid;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0){
            printf("[-]Error in connection.\n");
            exit(1);
        }
        printf("[+]Server Socket is created.\n");

        memset(&serverAddr, '\0', sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret < 0){
            printf("[-]Error in binding.\n");
            return;
        }
        printf("[+]Bind to port %d\n", PORT);

        if(listen(sockfd, 10) == 0){
            printf("[+]Listening....\n");
        }else{
            printf("[-]Error in binding.\n");
        }


        while(1){
            newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
            if(newSocket < 0){
                return;
            }
            printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

            if((childpid = fork()) == 0){
                close(sockfd);

                while(1){
                     char recievedstring[1000000];
                     char c[1];
                     char fname[100000];
                     recv(newSocket,c,1,0);
                     if(c[0]=='1')
                     { 
                         recv(newSocket,fname,10000,0);
                         recv(newSocket,recievedstring,100000,0);
                         ofstream myfile;
                         myfile.open(fname);
                         myfile<<recievedstring;

                     }
                     else if(c[0]=='2')
                     {
                         recv(newSocket,fname,10000,0);
                          std::ifstream ifs(fname);
                          std::string content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
                          char sending[100000];
                          for(int i=0;i<content.length();i++){sending[i]=content[i];}
                     
                          send(newSocket, sending, 100000, 0);

                     }

                    }
                }
            }

        

        close(newSocket);}
    

    void client(int port){
        int clientSocket, ret;
        struct sockaddr_in serverAddr;
        char buffer[1024];

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(clientSocket < 0){
            printf("[-]Error in connection.\n");
            exit(1);
        }
        printf("[+]Client Socket is created.\n");

        memset(&serverAddr, '\0', sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret < 0){
            printf("[-]Error in connection.\n");
            exit(1);
        }
        printf("[+]Connected to Server.\n");

        while(1){
            if(c==1)
            {std::ifstream ifs(filename);
            std::string content( (std::istreambuf_iterator<char>(ifs) ),(std::istreambuf_iterator<char>()    ) );
            char sending[100000];
            for(int i=0;i<content.length();i++){sending[i]=content[i];}
            char rand[1];
            rand[0]='1';
            send(clientSocket,rand,1,0);    
            send(clientSocket,filename,10000,0);    
            send(clientSocket, sending, 10000, 0);
           }
            else if(c==2)
            {
                char rand[1];char recievedfile[1000000];
                rand[0]='2';
                send(clientSocket,rand,1,0);
                send(clientSocket,filename,100000,0);
                recv(clientSocket,recievedfile,1000000,0);
                cout<<"file recieved is - "<<endl<<recievedfile;c=0;
            }

            if(strcmp(buffer, ":exit") == 0){
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }
        }

    }
};


int madhash(char str[])
{
      
      int sum=0;
      for(int i=0;str[i]!=0;i++)
      {
            sum=sum+(i+19973)%5;
      }
      sum=sum%5;
      sum*=1000;
      sum+=2000;
      
      return sum;
}

int main(int argc, char** argv){
    node* n = new node(atoi(argv[1]));

    thread ser(&node::server,n);

    string commands[] = {"help","store","search","quit","\0"};
    string command = "";
    while(1){
        cout<<"Enter command : ";
        cin>>command;
        if(command == "store"){
            cout<<"enter file name that you want to store ";
            cin>>filename;
             int port;

            port =madhash(filename);

            
            c=1;
            n->client(port);
        }
        else if(command=="search"){
           cout<<"enter file name whose information you need ";
            cin>>filename;
             int port;

            port =madhash(filename);

            
            c=2;
            n->client(port);

        }
        else if(command == "quit" || command == "q"){
            ser.detach();
            break;
        }
        else if(command == "help"){
            for(int i = 0;commands[i] != "\0";i++)
                cout<<commands[i]<<endl;
        }
    }
}
