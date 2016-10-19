#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

  
  
int main(int argc, char * argv[])
{
char message [1024];
char buf1 [1024];
strcpy(buf1, strtok(argv[2], "@"));
strcpy (message, strtok(NULL, "@"));
char buf[sizeof(message)];
char tar[512] = "tar -cvzf ";
size_t size;
pid_t pid;
FILE * file;
    int sock; int counter=0;
    struct sockaddr_in addr;
    
    unsigned long net_addr;
    net_addr=inet_network(buf1);
            strcat(tar, argv[1]);
            strcat(tar, ".tar ");
            strcat(tar, argv[1]);
            system(tar);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    file=fopen(strcat(argv[1], ".tar"), "r");   
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }  
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); // \u0438\u043b\u0438 \u043b\u044e\u0431\u043e\u0439 \u0434\u0440\u0443\u0433\u043e\u0439 \u043f\u043e\u0440\u0442...
    addr.sin_addr.s_addr = htonl(net_addr);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    send(sock, message, sizeof(message), 0);
    while((size=fread(message, 1, 1, file))>0)      
    {
	send(sock, message, 1, 0);
	}         
    printf(buf);    
    close(sock);
    strcpy(tar, "rm ");
    strcat(tar, argv[1]);
    system(tar);  
    return 0;
} 
