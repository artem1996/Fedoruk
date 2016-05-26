#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string>
#include <cstring>

typedef struct links
{
    char link[100];
} links;

std::string linkmas[100];

int linkcount=0;

int create_tcp_socket();
std::string get_ip(std::string host);
std::string build_get_query(std::string host, std::string page);
void usage();

void getlinks(std::string text)
{
    int i=0,j=0;
    while((i = text.find("href=\"", j)) != std::string::npos)
    {
        i += 6;
        j = text.find("\"", i);
        linkmas[linkcount++] = text.substr(i, j - i);
    }
}

void showLinks()
{
    int i = 0;
    if(i == linkcount)
        printf("No links on this page \n");
    while (i < linkcount)
        printf("%d. %s \n", i++, linkmas[i].c_str());
}

#define HOST "fedoruk.comcor.ru"
#define PAGE "/"
#define PORT 80
#define USERAGENT "HTMLGET 1.0"

int main(int argc, char **argv)
{
    struct sockaddr_in *remote;
    int sock,second = 0;
    int tmpres;
    std::string ip;
    std::string get;
    char buf[BUFSIZ+1];
    std::string host;
    std::string page;
    int newbuf;
    if(argc == 1)
        host = HOST;
    else
        host = argv[1];
    if(argc > 2)
        page = argv[2];
    else
        page = PAGE;

    while(1)
    {
        if (second)
        {
            showLinks();
            scanf("%d",&newbuf);
            page = linkmas[newbuf];
            linkcount = 0;
        }

        sock = create_tcp_socket();
        ip = get_ip(host);
        fprintf(stderr, "IP is %s\n", ip.c_str());
        remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
        remote->sin_family = AF_INET;
        tmpres = inet_pton(AF_INET, ip.c_str(), (void *)(&(remote->sin_addr.s_addr)));
        if( tmpres < 0)
        {
            perror("Can't set remote->sin_addr.s_addr");
            exit(1);
        }
        else if(tmpres == 0)
        {
            fprintf(stderr, "%s is not a valid IP address\n", ip.c_str());
            exit(1);
        }
        remote->sin_port = htons(PORT);

        if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0)
        {
            perror("Could not connect");
            exit(1);
        }
        get = build_get_query(host, page);
        fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get.c_str());

        //Send the query to the server
        int sent = 0;
        while(sent < get.length())
        {
            tmpres = send(sock, (get.substr(sent)).c_str(), get.length()-sent, 0);
            if(tmpres == -1)
            {
                perror("Can't send query");
                exit(1);
            }
            sent += tmpres;
        }
        //now it is time to receive the page
        memset(buf, 0, sizeof(buf));
        int htmlstart = 0;
        char * htmlcontent;
        while((tmpres = recv(sock, buf, BUFSIZ, 0)) > 0)
        {
            if(htmlstart == 0)
            {
                /* Under certain conditions this will not work.
                * If the \r\n\r\n part is splitted into two messages
                * it will fail to detect the beginning of HTML content
                */
                htmlcontent = strstr(buf, "\r\n\r\n");
                if(htmlcontent != NULL)
                {
                    htmlstart = 1;
                    htmlcontent += 4;
                }
            }
            else
            {
                htmlcontent = buf;
            }
            if(htmlstart)
            {
                //fprintf(1, "%s" ,htmlcontent);
                fwrite(htmlcontent,1,strlen(htmlcontent),stdout);
                getlinks(htmlcontent);
                second = 1;
                //fwrite(htmlcontent,1,strlen(htmlcontent),fp);
            }

            memset(buf, 0, tmpres);
        }

        if(tmpres < 0)
        {
            perror("Error receiving data");
        }
        free(remote);
        close(sock);
    }
    return 0;
}

int create_tcp_socket()
{
    int sock;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("Can't create TCP socket");
        exit(1);
    }
    return sock;
}


std::string get_ip(std::string host)
{
    struct hostent *hent;
    int iplen = 15; //XXX.XXX.XXX.XXX
    char* ip;
    memset(ip, 0, iplen+1);
    if((hent = gethostbyname(host.c_str())) == NULL)
    {
        herror("Can't get IP");
        exit(1);
    }
    if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
    {
        perror("Can't resolve host");
        exit(1);
    }
    return ip;
}

std::string build_get_query(std::string host, std::string page)
{
    std::string query;
    std::string getpage = page;
    std::string tpl = "GET /%s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n\r\n";

    if(getpage.substr(0, 1) == "/"){
        getpage = getpage.substr(1);
        fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page.c_str(), getpage.c_str());
    }
    // -5 is to consider the %s %s %s in tpl and the ending \0
    query += tpl;
    query += getpage;
    query += host;
    query += USERAGENT;
    return query;
}