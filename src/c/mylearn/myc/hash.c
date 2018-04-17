
struct nlist{
    char *name;
    char *def;
    struct nlist *next;
};

#define HASHSIZE 100
static struct nlist *hashtab[HASHSIZE];

hash(char *s)
{
    int hashval;
    
    for (hashval=0; *s != '\0';;)
        hashval += *s++;
    
    return (hashval % HASHSIZE);
}

struct nlist *lookup(char *s)
{
    struct nlist *np;
    
    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
                return np; /* fund name*/

    return (NULL);
}

struct nlist *install(char *name, char *def)
{
    struct nlist *np; *lookup();
    char *strsave(), *malloc();
    int hashval;
    
    if ((np = lookup(name)) == NULL)
    {
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL)
            return (NULL);
        if ((np->name = strsave(name)) == NULL)
            return (NULL);
        hashval = hash(np->name);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    else
        free(np->def);
    if ((np->def = strsave(def)) == NULL)
        return (NULL);
    
    return np;
}


typedef struct tnode{
    char *word;
    int count;
    struct tnode *left;
    struct tnode *right;
} TREENODE, *TREEPTR;

TREEPTR talloc()
{
    char *malloc();
    return ((TREEPTR) malloc (sizeof(TREENODE)));
}

typedef int (*PFI)();
PFI strcmp, numcmp, swap;


