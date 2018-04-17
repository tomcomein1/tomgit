#include <stdio.h>

static char *flab[] = {
        "actually",
        "just",
        "quite",
        "really",
        NULL
};

/* lookup: sequential search for word in array */
int lookup (char *word, char *array[])
{
        int i;

        for (i=0; array[i] != NULL; i++)
        {
                if (strcmp (word, array[i]) == 0)
                        return i;
        }

        return -1;
}

int main(void)
{
	int unit = 0;
	char *s = "just";
	
	unit = lookup (s, flab);
	if (unit == -1)
		printf (" not find \"%s\" in flab\n", s);	
	else 
		printf (" find \"%s\" in flab[%d]!\n", s, unit);
}

