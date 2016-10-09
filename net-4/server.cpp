#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>

#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

#include <iostream>

using namespace std;


#define SRV_PORT 8888
#define BUF_SIZE 512
#define REPLY_SIZE 2048
#define TXT_QUEST "Hello\n"

#define MAX_ARG_NUM 10
#define MAX_ARG_LEN 10

int sockID;
int clientSockID;

void Terminate(int sig)
{
    printf("SIGTERM accepted : closing sockets...\n");

    shutdown( clientSockID, 2 );
    shutdown( sockID, 2 );
    close( clientSockID );
    close( sockID );

    exit(0);
}


int main ()
{
    int clientAddrLen;

    int msgLen = 0;

    struct sockaddr_in serverSockAddr;
    struct sockaddr_in clientSockAddr;

    sockID = socket (PF_INET, SOCK_STREAM, 0);

    if( sockID < 0 )
    {
        printf( "Error: could not create socket!\n" );
        return sockID;
    }
    else
        printf( "Success: created socket %d.\n", sockID );


    memset( (char *)&serverSockAddr, 0, sizeof(struct sockaddr_in) );

    serverSockAddr.sin_family       = AF_INET;
    serverSockAddr.sin_addr.s_addr  = INADDR_ANY;
    serverSockAddr.sin_port         = SRV_PORT;

    if( bind( sockID, (struct sockaddr *)&serverSockAddr, sizeof( serverSockAddr ) ) == -1 )
    {
        printf( "Error: could not bind socket!\n" );
        return -1;
    }
    else
        printf( "Success: bound socket %d to network interfaces.\n", sockID );


    if( listen( sockID, 3 ) == -1 )
    {
        printf( "Error: could not listen socket!\n" );
        return -1;
    }

    signal( SIGINT, &Terminate );

    while (1)
    {
        int *argOffsets;

        int argNum = 0;
        int argCount = 0;
        int strOffset = 0;
        int i;

        clientAddrLen = sizeof( clientSockAddr );
        clientSockID  = accept ( sockID, (struct sockaddr*)&clientSockAddr, (socklen_t*)&clientAddrLen );

        if( !fork() )
        {
            char message[BUF_SIZE];

            char argStrings[MAX_ARG_NUM][MAX_ARG_LEN];

            struct termios stateBackup;
            struct termios newState;

            //Backup state of client socket
            tcgetattr( clientSockID, &stateBackup );

            bzero( &newState, sizeof( struct termios ) );

            newState.c_lflag &= ~(ICANON);

            //Set non-cannonical input for socket
            tcsetattr ( clientSockID, TCSANOW, &newState );

            msgLen = read ( clientSockID, message, BUF_SIZE );

            //Restore normal state for client socket
            tcsetattr ( clientSockID, TCSANOW, &stateBackup );

            FILE* stream = popen( message, "r" );

            char buf[BUF_SIZE];
            char* resStr = NULL;
            int size = 0;
            int allocSize = 0;

            bzero( buf, BUF_SIZE );

            resStr = (char*)malloc( 0 );

            int res = 0;

            do
            {
                res = fread( buf, BUF_SIZE - 1,  1, stream );

                size = strlen( buf );

                send ( clientSockID, buf, size, 0 );



                bzero( buf, BUF_SIZE );
            }
            while( res );

            send ( clientSockID, "end\0", 4, 0 );

            shutdown( clientSockID, 2 );
            close( clientSockID );
            exit( 0 );
        }
    }



    exit( 0 );
}

