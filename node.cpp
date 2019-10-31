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

using namespace std;

class node
{
    int PORT,ser_run;

    public:
    node(int p){
        PORT = p;
        ser_run = 1;
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
            return;
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


        while(ser_run){
            newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
            if(newSocket < 0){
                return;
            }
            printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

            if((childpid = fork()) == 0){
                close(sockfd);

                while(recv(newSocket, buffer, 1024, 0) > 0){
                    
                    cout<<"Cli : "<<buffer<<endl;
                    if(strcmp(buffer, ":exit") == 0){
                        printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
                        break;
                    }
                    memset(buffer, '\0', sizeof(buffer));
                }
            }

        }

        close(newSocket);
        cout<<"Server stopped"<<endl;
    }

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
            return;
        }
        printf("[+]Connected to Server.\n");

        while(1){
            printf("Client: \t");
            scanf("%s", &buffer[0]);
            send(clientSocket, buffer, strlen(buffer), 0);
            
            if(strcmp(buffer, ":exit") == 0){
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                return;
            }
            memset(buffer, '\0', sizeof(buffer));
        }

    }

    int store(string file_path){

    }

    int serach(int key){

    }
};

int main(int argc, char** argv){
    node* n = new node(atoi(argv[1]));

    thread ser(&node::server,n);

    string commands[] = {"help","store","search","quit","\0"};
    string command = "";
    while(1){
        cout<<"Enter command : ";
        cin>>command;
        if(command == "store"){
            cout<<"Port number : ";
            int port;
            cin>>port;
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
	return 0;
}