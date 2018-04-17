#include <stdio.h>
#include <fcntl.h>

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";
#define FILE_MODE 0644

int main(void)
{
	int fd;
	
	if ((fd = creat("file.hole", FILE_MODE)) < 0)
	{
		perror("creat error");
		exit(-1);
	}
	if (write(fd, buf1, 10) != 10)
	{
		perror("creat error");
		exit(-1);
	}
	if (lseek(fd, 16384, SEEK_SET) == -1)
	{
		perror("creat error");
		exit(-1);
	}
	if (write(fd, buf2, 10) != 10)
	{
		perror("creat error");
		exit(-1);
	}
	
	close (fd);
	
	rw_std ();
	
	exit (0);
}
/** od -c file.hole **/

#define BUFFSIZE 4096
int rw_std()
{
	int n;
	char buf[BUFFSIZE];
	
	while (( n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
	{
		if (write(STDOUT_FILENO, buf, n) != n)
		{
			perror("write error");
			return (-1);
		}
	}
	
	if (n < 0)
	{
		perror("write error");
		return (-1);
	}
	
	return 0;
}
