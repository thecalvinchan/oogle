#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED
#include <queue>
#include <iostream>
using namespace std;

template <class KeyType, class ValueType>
struct Node
{
    Node(KeyType key, ValueType value)
    {
        m_key = key;
        m_value = value;
        m_right=m_left=NULL;
    };
    ~Node()
    {
        delete m_right;
        delete m_left;
    };
    KeyType m_key;
    ValueType m_value;
    Node* m_right;
    Node* m_left;
};



template <class KeyType, class ValueType>
class MyMap
{
public:
    
    MyMap()
    {
        m_root = NULL;
        m_size = 0;
    }
    
    ~MyMap()
    {
        clear();
    }
    
    void clear()
    {
        if (m_root==NULL)
            return;
        else
        {
            delete m_root;
            m_root=NULL;
        }
    }
    
    int size() const
    {
        if (m_root==NULL)
            return 0;
        else
            return m_size;  // This compiles, but may not be correct
    }
    
    void associate(const KeyType& key, const ValueType& value)
    {
        //If tree is empty, just create new Node
        if(m_root==NULL)
        {
            m_root=new Node<KeyType,ValueType>(key,value);
            m_size++;
            return;
        }
        Node<KeyType,ValueType>* cur = m_root;
        for(;;)
        {
            if(cur->m_key==key)
            {
                cur->m_value=value;
                return;
            }
            if(key>cur->m_key)
            {
                if(cur->m_right!=NULL)
                    cur=cur->m_right;
                else
                {
                    cur->m_right = new Node<KeyType,ValueType>(key,value);
                    break;
                }
            }
            else if(key<cur->m_key)
            {
                if(cur->m_left!=NULL)
                    cur=cur->m_left;
                else
                {
                    cur->m_left = new Node<KeyType,ValueType>(key,value);
                    break;
                }
            }
        }
        m_size++;
    }
    
    const ValueType* find(const KeyType& key) const
    {
        Node<KeyType,ValueType>* temp = m_root;
        //Navigate the binary search tree
        while(temp!=NULL)
        {
            if(temp->m_key==key)
                return &(temp->m_value);
            else if(key>temp->m_key)
                temp=temp->m_right;
            else
                temp=temp->m_left;
        }
        //Node with key parameter does not exist
        return NULL;
    }
    
    ValueType* find(const KeyType& key)
    {
        // Do not change the implementation of this overload of find
        const MyMap<KeyType,ValueType>* constThis = this;
        return const_cast<ValueType*>(constThis->find(key));
    }
    
    ValueType* getFirst(KeyType& key)
    {
        if(m_root==NULL)
            return NULL;
        l_order.push(m_root);
        if (m_root->m_left!=NULL)
            l_order.push(m_root->m_left);
        if (m_root->m_right!=NULL)
            l_order.push(m_root->m_right);
        key=m_root->m_key;
        return &(m_root->m_value);
    }
    
    ValueType* getNext(KeyType& key)
    {
        if(m_root==NULL)
            return NULL;
        //Pops off front item in queue
        l_order.pop();
        if (l_order.size()==0||m_size<=1)
            return NULL;
        else
        {
            //If new front item has left children or right children, push them into queue
            if (l_order.front()->m_left!=NULL)
                l_order.push(l_order.front()->m_left);
            if (l_order.front()->m_right!=NULL)
                l_order.push(l_order.front()->m_right);
            key=l_order.front()->m_key;
            //Return the m_value of front item
            return &(l_order.front()->m_value);
        }
    }
    
    vector<string> inOrder(Node<int,vector<string> >* cur)
    {
        vector<string> temp;
        if(cur == NULL)
            return temp;
        vector<string> leftvec = inOrder(cur->m_left);
        temp.insert(temp.end(),leftvec.begin(),leftvec.end());
        temp.insert(temp.end(),cur->m_value.begin(),cur->m_value.end());
        vector<string> rightvec = inOrder(cur->m_right);
        temp.insert(temp.end(),rightvec.begin(),rightvec.end());
        return temp;
    }
    
    Node<KeyType,ValueType>* getRoot()
    {
        return m_root;
    }
    
private:
    MyMap(const MyMap &other);
    MyMap &operator=(const MyMap &other);
    // Add needed members
    std::queue<Node<KeyType,ValueType>*> l_order;
    Node<KeyType,ValueType>* m_root;
    int m_size;
};

#endif // MYMAP_INCLUDED

