#include <iostream> // for cin, cout
#include <map>      // for map container
#include <deque>    // for deque (double-ended queue)
#include <vector>   // for vector container
#include <string>   // for string class
#include <cstdlib>  // for rand(), srand()
#include <ctime>    // for time()

using namespace std;

// Constants
const int NPREF = 2;
const int MAXGEN = 10000;
const string NONWORD = "\n";

// Type definitions
typedef deque<string> Prefix;

// Global state table
map<Prefix, vector<string>> statetab;

// Function declarations
void build(Prefix &prefix, istream &in);
void add(Prefix &prefix, const string &s);
void generate(int nwords);

// markov main: markov-chain random text generation
int main(void)
{
    int nwords = MAXGEN;
    Prefix prefix;

    srand(time(NULL)); // seed random number generator

    for (int i = 0; i < NPREF; i++)
        add(prefix, NONWORD);
    build(prefix, cin);
    add(prefix, NONWORD);
    generate(nwords);
    return 0;
}

// add: add word to suffix list, update prefix
void add(Prefix &prefix, const string &s)
{
    if (prefix.size() == NPREF)
    {
        statetab[prefix].push_back(s);
        prefix.pop_front();
    }
    prefix.push_back(s);
}

// build: read input words, build state table
void build(Prefix &prefix, istream &in)
{
    string buf;
    while (in >> buf)
        add(prefix, buf);
}

// generate: produce output, one word per line
void generate(int nwords)
{
    Prefix prefix;
    int i;

    for (i = 0; i < NPREF; i++) // reset initial prefix
        add(prefix, NONWORD);
    for (i = 0; i < nwords; i++)
    {
        vector<string> &suf = statetab[prefix];
        if (suf.empty()) // check if suffix list is empty
            break;
        const string &w = suf[rand() % suf.size()];
        if (w == NONWORD)
            break;
        cout << w << "\n";
        prefix.pop_front();
        prefix.push_back(w);
    }
}
