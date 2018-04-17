#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXLINE 0x1000

static void sig_int(int);

                
                
int main(void)
{
        char buf[MAXLINE];
        pid_t pid;
        int status;

        if (signal (SIGINT, sig_int) == SIG_ERR)
        {
                perror("signal error");
                exit (errno);
        }

        printf ("%% ");
        while (fgets(buf, MAXLINE, stdin) != NULL) 
        {
                if (buf[strlen(buf) - 1] == '\n')
                        buf[strlen(buf) - 1] = 0;
                if ((pid = fork()) < 0) {
                        perror("fork error");
                        exit (errno);
                }
                else if (pid == 0) {
                        execlp (buf, buf, (char *)0);
                        /*execlp("ls", "ls", "-al", 0);*/
                        /*execl("/usr/bin/sh", "sh", "-c", "ls -l *.c",0);*/
                        if (errno) {
                                printf ("couldn't execute: %s\n", buf);
                                exit (127);
                        }
                }
                if ((pid = waitpid(pid, &status, 0)) < 0)
                {
                        perror("waitpid error");
                        exit (errno);
                }
                printf ("%% ");
        }
        exit (0);
        
}

void sig_int (int signo)
{
        printf ("interrup\n%% ");
}
