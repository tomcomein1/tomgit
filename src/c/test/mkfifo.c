#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	char input[] = "IPC by pipe";
	char output[BUFSIZ + 1];
	char p_name[] = "/tmp/test_fifo";
	int count = 0;
	int fd;
	int stat_value;
	pid_t pid, pid_res;

	memset(output, 0, sizeof(output) );
	if (mkfifo(p_name, 0777) == 0)
	{
		pid = fork();
		if (pid>0)
		{
			printf("father running\n");
			fd = open(p_name, 0); //open by read mode
			if  (fd == -1)
			{
				printf("open  pipe file failed.[%d][%s]\n", errno, strerror(errno));
				return 1;
			}
		}
		else if (pid == 0)
		{
			printf("son running\n");
			fd = open(p_name, 1); //open by write mode
			if (fd==-1)
			{
				printf("open pipe file failed\n");
				return 1;
			}

			count = write(fd, input, strlen(input));
			printf("son process write %d character, they are:%s\n", count, input);
			close(fd);
		}
		else 
		{
			printf("create process failed\n");
			return 1;
		}
		
	}
	else
	{
		printf("create fifo failed.[%s][%d][%s]\n", p_name, errno, strerror(errno));
	}
	pid_res = wait(&stat_value);
	if(pid_res > 0)
	{	
		count = read(fd, output, BUFSIZ);
		printf("father process read %d character, they are:%s\n", count, output);
		close(fd);
		unlink(p_name);
	}

	return 0;
}

