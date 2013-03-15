#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;


struct bucket;
const int MAX_MAP_SIZE=20351;

class IndexerImpl
{
public:
    IndexerImpl();
    ~IndexerImpl();
    bool incorporate(string url, WordBag& wb);
    vector<UrlCount> getUrlCounts(string word);
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
    struct point
    {
        point(int hash, int count):m_hash(hash),m_count(count){};
        int m_hash;
        int m_count;
    };
    //clears index
    void clear();
    //hashes a string and copies value into hash, returns hashed index
    int hashurl(string &url,int &hash);
    //idToUrl maps a hash to its string value
    MyMap <int,string>* idToUrl[MAX_MAP_SIZE];
    MyMap <string, vector<point> >* urlToCount;
    vector<int> indexes;
};

IndexerImpl::IndexerImpl()
{
    urlToCount=new MyMap<string, vector<point> >;
}

IndexerImpl::~IndexerImpl()
{
    //clear();
}

void IndexerImpl::clear()
{
    delete urlToCount;
    vector<int>::iterator it = indexes.begin();
    while(it!=indexes.end())
    {
        delete idToUrl[(*it)];
        it++;
    }
}

int IndexerImpl::hashurl(string &url, int &hash){
    int i, total=0;
    for (i=0;i<url.length();i++)
        total=total + (i+1)*url[i];
    hash = total;
    total = total % MAX_MAP_SIZE;
    return total;
}

bool IndexerImpl::incorporate(string url, WordBag& wb)
{

    int hash;
    //Generates a hash of url as well as index for linear probing
    int modhash = hashurl(url,hash);
    //If the head bucket of the array is not initialized, initialize it
    if (idToUrl[modhash]==NULL)
        idToUrl[modhash]=new MyMap<int,string>;
    //Checks to see if this url has already been passed to incorporate
    if (idToUrl[modhash]->find(hash)!=NULL)
        return false;
    //Adds url to hash-url map
    if (idToUrl[modhash]->size()==0)
        indexes.push_back(modhash);
    idToUrl[modhash]->associate(hash,url);
    
    
    string word;
    int count;
    //Iterates through W distinct words in wordbag. O(W)
    bool gotAWord = wb.getFirstWord(word, count);
    while (gotAWord)
    {
        //Accounts for case-insensitive indexing.
        strToLower(word);
        point bucket = point(hash,count);
        //Find if word already exists. O(log N).
        vector<point>* temp = urlToCount->find(word);
        if (temp==NULL)
        {
            //The word does not yet exist in the index
            //Create a temp vector
            temp = new vector<point>;
        }
        temp->push_back(bucket);
        //Map word to temp Map of urlhash to wordcount
        urlToCount->associate(word, *temp);
        gotAWord = wb.getNextWord(word,count);
    }
    //Final BigO is O(WlogN) which is less than O(WN)
    return true;
}

vector<UrlCount> IndexerImpl::getUrlCounts(string word)
{
    strToLower(word);
    vector<UrlCount> output;
    //First locates the word in urlToCount Map. O(log N) steps
    vector<point>* temp = urlToCount->find(word);
    //If word is not found in index, return empty vector
    if (temp==NULL)
        return output;
    vector<point>::iterator it = temp->begin();
    //Iterates through the vector mapped to the word. O(log N) + V steps where V is the size of the vector
    while (it!=temp->end())
    {
        UrlCount copy;
        copy.count=it->m_count;
        int id = it->m_hash;
        string url = *(idToUrl[id%MAX_MAP_SIZE]->find(id));
        copy.url=url;
        output.push_back(copy);
        it++;
    }
    //Final BigO is O(logN + V) which is less than O(N + V)
    return output;  
}

bool IndexerImpl::save(string filenameBase)
{
    ofstream outfile(filenameBase+".txt");
    if ( ! outfile )		 
	{
	    cerr << "Error: Cannot create data.txt!" << endl;
	    return false;
	}
    //Map to store locations to grab from array
    MyMap<int, int>* h2pull = new MyMap<int, int>;
    //Save size of urlToCount to outfile
    outfile << urlToCount->size() << endl;
    string word;
    vector<point>* value = urlToCount->getFirst(word);
    while (value!=NULL)
    {
        //save word and size of vector
        outfile << word << " " << value->size() << endl;
        vector<point>::iterator it = value->begin();
        while (it!=value->end())
        {
            int hash = it->m_hash;
            outfile << hash << " " << it->m_count << endl;
            h2pull->associate(hash%MAX_MAP_SIZE, hash%MAX_MAP_SIZE);
            it++;
        }
        value = urlToCount->getNext(word);
    }
    //Save size of h2pull (number of elements in array to visit)
    outfile << h2pull->size() << endl;
    int temp;
    int* location = h2pull->getFirst(temp);
    while (location!=NULL)
    {
        int size = idToUrl[temp]->size();
        outfile << temp << " " << size <<endl;
        int hash;
        string* url = idToUrl[temp]->getFirst(hash);
        while(url!=NULL)
        {
            outfile << hash << " "<< *url << endl;
            url = idToUrl[temp]->getNext(hash);
        }
        location = h2pull->getNext(temp);
    }
    
    delete h2pull;
    return true;
}

bool IndexerImpl::load(string filenameBase)
{
    //Deletes existing map of url to count
    clear();
    urlToCount = new MyMap<string, vector<point> >;
    ifstream infile(filenameBase+".txt");
    if ( ! infile )
	{
	    cerr << "Error: Cannot open data.txt!" << endl;
	    return false;
	}
    int urlToCountsize;
    infile >> urlToCountsize;
    infile.ignore(10000, '\n');
    string tempword;
    int vectorsize;
    for (int i=0;i<urlToCountsize;i++)
    {
        infile >> tempword;
        infile >> vectorsize;
        infile.ignore(10000, '\n');
        vector<point> tempvec;
        for (int j=0;j<vectorsize;j++)
        {
            int hash, count;
            infile >> hash;
            infile >> count;
            infile.ignore(10000, '\n');
            tempvec.push_back(point(hash,count));
        }
        urlToCount->associate(tempword, tempvec);
    }
    int idToUrlsize;
    infile >> idToUrlsize;
    infile.ignore(10000, '\n');
    for (int j=0;j<idToUrlsize;j++)
    {
        int index, count;
        infile >> index;
        infile >> count;
        infile.ignore(10000, '\n');
        for (int i=0;i<count;i++)
        {
            int key;
            string url;
            infile >> key;
            infile >> url;
            infile.ignore(10000, '\n');
            if (idToUrl[index]==NULL)
                idToUrl[index]=new MyMap<int, string>;
            idToUrl[index]->associate(key, url);
        }
    }
    return true;
}

//******************** Indexer functions *******************************

// These functions simply delegate to IndexerImpl's functions.
// You probably don't want to change any of this code.

Indexer::Indexer()
{
    m_impl = new IndexerImpl;
}

Indexer::~Indexer()
{
    delete m_impl;
}

bool Indexer::incorporate(string url, WordBag& wb)
{
    return m_impl->incorporate(url, wb);
}

vector<UrlCount> Indexer::getUrlCounts(string word)
{
    return m_impl->getUrlCounts(word);
}

bool Indexer::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool Indexer::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
