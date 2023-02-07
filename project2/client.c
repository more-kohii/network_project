#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF     1024
#define NAMESIZE   20

pthread_mutex_t mutex;
char name[NAMESIZE] = {0, };
int name_len;

void *send_msg(void *arg);
void *recv_msg(void *arg);

int main(int argc, char **argv){
    pthread_mutex_init(&mutex, NULL);
    int sock;
    struct sockaddr_in server_addr;
    pthread_t send_t, recv_t;
    void *thread_return;
    sprintf(name, "%s", argv[3]);
    name_len = strlen(name);

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

    if(send(sock, name, name_len, 0) == -1){
        printf("CLIENT : name send error\n");
        return 1;
    }
    printf("%s is connected\n",name);
    pthread_create(&send_t, NULL, send_msg, (void*)&sock);
    pthread_create(&recv_t, NULL, recv_msg, (void*)&sock);
    pthread_join(send_t, &thread_return);
    pthread_join(send_t, &thread_return);
    
    close(sock);
    return 0;
}

void *send_msg(void* arg){
    int sock = *((int*)arg);
    char buf[MAXBUF];

    while(1){
        scanf("%s",buf);

        if(send(sock, buf, MAXBUF, 0) == -1){
            printf("CLIENT : send error\n");
            return NULL;
        }

        if(!strcmp(buf, "QUIT")){
            printf("%s is disconnected", name);
            close(sock);
            return NULL;
        }
    }
}

void *recv_msg(void* arg){
    int sock = *((int*)arg);
    char buf[MAXBUF];
    int len;
    char str[100];
    sprintf(str, "%s is disconnnected\n",name);

    while(1){
        if(len = recv(sock, buf, MAXBUF, 0) == -1){
            printf("CLIENT : recv error\n");
        }
        if(!strcmp(buf, str)){
            close(sock);
            return NULL;
        }
        printf("%s", buf);
    }
}