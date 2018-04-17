/*
 * tcpopt.c --- the interface function of TCP/IP communication 
 * used by SCO OPENSERVER.
 */

# include	"B2I_com.h"

int 	timeout = 0;
void	catchalm ( ), clearalm ( );
int	sockfd;
extern	int	errno;

/*
 * Set timeout signal;
 */

void set_alrm ( short sec )
{
	timeout = 0;

	alarm ( sec );
}


/*
 * open_sock(): open a TCP stream socket and connect it to server.
 *          if success it return the socket descriptor,or it
 *          return 0;
 */

int open_sock( unsigned int sock_num )    
{
    int sock;
    struct sockaddr_in proadd;
    char filename[80];
    FILE *fp;
    char ipaddr[40];

    sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sock < 0 ) {
        ;//comlog ( "open socket err!", __FILE__, __LINE__ );
         return FALSE;
       }

	memset ( filename , 0 , 80 );
	strcpy ( filename , B2IFILE ) ;
	;//strcpy ( filename , getetcf ( filename ) ) ;
	if ( !filename ) {
		;//comlog ( "get pathname err!", __FILE__, __LINE__ );
		return FALSE;
	}

	if ( ( fp = fopen ( filename, "r+" ) ) == NULL ) {
		;//comlog ( "open file err!", __FILE__, __LINE__ );
		return FALSE;
	}

    proadd.sin_family = AF_INET;
    fscanf ( fp , "%s" , ipaddr );
    proadd.sin_addr.s_addr = inet_addr ( ipaddr ) ;
    fclose ( fp ) ;
    proadd.sin_port = htons ( sock_num );        

    if ( connect ( sock, ( struct sockaddr * ) &proadd, sizeof(proadd)) == 0 )
       {
        return sock;
       }
    else {
        ;//comlog ( "Connect server failed!", NULL, 0 );
        close_sock ( sock );
	return FALSE;
    }
}

void close_sock ( const int fd )
{
    close ( fd ) ;
}

int 
rcvpkt ( const int sock, void *pktptr, int rcvlen, const short timval )
{
    int	result;
    int tmp = 1 ;

    signal ( SIGALRM , catchalm ) ;

    while ( tmp )
    {
	set_alrm ( timval ) ;
	result =  read ( sock, pktptr, rcvlen );
	clearalm ( ) ;
	if ( ( result < 0 ) && timeout ) 
	{
	    //comlog ( "read data from socket port error!", __FILE__, __LINE__ );
            printf("read data from socket port error!");
	    return ETIMEOUT ;
	}
	else 
	    tmp = !tmp ;
    }

    return result;
}

int 
b2i_rcv( const int sock, void *rcvptr ,int rcvlen, int timeval)
{
    int ret=0;

    do {
        ret=rcvpkt( sock, rcvptr, rcvlen, timeval );            
        if ( ret<0 ) {
            //comlog( "recv err!", __FILE__, __LINE__ );
 	   printf("recv err!");
            return FALSE;
        }

    } while ( !ret );

    return ret ;
}

int 
b2i_snd( const int sock, void *sndptr, int sendlen)
{
    int		result;

    result =  write ( sock, sndptr, sendlen );
    if ( result < 0 ) {
	//comlog ( "write data to socket port error!", __FILE__, __LINE__ );
		printf("write data to socket port error!");
		return FALSE;
    }
    return TRUE;
}

/*
 * get transcation code form socket port.
 */

# define	DEFAULTT	60

char *
getcode ( const int sockfd , struct Ctl_Msg *ctl_msg )
{
	char 	*code;

	void	catchalm ( );

	code = ( char * ) malloc ( ( CODESIZ + 1 ) * sizeof ( char ) );

	signal ( SIGALRM, catchalm );
	set_alrm ( DEFAULTT );

	b2i_rcv ( sockfd , ctl_msg , CTLMSGLEN , 10 ) ;
	strcpy ( code , ctl_msg -> TransCode ) ;

	clearalm ( );

	return code;
}

void
catchalm ( )
{
	//comlog ( "timeout!!", NULL, 0 );
        printf("timeout!!");

	timeout = 1;

	close_sock ( sockfd );
}

void
clearalm ( )
{
	alarm ( 0 );
}

