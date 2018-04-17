#include "para_svr.h"
#include "para_macro.h"

/**处理connect错误的方法 指数补偿(exponential backoff)算法 **/
int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
    int nsec;
    /*
     * Try to connect with exponential backoff.
     */
    for (nsec = 1; nsec <= MAXSLEEP; nsec <<= 1)
    {
        if (connect(sockfd, addr, alen) == 0)
        {
            /*Connection accepted. */
            return (0);
        }

        /*
         * Delay befor trying again.
         */
        if (nsec <= MAXSLEEP / 2)
            sleep(nsec);
    }
    return (-1);
}

/** 建立socket服务器 **/
int create_server(const char *ip_addr, const int iport, int lnum)
{
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    int nsock, ssock;
    int cli_len;
    pid_t pid;
    
    memset( &serv_addr, 0, sizeof( serv_addr ) );
    serv_addr.sin_family  = AF_INET;
    if ( strlen(ip_addr) < 1)
        serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    else
        serv_addr.sin_addr.s_addr = inet_addr( ip_addr );
    serv_addr.sin_port = htons( iport );
    
    ssock = initserver(SOCK_STREAM, ( const struct sockaddr * )&serv_addr, sizeof(serv_addr), lnum);
    if (ssock < 0) return -1;
    
    cli_len = sizeof(cli_addr);
    while(1)
    {
        nsock = accept(ssock, (struct sockaddr * )&cli_addr, (socklen_t *)&cli_len );
        if (nsock < 0)
            continue;
        
        pid = fork();
        if (pid > 0)
        {
            close(nsock);
            WriteLog( ERR_LVL, "%08d : main fork ok child'pid[%d]", getpid(), pid );
            continue;
        }
        else if(pid != 0)
        {
            fprintf(stderr, "fork errno[%d][%s]", errno, strerror(errno) );
            exit (-1);
        }
        else {
            close(ssock); 
            exec_sub_func(nsock);
            close(nsock);
            exit(0);
        }

    }
}


/** 服务器初始化套接字端点 **/
int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd;
    int reuse = 1;

    if ((fd = socket(addr->sa_family, type, 0)) < 0)
    {
        fprintf(stderr, "ERROR: socket errno[%d][%s]", errno, strerror(errno) );
        return (-1);
    }
    
    /* 地址复用初始化套接字端点 */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        goto errout;

    if (bind(fd, addr, alen) < 0)
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
    {
        if (listen(fd, qlen) < 0)
            goto errout;
    }
    return fd;

errout:
    fprintf(stderr, "ERROR: errno[%d][%s]\n", errno, strerror(errno) );
    close(fd);
    return (-1);
}

/**循环接收数据***/
int recv_data(int sockfd, char *buf, int sec_timeout)
{
    int n = 0;
    char p[BUFLEN]; 
    char s_len[10];
    int  first = 1, b_len = 0 ;

    memset(s_len, 0, sizeof(s_len) );
    set_rcv_time_out(sockfd, sec_timeout);
    
    while ( 1 )
    {
        memset(p, 0, BUFLEN);
        if (n = recv(sockfd, p, BUFLEN, 0) < 0) 
            break;

        if (first == 1)
        {
            strncpy(s_len, p, 8 ) ;
            b_len = atol(s_len);
            first++;
        }

        strcat(buf, p);
        if (strstr(buf, "</body>") ) break;

        if (strlen(buf) >= b_len) break; 
    }
    
    return n;
}

/*设置接收超时间*/
int set_rcv_time_out(const int sockt, const int rcv_timeout)
{
    struct fd_set      fds;
    struct timeval     tm;
    int  ret = 0;
    
    /* 等待接收报文 设置为非阻塞模式 */
    FD_ZERO( &fds );
    FD_SET( sockt, &fds );
    tm.tv_sec = rcv_timeout;
    tm.tv_usec = 0;
    
    ret = select( sockt+1, &fds, NULL, NULL, &tm );
    if( ret == 0  ) {
        fprintf(stderr, "ret[%d] 获取数据超时 errno[%d][%s]", ret, errno,strerror(errno) );
        close( sockt );
        return (-1);
    }
    else if( ret <= 0 || FD_ISSET( sockt, &fds) == 0 ) {
        fprintf(stderr, "ret[%d] 获取数据失败 errno[%d][%s]", ret, errno,strerror(errno) );
        close( sockt );
        return (-1);
    }

    return 0;
}

/** 网络连通测试 ***/
int pingtest(const char *host)
{
    FILE *fp;
    char buf[201];

#ifdef HPUNIX
    sprintf(buf, "ping %s -c 100", host);
#else
    sprintf(buf, "ping -c 100 %s", host);
#endif
    fp = popen(buf, "r");
    if(fp != NULL)
    {
        while(fgets(buf, 201, fp) != NULL)
        {
            if(strstr(buf, " from "))
            {
                pclose(fp);
                return 1;
            }
            else if(strstr(buf, "ret=-1"))
            {
                fprintf(stderr, "不能得到远方主机服务(%s)\n", host);
                pclose(fp);
                return 0;
            }
        }
        pclose(fp);
        fprintf(stderr, "时间超出,不能得到远方主机服务(%s)\n", host);
    }
    else
    {
        fprintf(stderr, "不能执行与远方主机的连通测试(%s)\n", host);
    }

    return 0;
}

/*------------------------------------------------------------------------
 * connectsock - allocate & connect a socket using TCP or UDP
 *------------------------------------------------------------------------
 */
/** 客户端建立socket连接 返回socket值 **/
int connectsock(const char *host, const int service, const char *transport )
{
    struct sockaddr_in sin;
    int s, type;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = PF_INET;
    sin.sin_port = htons(service);
    sin.sin_addr.s_addr = inet_addr(host);

    /* Use protocol to choose a socket type */
    if (strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    /* Allocate a socket */
    s = socket(sin.sin_family, type, 0);
    if (s < 0)
    {
        fprintf(stderr, "can't create socket: %s", strerror(errno));
        return -1;
    }

    /* Connect the socket */
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        fprintf(stderr, "不能连接到%s.%s: %s", host, service, strerror(errno));
        return -1;
    }

    return s;
}

/*执行子函数*/
int  exec_sub_func(int sockfd)
{
    char in_buff[MAX_BUFF_LEN];
    char out_buff[MAX_BUFF_LEN];
    char *p = NULL;
    int ret = 0;
    
    memset( in_buff, 0, sizeof( in_buff ) );

    if (recv_data(sockfd, in_buff, 30)< 0)
    {
        fprintf(stderr, "recv error.sock[%d].err[%s]\n", sockfd,  strerror(errno));
        return -1;
    }

#ifdef DBG_UTF_PKG
    WriteLog( ERR_LVL, "RECV UTF-8 BUFF:[%d] [%s]", strlen(in_buff), in_buff);
#endif

    p = para_iconv(UT_CODE, GB_CODE, in_buff);
    if (p == NULL) return -1;
    strcpy (in_buff, p);
    free(p), p=NULL;

    if (strstr(in_buff, "DBPWDCHECK") == NULL)
        WriteLog( ERR_LVL, "RECV BUFF:[%d] [%s]\n", strlen(in_buff), in_buff);
    else
        WriteLog( ERR_LVL, "RECV DBPWDCHECK BUFF:[%ld]\n", strlen(in_buff) );

/** for test time out **/
sleep(30);

    memset(out_buff, 0, sizeof( out_buff ) );
    /** LoadFunc( "ParamSet", in_buff, out_buff ); **/
    ret = ParamSet(in_buff, out_buff);
    WriteLog( ERR_LVL, "SEND BUFF:[%d] [%s]", strlen(out_buff), out_buff);

    p = para_iconv(GB_CODE, UT_CODE, out_buff);
    if (p == NULL) return -1;
    strcpy (out_buff, p);
    free(p), p=NULL;

#ifdef DBG_UTF_PKG
    WriteLog( ERR_LVL, "SEND UTF-8 BUFF:[%d] [%s]", strlen(out_buff), out_buff);
#endif

    if ( send( sockfd, out_buff, strlen( out_buff ) + sizeof( char ), 0 ) != strlen( out_buff ) + sizeof( char ) )
    {
        WriteLog( ERR_LVL, "send error: out_buff[%s]", out_buff );
        return (-1);
    }

    return 0;
}

/*调用动态库*/
int LoadFunc( char *name, char *in, char *out )
{
    void         *handle;
    int          ( *fcn )( char *in, char *out ) = NULL;
    const char * errmsg;
    char         path[1024];

    WriteLog( ERR_LVL, "LoadFunc start" );
    memset( path, 0, sizeof( path ) );
    sprintf( path, "%s/lib/libpara_busi.so", getenv( "HOME" ) );
    WriteLog( ERR_LVL, "so[%s] name[%s] ", path, name );

    handle = dlopen( path, RTLD_NOW );
    if( handle == NULL )
    {
        WriteLog( ERR_LVL, "Failed to load libpara_busi.so: [%s]", dlerror() );
        return -1;
    }
    if ( ( fcn = ( int (*)( char *, char * ) )dlsym( handle, name ) ) == NULL )
    {
        WriteLog( ERR_LVL, "Failed to load func: [%s]", dlerror() );
        dlclose( handle );
        return -1;
    }

    dlclose( handle );
    WriteLog( ERR_LVL, "LoadFunc end" );
    return 0;
}

int usage(const char *_progname)
{
    printf ("*************************************\n");
    printf ("*=根据表名生成相应的表结构头文件定义=\n");
    printf ("*\t%s\t\t\n", _progname );
    printf ("*\t\t -i [ip addr] \t\t\n");
    printf ("*\t\t -p [port]  \t\t\n");
    printf ("*\t\t -n [进程数] \t\t\n");
    printf ("*\t\t -h \t\t\n");
    printf ("*\t\t -k \t\t\n");
    printf ("*************************************\n");
}

int kill_server(char *prog_name)
{
    char cmd[COMMAND_LEN];
    sprintf (cmd, "ps -ef|grep %s;"
                  "kill -9 `ps -ef|grep %s|grep -v grep | awk '{print $2}'`;", 
                 prog_name, prog_name );

    printf ("EXEC: [%s]\n", cmd);
    return system(cmd);
}

/** for test socket **/
int main(int argc, char *argv[])
{
    pid_t pid = -1;

    char ip_addr[50];
    char port[10];
    char lisnum[10]="5";

    int  iport = 6789;
    int ch;

    memset (ip_addr, 0x00, sizeof(ip_addr) );
    memset (port , 0x00, sizeof(port) );
    memset (Plogdir, 0x00, sizeof(Plogdir) );
    strcpy (Plogdir, "ParaSet.log");
    
    while ((ch = getopt(argc, argv, "i:I:p:P:m:M:kKhH"))!= -1)
    {
        switch(ch)
        {
            case 'i':
            case 'I':
                strncpy (ip_addr, optarg, 49);
                break;
            case 'p':
            case 'P':
                strncpy (port, optarg, 9);
                break;
            case 'k':
            case 'K':
                if( kill_server(argv[0]) != 0 )
                    printf( "kill_ib_server fail" );
                return 0;
            case 'm':
            case 'M':
                strncpy (lisnum, optarg, 9);
                break;
            case 'h':
            case 'H':
                usage(argv[0]);
                return 0;
            default:
                usage(argv[0]);
                return 0;
        }
    }

    if ( strlen(port) > 0)
    {
         iport = atoi(port);
    }
    
     /* 子进程守护 */
    pid = fork();
    if ( pid == 0) {
        signal(SIGCLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

        /* 初始化进程表 */
        WriteLog( ERR_LVL, "create_server:ip_addr[%s]iport[%d]lisnum[%s]", ip_addr, iport, lisnum);
        create_server(ip_addr, iport , atol(lisnum) );
        exit(-1);
    }
    else if (pid<0)
    {
        usage(argv[0]);
        return -1;
    }
    
    /*主进程退出*/
    print_time( "==================================" );
    if( pid>0 ) {
        printf("主控进程为[%d] -- port[%d]\n", pid, iport);
        print_time( "==================================" );
    }

    exit(0);
}
