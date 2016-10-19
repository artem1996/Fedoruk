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
  
int main()
{
    int sock, listener;
    struct sockaddr_in addr;//ñòðóêòóðà îïèñûâàåò ñîêåò äëÿ ðàáîòû ñ ïðîòîêîëîì IP
    char buf[1024];
    char address [1024];
    int bytes_read;
    int check=0;
        char tar[512] = ""; //èçâëå÷ü èç àðõèâà ôàéë 
    FILE * file;
	int counter=0;  
    listener = socket(AF_INET, SOCK_STREAM, 0); //ñîçäàåì ñîêåò òèïà ñòðèì ñ ïðîòîêîëîì tcp âîçâðàùåò ñôàéëîâûé äåñêðèïòîð ñûëàþùèéñÿ íà ñîêåò èëè -1
    if(listener < 0)
    {
        perror("socket"); //îøèáêà ñîêåòà
        exit(1);
    }      
    addr.sin_family = AF_INET; //âñåãäà ðàâíî AF_INET
    addr.sin_port = htons(3425); //íîìåð ïîðòà êòîðûé çàíèìàåò ïðîöåññ
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //ip àäðåñ ê êîòîðîìó ïðèâÿåí ñîêåò
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }  
    listen(listener, 1);  

    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }  
        while(1)
        {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            if (!check) {
                strcat(tar, "mkdir ");
                strcat(tar, buf);
                strcat(tar, "\ntar -xvf ");
                strcat(tar, buf);
                strcat(tar, ".tar");
                strcat(tar, " -C ");
                strcat(tar, buf);
                strcat(tar, " \nrm ");
                strcat(tar, buf);
                strcat(tar, ".tar");
                printf("%s\n", tar);
        		check=1;
        		file=fopen(strcat(buf, ".tar"), "w+");
            } else {	
        		fwrite(buf, bytes_read, 1, file);
        		//printf("bytes read %d count %d\n",	bytes_read, counter++);
    		}
        }
        close(sock);
		fclose(file);  
        system(tar);
    }      
    return 0;
} 
