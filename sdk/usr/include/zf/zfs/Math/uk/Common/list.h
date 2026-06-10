#ifndef my_list_H
#define my_list_H


// Declare templates
template <class T> 
class List;

template <class T>
class Node;

template <class T>
class MyNode;

template <class T>
class Root {
protected:
    List <T> *listPtr;
    Node <T> *nodePtr;
    Root <T> *next;
public:
    Root(void);
    Root(List <T> *initListPtr, Node<T> *initNodePtr);
          ~Root(void); 

    friend class MyNode<T>;
};

template <class T>
class MyNode {
protected:
          Root<T> *rootNode;

public:
    void disconnect(void);
    Node <T> *getRoot(const List <T> *ListPtr);
    void addRoot(List <T> *ListPtr, Node <T> *NodePtr);
    int deleteRoot(List <T> *ListPtr, Node <T> *NodePtr);

    MyNode(void);
    ~MyNode(void);
    
    virtual void displayNode(void);
    
};


template <class T>
class Node {
public:
    T           *item;
    List<T>     *parent;
    Node        *next;
    Node        *prev;

    Node(void);
    Node(List<T> *newParent, T *newItem);
    virtual ~Node(void);

    Node<T>*     getNodePrev(void);
    Node<T>*     getNodeNext(void);

    friend class List<T>;
    int displayNode(void);

};

template <class T>
class List {
public:
    int         verbose;
private:

public:
    Node<T>     *head;
    Node<T>     *tail;
    int         length;

    List(void);
    ~List(void);

    int         displayNode(void);

    int         initialise( int freeEnts = 1 );

    void            printNode(int n);

    Node<T>*        addHead(T *);

    // ???? Test code 
    Node<T>*        addHead(Node<T>*, T *);

    Node<T>*    addTail(T *);
    Node<T>*        insertNode(Node<T>*, T *);

    Node<T>*        moveNode(Node<T>*, int distance);

    Node<T>*        getNode( T *) const;
    Node<T>*        getNode(int n) const;

    int             getNodeNumber(Node<T> *n);
    int             getLength(void) const {return length;}

    int         deleteNode(Node<T>*);
    int             deleteNode(T *);

    int         deleteHead(void);
    int         deleteTail(void);

    // Connects onto another list of the same type
    void            connectToTail(List<T> &);
    void            connectToHead(List<T> &);

    List<T>*        splitListAtNode(Node<T>*);
};

#endif
