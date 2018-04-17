/*����Ϣ�������Ա�����, ����ÿ�����֧��4096 byte*/
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define ERROR -1
#define SUCC  1

#define MSGKEY   22222
#define MSGTYPE  0666
#define END 0

struct msg_struct
{
    int msg_type;            /*����*/
    char msg_text[BUFSIZ]; /*�洢��Ϣ�ռ��С*/
};

key_t get_key_queue()
{
    key_t  msgkey;
    char filepath[2+1];
    
    strcpy (filepath, "." );
    msgkey = ftok (filepath, 0x88);
    return msgkey;
}

/*����һ����Ϣ����*/
int create_msg_queue(int *id)
{
    int msgid;
    key_t msgkey;
    
    if ((msgkey = get_key_queue () ) == ERROR)
        return ERROR;
        
    if ((msgid = msgget( msgkey, MSGTYPE|IPC_CREAT)) == ERROR) 
    {
        perror("msgget");
        exit(ERROR);
    }
    /*��Ϣ����ֵ*/
    *id = msgid; 
    
    return SUCC;
}

/*д��Ϣ��������*/
int send_queue_msg(int msgid, struct msg_struct msginfo)
{
    if ((msgsnd(msgid, (void *)&msginfo, BUFSIZ, 0)) == ERROR )
    {
        fprintf(stderr, "msgsnd [%d]\n", errno);
        return ERROR;
    }
    /* printf ("[%d]write 0x%x : %s\n", msgid, msgkey, msginfo.msg_text); */
    return SUCC;
}

/*����Ϣ��������*/
int recive_msg_queue(int msgid, struct msg_struct *msginfo)
{
    int msg_to_receive = 0;
    key_t msgkey;
    
    if ((msgkey = get_key_queue () ) == ERROR)
        return ERROR;

    if ((msgrcv(msgid, (void *)msginfo, BUFSIZ, msg_to_receive, 0)) == ERROR )
    {
        fprintf(stderr, "msgrcv [%d]\n", errno);
        return ERROR;
    }
    printf ("[%u] read 0x%x : %s", (unsigned int) msgid, msgkey, msginfo->msg_text);
    if (strncmp(msginfo->msg_text, "end", 3) == 0 ) return END;

    return SUCC;
}

/*�ͷ���Ϣ����*/
int free_msg_queue(int msgid)
{
    if (msgctl(msgid, IPC_RMID, 0) == ERROR)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(ERROR);
    }
    printf ("[%d]free \n", msgid);
    
    return SUCC;
}

void usage()
{
	printf ("msgqueue -w [send msg] -r -h\n");
	exit (0);
}

/*�����Ժ���*/
int main(int argc, char *argv[])
{
    int msgid = 0;
    int ret = 0;
    struct msg_struct msg;
    char buffer[BUFSIZ];
    int ch ;
    
    int running = 1;
    
    if (argc < 2)
	usage();

    while ((ch = getopt(argc, argv, "rRhw:W:")) != -1)
    {
        ret = create_msg_queue (&msgid);
        if (ret == ERROR) return -1;

        switch(ch)
        {
            case 'w':
            case 'W':
                 msg.msg_type = 1;
                 strcpy(msg.msg_text, optarg);
                 ret = send_queue_msg(msgid, msg);
                 break;
            case 'r':
            case 'R':
                 while (recive_msg_queue(msgid, &msg));
                 ret = free_msg_queue(msgid);
                 break;
            case 'h': usage(); break;
	    default:
                usage();
                break;
        }
    }
    
    return ret;
}

