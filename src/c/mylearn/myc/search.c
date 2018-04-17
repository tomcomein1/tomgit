#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct RE {
	int type; /* CHAR, STAR, etc. */
	int ch;   /* the character itself */
	char *cc1; /* for [...] instead*/
	int ncc1;  /* true if class is negated[^...] */
}RE;

int match(char *regexp, char *text);
int matchhere (char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);

/* match: search for regexp anywhere in text */
int match(char *regexp, char *text)
{
	if (regexp[0] == '^')
		return matchhere(regexp+1, text);
	do {
		/*must look even if string is empty*/
		if (matchhere(regexp, text))
			return 1;
	} while (*text++ != '\0');

	return 0;
}

/*matchhere: search for regexp at beginning of text*/
int matchhere (char *regexp, char *text)
{
	if (regexp[0] == '\0')
		return 1;
	if (regexp[0] == '*')
		return matchstar(regexp[1], regexp+2, text);
	if (regexp[0] == '$' && regexp[1] == '\0')
		return *text == '\0';
	if (*text != '\0' && (regexp[0]== '.' || regexp[0]==*text))
		return matchhere(regexp+1, text+1);
	return 0;
}
/* matchstar: search for c*regexp at beginning of text */
int matchstar1(int c, char *regexp, char *text)
{
	do { /* a * matches zero or more instances */
		if (matchhere(regexp, text))
			return 1;
	} while (*text != '\0' && (*text++ == c || c== '.'));
	return 0;
}

/*new matchstar calc*/
int matchstar(int c, char *regexp, char *text)
{
	char *t;
	for (t=text; *t != '\0' && (*t==c || c=='.'); t++)
		;
	do { /* * matches zero or more */
		if (matchhere(regexp, t))
			return 1;
	} while (t-- > text);
	return 0;
}

/*test this search*/
int main(int argc, char *argv[])
{
	int fund=0;
	char source[1000], s[50];
	
	strcpy (source, argv[1]);
	strcpy (s, argv[2]);

	fund=match(s, source);
	if (fund)
		printf ("fund data %s: %s\n", source, s);	
	else
		printf ("not fund data!\n");

	exit (0);
}

