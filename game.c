#include <stdio.h>  // for printf, scanf, FILE, stdin, EOF
#include <stdlib.h> // for NULL, malloc
#include <string.h> // for strcmp, memmove
#include <time.h>   // for time() to seed rand

#define MULTIPLIER 31

enum
{
    NPREF = 2,      /* number of prefix words */
    NHASH = 4093,   /* size of state hash table array */
    MAXGEN = 10000, /* maximum words generated */
    BUFSIZE = 100,
};

char *NONWORD = "\n"; // sentinel to mark word boundaries

typedef struct State State;
typedef struct Suffix Suffix;
struct State
{
    /* prefix + suffix list */
    char *pref[NPREF]; /* prefix words */
    Suffix *suf;       /* list of suffixes */
    State *next;       /* next in hash table*/
};

struct Suffix
{
    char *word;   /* suffix */
    Suffix *next; /* next in list of suffixes*/
};

State *statetab[NHASH]; /* hash table of states */

/* Function declarations */
unsigned int hash(char *s[NPREF]);
void add(char *prefix[NPREF], char *word);
void addsuffix(State *sp, char *word);
void generate(int nwords);
State *lookup(char *prefix[NPREF], int create);
void build(char *prefix[NPREF], FILE *f);

/* emalloc: malloc and report if error */
void *emalloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL)
    {
        fprintf(stderr, "malloc of %zu bytes failed\n", n);
        exit(1);
    }
    return p;
}

/* estrdup: duplicate a string, report if error */
char *estrdup(char *s)
{
    char *t = malloc(strlen(s) + 1);
    if (t == NULL)
    {
        fprintf(stderr, "estrdup(\"%.20s\") failed\n", s);
        exit(1);
    }
    strcpy(t, s);
    return t;
}

/* markov main: markov-chain random text generation */
int main(void)
{
    int i, nwords = MAXGEN;
    char *prefix[NPREF];

    srand(time(NULL)); // seed random number generator

    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;
    build(prefix, stdin);
    add(prefix, NONWORD);
    generate(nwords);
    return 0;
}

/* hash: compute hash value for array of NPREF strings */
unsigned int hash(char *s[NPREF])
{
    unsigned int h;
    unsigned char *p;
    int i;

    h = 0;
    for (i = 0; i < NPREF; i++)
        for (p = (unsigned char *)s[i]; *p != '\0'; p++)
            h = MULTIPLIER * h + *p;
    return h % NHASH;
}

/* generate: produce output, one word per line */
void generate(int nwords)
{
    State *sp;
    Suffix *suf;
    char *prefix[NPREF], *W;
    int i, nmatch;

    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;

    for (i = 0; i < nwords; i++)
    {
        sp = lookup(prefix, 0);
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next)
            if (rand() % ++nmatch == 0) /*prob = 1/nmatch */
                W = suf->word;
        if (strcmp(W, NONWORD) == 0)
            break;
        printf("%s\n", W);
        memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
        prefix[NPREF - 1] = W;
    }
}

/* Lookup: search for prefix; create if requested.*/
/* returns pointer if present or created; NULL if not. */
/* creation doesn't strdup so strings mustn't change later. */
State *lookup(char *prefix[NPREF], int create)
{
    int i, h;
    State *sp;

    h = hash(prefix);
    for (sp = statetab[h]; sp != NULL; sp = sp->next)
    {
        for (i = 0; i < NPREF; i++)
            if (strcmp(prefix[i], sp->pref[i]) != 0)
                break;
        if (i == NPREF) /* found it a */
            return sp;
    }
    if (create)
    {
        sp = (State *)emalloc(sizeof(State));
        for (i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}

/* build: read input, build prefix table */
void build(char *prefix[NPREF], FILE *f)
{
    char buf[BUFSIZE], fmt[] = "%99s"; /* BUFSIZE -1 */

    /* create a format string; %s could overflow buf */
    sprintf(fmt, "%%%ds", (int)(sizeof(buf) - 1));
    while (fscanf(f, fmt, buf) != EOF)
        add(prefix, estrdup(buf));
}

/* add: add word to suffix list, update prefix */
void add(char *prefix[NPREF], char *word)
{
    State *sp;
    sp = lookup(prefix, 1);
    addsuffix(sp, word);

    /* move the words down the prefix */
    memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
    prefix[NPREF - 1] = word;
}

/* addsuffix: add to state, suffix must not change later */
void addsuffix(State *sp, char *word)
{
    Suffix *suf;
    suf = (Suffix *)emalloc(sizeof(Suffix));
    suf->word = word;
    suf->next = sp->suf;
    sp->suf = suf;
}
