#include "provided.h"
#include "MyMap.h"
#include "http.h"
#include <iostream>
#include <string>
using namespace std;


void foo();
bool testIndexer();
bool testWebCrawler();

bool testSearcher();

int main()
{
    foo();
    WordBag wb("<html>I’m livin’ 2 love hash tables, I say.</html>");
    // etc.
    string word;
    int count;
    bool gotAWord = wb.getFirstWord(word, count);
    while (gotAWord)
    {
        cout << "The word " << word << " occurs " << count
        << " times" << endl;
        gotAWord = wb.getNextWord(word,count);
    }
    testIndexer();
    testWebCrawler();
    testSearcher();
}

void foo()
{
    MyMap<string,double> nameToGPA; // maps student name to GPA
    // add new items to the binary search tree-based map
    nameToGPA.associate("Carey", 3.5); // Carey has a 3.5 GPA
    nameToGPA.associate("David", 3.99); // David beat Carey
    nameToGPA.associate("Abe", 3.2); // Abe has a 3.2 GPA
    double* davidsGPA = nameToGPA.find("David");
    if (davidsGPA != NULL)
        *davidsGPA = 1.5; // after a re-grade of David’s exam
    nameToGPA.associate("Carey", 4.0); // Carey deserves a 4.0
    // replaces old 3.5 GPA
    double* lindasGPA = nameToGPA.find("Linda");
    if (lindasGPA == NULL)
        cout << "Linda is not in the roster!" << endl;
    else
        cout << "Linda’s GPA is: " << *lindasGPA << endl;
    // now let’s print all associations out in a level-order ordering
    cout << "Here are the " << nameToGPA.size() << " associations: "
    << endl;
    string name;
    for (double* GPAptr = nameToGPA.getFirst(name); GPAptr != NULL;
         GPAptr = nameToGPA.getNext(name))
    {
        cout << name << " has a GPA of " << *GPAptr << endl;
    }
    // The above loop would print:
    // Carey has a GPA of 4.0
    // Abe has a GPA of 3.2
    // David has a GPA of 1.5
}

void writeWordInfo(Indexer& indexer, string word)
{
    vector<UrlCount> urlCounts = indexer.getUrlCounts(word);
    if (urlCounts.empty())
    {
        cout << word << " was not found in the index." << endl;
        return;
    }
    for (int i = 0; i < urlCounts.size(); i++)
        cout << word << " appears " << urlCounts[i].count
        << " times at " << urlCounts[i].url << endl;
}
bool testIndexer()
{
    const string INDEX_PREFIX = "/Users/Calvin/Desktop/testindexer";
    Indexer indexer;
    WordBag wb1("<html>i like gogiberries and I hate spam</html>");
    indexer.incorporate("www.a.com", wb1);
    WordBag wb4("<html>i like gogiberries and I love you</html>");
    indexer.incorporate("www.d.com", wb4);
    WordBag wb5("<html>i like gogiberries and I love</html>");
    indexer.incorporate("www.e.com", wb5);
    writeWordInfo(indexer, "I");
    // writes I appears 2 times at www.a.com
    // save the index as file(s) whose names start with prefix
    if ( ! indexer.save(INDEX_PREFIX))
        return false; // error saving the index
    // load the just-saved index into another Indexer
    Indexer indexer2;
    if ( ! indexer2.load(INDEX_PREFIX))
        return false; // error loading the index
    // add more pages to the second index
    WordBag wb2("<html>engineering is FUN</html>");
    indexer2.incorporate("www.b.com", wb2);
    WordBag wb3("<html>Engineering majors like fun</html>");
    indexer2.incorporate("www.c.com", wb3);
    writeWordInfo(indexer2, "like");
    // writes in some order:
    // like appears 1 times at www.a.com
    // like appears 1 times at www.c.com
    writeWordInfo(indexer2, "smallberg");
    // writes smallberg was not found in the index
    return true;
    
}

void reportStatus(string url, bool success)
{
    if (success)
        cout << "Downloaded and indexed the page at " << url << endl;
    else
        cout << "Unable to download the page at " << url << endl;
}
bool testWebCrawler()
{
    const string INDEX_PREFIX = "/Users/Calvin/Desktop/testindexer";
    WebCrawler wc;
    // load a previously-saved index from disk
    if ( ! wc.load(INDEX_PREFIX)) // error loading
        return false;
    // specify which URLs we are to crawl and index
    
    wc.addUrl("http://www.nytimes.com");
    wc.addUrl("http://www.theverge.com");
    wc.addUrl("http://www.yahoo.com/main");
    // download the specified URLs and add their contents to the
    // index, designating reportStatus as the callback function
    wc.crawl(reportStatus);
    // save the updated index to disk
    if ( ! wc.save(INDEX_PREFIX)) // error saving
        return false;
    return true; // no errors
}

bool testSearcher()
{
    const string INDEX_PREFIX = "/Users/Calvin/Desktop/testindexer";
    Searcher s;
    // load a previously-built index
    if ( ! s.load(INDEX_PREFIX)) // error loading
        return false;
    string query;
    while (getline(cin, query) && !query.empty())
    {
        vector<string> matches = s.search(query);
        if (matches.empty())
            cout << "No pages matched your search terms." << endl;
        else
        {
            cout << "Your search found " << matches.size()
            << " matching pages." << endl;
            for (int i = 0; i < matches.size(); i++)
                cout << matches[i] << endl;
        }
    }
    return true;
}
