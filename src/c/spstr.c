#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *str = NULL;
	int i=1;

	if (argc < 3) {
		printf("%s <s-string> <token-str>\n", argv[0]);
		exit (-1);
	}

	printf ("source: %s\n", argv[1]);
	printf ("Token: %s\n", argv[2]);
	str = strtok(argv[1], argv[2]);
	
	while(str != NULL) {
		printf ("[%d]%s\n", i++, str);
		str = strtok(NULL, argv[2]);
	}

	exit (0);
}

