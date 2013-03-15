#include "provided.h"
#include "MyMap.h"
#include <string>
using namespace std;

class SearcherImpl
{
public:
    SearcherImpl();
    ~SearcherImpl();
    vector<string> search(string terms);
    bool load(string filenameBase);
private:
    Indexer* m_index;
    struct bucket
    {
        int m_distinct;
        int m_total;
    };
};

SearcherImpl::SearcherImpl(){m_index=new Indexer;}
SearcherImpl::~SearcherImpl(){delete m_index;}

vector<string> SearcherImpl::search(string terms)
{
    int numTerms=0;
    MyMap <string,bucket> tempmap;
    WordBag tempbag(terms);
    string word;
    int count;
    bool hasWord = tempbag.getFirstWord(word, count);
    while (hasWord)
    {
        strToLower(word);
        //If m_index has N words, this will take O(log N)
        vector<UrlCount> temp = m_index->getUrlCounts(word);
        vector<UrlCount>::iterator it = temp.begin();
        while (it!=temp.end())
        {
            bucket* tempnode=tempmap.find((*it).url);
            if (tempnode==NULL)
            {
                tempnode = new bucket();
                tempnode->m_distinct=1;
                tempnode->m_total=((*it).count);
            }
            else
            {
                tempnode->m_distinct+=1;
                tempnode->m_total+=((*it).count);
            }
            tempmap.associate((*it).url,*tempnode);
            it++;
        }
        numTerms++;
        hasWord=tempbag.getNextWord(word, count);
    }
    
    MyMap<int,vector<string> > matches;
    string url;
    int match = 1;
    if (numTerms>1)
        match = 0.7 * numTerms;
    bucket* curnode = tempmap.getFirst(url);
    while (curnode!=NULL)
    {
        if (curnode->m_distinct >= match)
        {
            vector<string>* temp = matches.find(curnode->m_total);
            if (temp==NULL)
                temp = new vector<string>;
            temp->push_back(url);
            matches.associate(curnode->m_total, *temp);
        }
        curnode = tempmap.getNext(url);
    }
    //MyMap matches now contains a tree of urls sorted by their score.
    //Must insert into vector using inorder traversal
    return matches.inOrder(matches.getRoot());
}

bool SearcherImpl::load(string filenameBase)
{
    return m_index->load(filenameBase);
}

//******************** Searcher functions *******************************

// These functions simply delegate to SearcherImpl's functions.
// You probably don't want to change any of this code.

Searcher::Searcher()
{
    m_impl = new SearcherImpl;
}

Searcher::~Searcher()
{
    delete m_impl;
}

vector<string> Searcher::search(string terms)
{
    return m_impl->search(terms);
}

bool Searcher::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
