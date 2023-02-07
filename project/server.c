#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFMAX 1024

int main(int argc, char **argv){
    int server_socket, client_socket;

    struct sockaddr_in client_addr, server_addr;
    int client_addr_size = sizeof(client_addr);
 
    char buf[BUFMAX];
    memset(buf, 0x00, BUFMAX);

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket == -1){
        printf("SERVER : bind error\n");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;                  //IPv4 인터넷 프로토콜
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);        //32bit IPv4 주소 자동으로 배정함.
    server_addr.sin_port        = htons(atoi(argv[1]));     //인자로 port 번호 할당
 
    if(bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
        printf("SERVER : bind error\n");
        return 1;
    }
    
    if(listen(server_socket, 5) == -1){
        printf("SERVER : listen error\n");
        return 1;
    }
   
    client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);
    if(client_socket == -1){
        printf("SERVER : accept error\n");
        return 1;
    }

    printf("Connection from %s:%d\n",inet_ntoa(client_addr.sin_addr),client_addr.sin_port);

    while(1){
        if(recv(client_socket, buf, BUFMAX, 0) == -1){
            printf("SERVER : recv error");
            close(client_socket);
            close(server_socket);
            return 0;
        }
        
        if(!strcmp(buf, "QUIT")){
            printf("client : %s\n", buf);
            printf("Disconnected\n");
            close(server_socket);
            return 0;
        }

        printf("client : %s\n", buf);

        memset(buf, 0x00, BUFMAX);
        printf("me > ");
        fgets(buf, BUFMAX, stdin);
        buf[strlen(buf) - 1] = '\0';
        if(!strcmp(buf, "QUIT")){
            if(send(client_socket, buf, BUFMAX, 0) == -1){
                printf("SERVER : write error\n");
                return 1;
            }
            printf("Disconnected\n");
            close(client_socket);
            close(server_socket);
            return 0;
        }

        if(send(client_socket, buf, BUFMAX, 0) == -1){
            printf("SERVER : write error\n");
            return 1;
        }
    }
    
    close(client_socket);
    close(server_socket);
    return 0;
}