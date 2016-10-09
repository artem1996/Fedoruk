#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>

#include <stdio.h>

#include <iostream>

using namespace std;


#define SRV_PORT 8888
#define CLNT_PORT 8889
#define BUF_SIZE 640
#define TXT_ANSW "/bin/pwd"

#define SRV_ADDR_LEN 16
#define MAX_COMMAND_LEN 256

 int main (int argc, char **argv)
 {
     if( argc != 3 )
     {
         printf( "usage : remcmd server_addr command\nexample : remcmd 192.168.0.1 pwd\n" );
         return -1;
     }

    int sockID;
    char srvAddrStr[SRV_ADDR_LEN];
    char cmdStr[MAX_COMMAND_LEN];

    strcpy( srvAddrStr, argv[1] );
    strcpy( cmdStr,     argv[2] );


//    int clientAddrLen;
 //   char buf[BUF_SIZE];

    struct sockaddr_in clientSockAddr;
    struct sockaddr_in serverSockAddr;

    struct servent *sp;

    sockID = socket (AF_INET, SOCK_STREAM, 0);
    memset ( &clientSockAddr, 0, sizeof( struct sockaddr_in ) );

    clientSockAddr.sin_family       = AF_INET;
    clientSockAddr.sin_addr.s_addr  = INADDR_ANY;
    clientSockAddr.sin_port         = CLNT_PORT;

    bind ( sockID, (struct sockaddr*)&clientSockAddr, sizeof( clientSockAddr ) );


    memset ( (char *)&serverSockAddr, 0, sizeof( serverSockAddr ) );

    inet_aton( srvAddrStr,  &serverSockAddr.sin_addr );

    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_port   = SRV_PORT;

    int res = connect ( sockID, (struct sockaddr*)&serverSockAddr, sizeof( serverSockAddr ) );

    if( res == -1 )
    {
        printf( "error connecting to server \"%s\".\n", srvAddrStr );
        return -1;
    }

    //from_len = recv (s, buf, BUF_SIZE, 0);
    //write (1, buf, from_len);

    //printf("%d\n", sizeof(argv[1]));
    send ( sockID, cmdStr, 100, 0 );

    char buf[BUF_SIZE];
    char* resStr = NULL;
    int size = 1;
    int allocSize = 0;

    bzero( buf, BUF_SIZE );

    resStr = (char*)malloc( 0 );

    while( size )
    {
        size = recv( sockID, buf, BUF_SIZE, 0 );

        if( strcmp( buf, "end" ) == 0 )
            break;

        write( 1, buf, size );
    }


    shutdown( sockID, 2 );
    close ( sockID );

    return 0;
 }

