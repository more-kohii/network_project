#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF 1024
#define MAXCLIENT 5
#define NAMESIZE 20

void *t_main(void *arg);
void send_msg(char* msg, int len, int client_socket);

pthread_mutex_t mutex;
int client[MAXCLIENT];
int cnt = 0;
int check = 0;

int main(int argc, char **argv){

    if(argc != 2){
        printf("Usage: %s [port]\n", argv[0]);
        return 1;
    }

    pthread_t tid;

    int server_socket, client_socket;

    struct sockaddr_in client_addr, server_addr;
    int client_addr_size = sizeof(client_addr);

    pthread_mutex_init(&mutex, NULL);

    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket == -1){
        printf("SERVER : socket error\n");
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

    while(1){
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size);
        if(client_socket == -1){
            printf("SERVER : accept error\n");
            continue;
        }

        if(cnt == MAXCLIENT){
            if(check == 0){
                printf("client is full\n");
                check = 1;
            }
            close(client_socket);
            continue;
        }

        printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        pthread_mutex_lock(&mutex);
        client[cnt++] = client_socket;
        pthread_mutex_unlock(&mutex);

        pthread_create(&tid, NULL, t_main, (void *)&client_socket);
        pthread_detach(tid);
    }
    
    close(server_socket);
    return 0;
}

void *t_main(void *arg){
    int i, len = 0, name_len = 0,client_socket = *((int *)arg);
    
    char name[NAMESIZE];
    char tmp[MAXBUF];
    char buf[MAXBUF + name_len];

    if((name_len = recv(client_socket, name, NAMESIZE, 0)) == -1){
        printf("SERVER : name recv error\n");
        return NULL;
    }
    
    memset(buf, 0x0, MAXBUF);
    sprintf(buf, "%s is connected\n", name);
    printf("%s is connected\n", name);
    send_msg(buf, MAXBUF, client_socket);    
    
    while(1){
        if((len = recv(client_socket, tmp, MAXBUF, 0)) == -1){
            printf("SERVER : recv error\n");
            continue;
        }
        if(!strcmp(tmp, "QUIT"))
            break;
        sprintf(buf ,"%s: %s\n", name, tmp);
        printf("%s", buf);
        send_msg(buf, len, client_socket);
    }

    memset(buf, 0x0, MAXBUF);
    sprintf(buf, "%s is disconnected\n", name);
    printf("%s is disconnected\n", name);
    send(client_socket, buf, MAXBUF, 0);
    send_msg(buf, MAXBUF, client_socket);    

    pthread_mutex_lock(&mutex);
    for(i = 0; i < cnt; i++){
        if(client_socket == client[i]){
            while(i++ < cnt-1)
                client[i] = client[i+1];
            break;
        }
    }

    check = 0;
    cnt--;
    pthread_mutex_unlock(&mutex);
    close(client_socket);
}

void send_msg(char* msg, int len, int client_socket){
    int i;
    pthread_mutex_lock(&mutex);
    for(i = 0; i < cnt; i++)
        if(client[i] == client_socket)
            continue;
        else
            send(client[i], msg, len, 0);
    pthread_mutex_unlock(&mutex);
}