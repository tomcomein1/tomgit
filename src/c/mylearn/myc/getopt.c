/* 此程序用于测试输入参数 */

#include <stdio.h>
#include <unistd.h>

void usage (char *prname)
{
    printf ("%s -d [directory] -f [filename] -p [pointer] -h \n", prname);
}

int main(int argc, char *argv[])
{
    int ch;
    int have_para = 0;

    char *p;
    char direct[20] = ".";
    char file[80] = "\0";
    char pointer[4] = "\0";

    while ( (ch=getopt(argc, argv, "d:f:p:h")) != -1)
    {
        switch (ch)
        {
        case 'd':
            p = direct;
            break;
        case 'f':
            p = file;
            break;
        case 'p':
            p = pointer;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 0;
        } /*end switch*/
        have_para = 1;

        memcpy (p, optarg, strlen(optarg) );
        p[strlen(optarg)] = '\0';
    } /*end while*/

    if (have_para == 1)
        printf ("%s -d %s -f %s -p %s \n", argv[0], direct, file, pointer);
    else
        usage(argv[0]);

    exit (0);
}

