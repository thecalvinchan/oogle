#include "provided.h"
#include "MyMap.h"
#include <iostream>
#include <string>
using namespace std;

class WordBagImpl
{
public:
    WordBagImpl(const string& text);
    ~WordBagImpl();
    bool getFirstWord(string& word, int& count);
    bool getNextWord(string& word, int& count);
private:
	MyMap<string,int>* m_map;
};

WordBagImpl::WordBagImpl(const string& text)
{
	Tokenizer t(text);
	m_map = new MyMap<string,int>;
	string temp;
	while(t.getNextToken(temp))
	{
        strToLower(temp);
		int* count = m_map->find(temp);
		//If the word already exists, increment the count
		if (count!=NULL)
			m_map->associate(temp,*count+1);
		//If the word does not exist, add it to the map
		else
		{
            m_map->associate(temp,1);
        }
        
	}
}

WordBagImpl::~WordBagImpl()
{
    delete m_map;
}


bool WordBagImpl::getFirstWord(string& word, int& count)
{
	int* temp = m_map->getFirst(word);
	if (temp==NULL)
    	return false;
    else
    {
    	count=*temp;
    	return true;
    }
}

bool WordBagImpl::getNextWord(string& word, int& count)
{
    int* temp = m_map->getNext(word);
    if (temp==NULL)
    	return false;
    else
    {
    	count=*temp;
    	return true;
    }
}

//******************** WordBag functions *******************************

// These functions simply delegate to WordBagImpl's functions.
// You probably don't want to change any of this code.

WordBag::WordBag(const std::string& text)
{
    m_impl = new WordBagImpl(text);
}

WordBag::~WordBag()
{
    delete m_impl;
}

bool WordBag::getFirstWord(string& word, int& count)
{
    return m_impl->getFirstWord(word, count);
}

bool WordBag::getNextWord(string& word, int& count)
{
    return m_impl->getNextWord(word, count);
}
