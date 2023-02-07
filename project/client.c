#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>

#define BUFMAX     1024

int main(int argc, char **argv){
    int sock;
    struct sockaddr_in server_addr;
    
    char buf[BUFMAX];
	memset(buf, 0x00, BUFMAX);

    //socket()
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1){
        printf("CLIENT : socket error\n");
        return 1;
    }
 
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;                  //IPv4 인터넷 프로토콜
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);       //인자로 서버의 IP 주소 할당
    server_addr.sin_port        = htons(atoi(argv[2]));     //인자로 port 번호 할당
		 
	//connect()
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        printf("CLIENT : connect error\n");
        return 1;
    }

    printf("Connected\n");

    while(1){
        printf("me > ");
        fgets(buf, BUFMAX, stdin);
        buf[strlen(buf) - 1] = '\0';
        if(!strcmp(buf, "QUIT")){
            if(send(sock, buf, BUFMAX, 0) == -1){
                printf("CLIENT : write error\n");
                return 1;
            }
            printf("Disconnected\n");
            close(sock);
            return 0;
        }

        if(send(sock, buf, BUFMAX, 0) == -1){
            printf("CLIENT : write error\n");
            return 1;
        }
        
        memset(buf, 0x00, BUFMAX);
        if(recv(sock, buf, BUFMAX, 0) == -1){
            printf("CLIENT : read error\n");
            return 1;
        }

        if(!strcmp(buf, "QUIT")){
            printf("server : %s\n", buf);
            printf("Disconnected\n");
            close(sock);
            return 0;
        }
        printf("server : %s\n", buf);
    }
    close(sock);
    return 0;
}