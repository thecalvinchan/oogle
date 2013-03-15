#include "provided.h"
#include "MyMap.h"
#include "http.h"
#include <string>
using namespace std;

class WebCrawlerImpl
{
public:
    WebCrawlerImpl();
    ~WebCrawlerImpl();
    void addUrl(string url);
    int getNumberOfUrls() const;
    void crawl(void (*callback)(string url, bool success));
    bool save(string filenameBase);
    bool load(string filenameBase);
private:
    Indexer* m_index;
    MyMap<string,bool>* m_urls;
};

WebCrawlerImpl::WebCrawlerImpl()
{
    m_index=new Indexer;
    m_urls=new MyMap<string,bool>;
}

WebCrawlerImpl::~WebCrawlerImpl()
{
    delete m_urls;
    delete m_index;
}

void WebCrawlerImpl::addUrl(string url)
{
    m_urls->associate(url, false);
}

int WebCrawlerImpl::getNumberOfUrls() const
{
    int m_visited=0;
    string tempurl;
    bool* visited = m_urls->getFirst(tempurl);
    while (visited != NULL)
    {
        if (*visited==true)
            m_visited++;
        visited = m_urls->getNext(tempurl);
    }
    return m_visited;
}

void WebCrawlerImpl::crawl(void (*callback)(string url, bool success))
{
    string tempurl;
    bool* visited = m_urls->getFirst(tempurl);
    while (visited != NULL)
    {
        if(*visited == false)
        {
            string content;
            bool connect = HTTP().get(tempurl, content);
            if (connect)
            {
                WordBag wb(content);
                m_index->incorporate(tempurl, wb);
            }
            callback(tempurl,connect);
            *visited = true;
        }
        visited = m_urls->getNext(tempurl);
    }
}

bool WebCrawlerImpl::save(string filenameBase)
{
    return m_index->save(filenameBase);
}

bool WebCrawlerImpl::load(string filenameBase)
{
    return m_index->load(filenameBase);
}

//******************** WebCrawler functions *******************************

// These functions simply delegate to WebCrawlerImpl's functions.
// You probably don't want to change any of this code.

WebCrawler::WebCrawler()
{
    m_impl = new WebCrawlerImpl;
}

WebCrawler::~WebCrawler()
{
    delete m_impl;
}

void WebCrawler::addUrl(string url)
{
    m_impl->addUrl(url);
}

int WebCrawler::getNumberOfUrls() const
{
    return m_impl->getNumberOfUrls();
}

void WebCrawler::crawl(void (*callback)(string url, bool success))
{
    m_impl->crawl(callback);
}

bool WebCrawler::save(string filenameBase)
{
    return m_impl->save(filenameBase);
}

bool WebCrawler::load(string filenameBase)
{
    return m_impl->load(filenameBase);
}
