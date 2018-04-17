#include "liblog.h"

/*
    filename: GetDateTime.c
    function: ȡ��ǰϵͳ����ʱ�䣬
              ��fmtָ���ִ���ʽ
              [y|Y - ������
               m - ������
               d|D - ������
               H|h - ����Сʱ
               M - �������
               S|s - ������
               ]
               �����ָ���ԭ�����
*/
unsigned long GetDateTime(char *date_time, const char *fmt)
{
    time_t    tbuf;
    struct tm *t;
    unsigned char c ;
    char format_time[DT_STR_LEN];
    const char format[] = "ymdHMS";
    unsigned long nSysDate = 0;

    memset(format_time, 0x00, sizeof(format_time) );

    time(&tbuf);
    t = (struct tm *)localtime(&tbuf);

    if (*fmt == '\0')
        fmt = format;

    /*�����ָ����ʽ*/
    while(c = *fmt)
    {
        switch(c)
        {
        case 'y':
        case 'Y':
            sprintf(format_time + strlen(format_time), "%4.4d", t->tm_year + 1900);
            break;
        case 'm':
            sprintf(format_time + strlen(format_time), "%2.2d", t->tm_mon + 1);
            break;
        case 'd':
        case 'D':
            sprintf(format_time + strlen(format_time), "%2.2d", t->tm_mday);
            break;

        case 'H':
        case 'h':
            sprintf(format_time + strlen(format_time), "%2.2d", t->tm_hour);
            break;
        case 'M':
            sprintf(format_time + strlen(format_time), "%2.2d", t->tm_min);
            break;
        case 'S':
        case 's':
            sprintf(format_time + strlen(format_time), "%2.2d", t->tm_sec);
            break;

        default:
            sprintf(format_time + strlen(format_time), "%c", c );
            break;
        } /* end switch*/
        fmt++;
        while (c == *fmt) fmt++;

    } /*end while*/

    strcpy(date_time, format_time );

    nSysDate = (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
    return nSysDate;
}

/*������־Ŀ¼*/
void  CreateDir(const char *path)
{
    char buf[DIR_STR_LEN];
    char t_path[DIR_STR_LEN];
    char *offset = NULL;
    int  len = 0;
    int  path_len ;

    strcpy (t_path, path);
    path_len =  strlen(t_path);
    offset = t_path + path_len;

    while (offset > t_path && offset[0] != '/')
    {
        offset[0] = '\0';
        offset--;
        path_len--;
    }

    if (path_len <= 0)
        return ;
    else t_path[path_len] = '\0';
    /* fprintf (stdout, "path [%s]\n", path); */

    if(access(offset, 0))
    {
        sprintf(buf, "mkdir -p  %s", t_path);
        system(buf);
        sprintf(buf, "chmod 777 %s", t_path);
        system(buf);
    }
}

/*************************************************************
 * �����־��Ϣ
 * ���ܣ���ʾ������Ϣ
 * ������char *format:�ɱ�����б���ʾ������Ϣ
 *       ...:
 * ���أ�>0:�ɹ�  <0:ʧ��   =0:û�ҵ�����
 * ˵����
*************************************************************/
void WriteLog(const char *logfile, const char *file, int line, char *format, ...)
{
    va_list pvar;
    char    buf[MAX_STRING_LEN];
    char    dt[DT_STR_LEN];
    FILE	*fp = NULL;

    va_start(pvar, format);
    vsprintf(buf, format, pvar);
    va_end(pvar);

    GetDateTime(dt, "Y-m-d H:M:S");

    /* fprintf(stderr,"[%s][%s:%d] %s\n", dt, file, line, buf); */

    /* fprintf (stdout, "logfile=[%s]\n", logfile); */

    CreateDir(logfile);
    fp = fopen(logfile, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "Open log file error.[%s]\n", logfile);
        return ;
    }

    fprintf(fp, "[%s][%s:%d] %s\n", dt, file, line, buf);
    fclose(fp);

    return ;
}


/* ������־Ŀ¼ �ļ�*/
int GetLogFile(char *logfile, const char *file)
{
    char dt[DT_STR_LEN];
    long nsysdate = 0;

    nsysdate = GetDateTime(dt, "Y-m-d H:M:S");

#ifdef WINDOWS
    sprintf (logfile, "%s.%ld.log", file, nsysdate);
#else
    sprintf (logfile, "%s/%s/%ld/%s", getenv("HOME"), "log", nsysdate, file );
#endif

    return 0;
}

/* ��ʼ������ */
LIST *init_err_list()
{
    LIST *node = (LIST *)malloc (sizeof(LIST) ) ;

    if (node)
    {
        node->next = NULL;
        memset(&node->ERR , 0x00, sizeof(ERR_STRUCT));
    }
    else
    {
        fprintf(stderr, "malloc false!\n");
        return NULL;
    }

    printf ("init link head ok!\n");
    return node;
}

void free_err_list (LIST *head)
{
    LIST *L = head;
    LIST *node = NULL;
    int record = 0;

    while (L != NULL)
    {
        node = L;
        L = L->next;
        memset (&node->ERR, 0x00, sizeof(ERR_STRUCT) );
        free(node);
        node = NULL;
        /* printf ("free link... \n"); */
        record++;
    }
    printf ("�ͷ������¼��: [%d]\n", record);
    printf ("free link ok ! \n");
}

/* �������� ����*/
int insert_err_list_tail(LIST *head, ERR_STRUCT *err_data)
{
    LIST *L = head;
    LIST *node;

    node = (LIST *)malloc (sizeof(LIST) );
    if (node == NULL)
    {
        fprintf(stderr, "malloc false!\n");
        return (-1);
    }

    memcpy (&node->ERR, err_data, sizeof(ERR_STRUCT) );
    node->next = NULL;

    while ( L->next != NULL)
        L = L->next;

    /** printf ("insert link tail ok\n"); **/

    return 0;
}

/**ͷ�巨**/
int insert_err_list_head(LIST *head, ERR_STRUCT *err_data)
{
    LIST *L = head->next;
    LIST *node;

    node = (LIST *) malloc (sizeof(LIST) );
    if (node == NULL)
    {
        fprintf(stderr, "malloc false!\n");
        return (-1);
    }

    memcpy (&node->ERR, err_data, sizeof(ERR_STRUCT) );

    node->next = L;
    head->next = node;

    /** printf ("insert link head ok\n"); **/

    return 0;
}

void prt_err_list(LIST *head)
{
    LIST *L = head->next;

    while (L != NULL)
    {
        fprintf (stdout, "SysNo = [%s]\n",  L->ERR.SysNo);
        fprintf (stdout, "ErrNo = [%ld]\n",  L->ERR.ErrNo);
        fprintf (stdout, "ErrMsg = [%s]\n", L->ERR.ErrMsg);
        L = L->next;
    }
}

/** ����������Ϣ **/
int find_err_list(LIST *head, ERR_STRUCT *err_data)
{
    LIST *L = head->next;

    while (L != NULL)
    {
        /** ���ҵ���Ӧ���� **/
        if ( (strcmp (L->ERR.SysNo, err_data->SysNo) == 0) &&
                L->ERR.ErrNo == err_data->ErrNo )
        {
            strcpy (err_data->ErrMsg, L->ERR.ErrMsg);
            return 0;
        }
        L = L->next;
    }

    strcpy (err_data->ErrMsg, "δ֪����!");
    return -1;
}

/* ���ָ��ȡ�ַ����е����� n�ڼ����ָ������ b�ָ���� ��ͬ��[TOM] */
char *get_sub_string(char *source, char *dest, int c, int n)
{
    char *p = source;
    int i = 1;

    c = c ? c : ' '; /*Ĭ��Ϊ�ո�*/
    if (n > 1)
    {
        while(1)
        {
            if ((i == n) || (*p == '\0') || (*p == '\n') ) break;
            if (*p++ == c) i++;
        }
    }

    /*  ���n<=1������ȡ�ַ��� */
    while ((*p != '\0') && (*p != c) && (*p != '\n'))
        *dest++ = *p++;

    *dest = '\0';

    return ((*p) ? ++p : NULL);
}

/** ����ʵ�� **/
int get_err_msg(char *filename, char *_SysNo, int _ErrNo, char *_ErrMsg)
{
    FILE *fp = NULL;
    LIST *H = NULL;
    ERR_STRUCT err;
    char line[MAX_STRING_LEN];
    char ErrNo[10] ;
    int record = 0;

    fp = fopen (filename, "r");
    if (NULL == fp )
    {
        fprintf (stderr, "open file error: [%s]\n", filename);
        return -1;
    }

    H = init_err_list();
    if (H == NULL) return -1;
    record++;

    while (1)
    {
        memset (line, 0x00, sizeof(line) );
        memset (&err, 0x00, sizeof(err) );
        if (fgets(line, MAX_STRING_LEN, fp) == NULL)
            break;

        /** ���˵�ע�� **/
        if (line[0] == '#') continue;

        get_sub_string(line, err.SysNo, '|', 1);
        /** �����൱ϵͳ��װ�������Ч�� **/
        if (strncmp (err.SysNo, _SysNo, strlen(_SysNo) != 0) )
        {
            continue;
        }

        get_sub_string(line, ErrNo, '|', 2);
        err.ErrNo = atol(ErrNo);
        get_sub_string(line, err.ErrMsg, '|', 3);

        insert_err_list_head(H, &err);
        record++;
    }

    fprintf (stdout, "װ�������¼�� : [%d] \n", record);

    /* prt_err_list(H); */
    memset (&err, 0x00, sizeof(ERR_STRUCT) );
    strcpy (err.SysNo, _SysNo);
    err.ErrNo = _ErrNo;

    find_err_list(H, &err);
    strcpy (_ErrMsg, err.ErrMsg);

    free_err_list(H);

    fclose (fp);
    fp = NULL;
}

/* LOG TEST Main*/
int main(int argc, char *argv[])
{
    char logfile[DIR_STR_LEN];
    char ErrMsg[256];

    get_err_msg("err_info.txt", "IBPS", 10, ErrMsg);
    printf ("ErrMsg = [%s]\n", ErrMsg);

    GetLogFile(logfile, "run.log");
    DBG(logfile, _ARGL_, "aaaaaaaaa");

    return 0;
}

