#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	char buffer[BUFSIZ+1];
	int read_count = 0;
	FILE *p_read, *p_write;

	memset(buffer, 0, sizeof(buffer));

	p_read = popen("env", "r");
	if (NULL == p_read)
	{
		printf("open pipe for reading faild\n");
		return 1;
	}
	
	p_write = popen("grep bash", "w");
	if (NULL == p_write)
	{
		printf("open pipe for writing faild\n");
		return 1;
	}

	read_count = fread(buffer, sizeof(char), BUFSIZ, p_read);
	while(read_count > 0)
	{
		fwrite(buffer, sizeof(char), strlen(buffer), p_write);
		read_count = fread(buffer, sizeof(char), BUFSIZ, p_read);
	}
	
	pclose(p_read);
	pclose(p_write);

	return 0;
}

