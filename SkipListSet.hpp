// SkipListSet.hpp
//
// ICS 46 Winter 2021
// Project #4: Set the Controls for the Heart of the Sun
//
// A SkipListSet is an implementation of a Set that is a skip list, implemented
// as we discussed in lecture.  A skip list is a sequence of levels
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the keys and their
// values.  Instead, you'll need to implement your own dynamically-allocated
// nodes, with pointers connecting them.  You can, however, use other parts of
// the C++ Standard Library -- including <random>, notably.
//
// Each node should contain only two pointers: one to the node that follows it
// on the same level and another to the equivalent node on the level below it.
// Additional pointers use more memory but don't enable any techniques not
// enabled by the other two.
//
// A couple of utilities are included here: SkipListKind and SkipListKey.
// You can feel free to use these as-is and probably will not need to
// modify them, though you can make changes to them, if you'd like.

#ifndef SKIPLISTSET_HPP
#define SKIPLISTSET_HPP

#include <memory>
#include <random>
#include "Set.hpp"



// SkipListKind indicates a kind of key: a normal one, the special key
// -INF, or the special key +INF.  It's necessary for us to implement
// the notion of -INF and +INF separately, since we're building a class
// template and not all types of keys would have a reasonable notion of
// -INF and +INF.

enum class SkipListKind
{
    Normal,
    NegInf,
    PosInf
};




// A SkipListKey represents a single key in a skip list.  It is possible
// to compare these keys using < or == operators (which are overloaded here)
// and those comparisons respect the notion of whether each key is normal,
// -INF, or +INF.

template <typename ElementType>
class SkipListKey
{
public:
    SkipListKey(SkipListKind kind, const ElementType& element);

    bool operator==(const SkipListKey& other) const;
    bool operator<(const SkipListKey& other) const;

private:
    SkipListKind kind;
    ElementType element;
};


template <typename ElementType>
SkipListKey<ElementType>::SkipListKey(SkipListKind kind, const ElementType& element)
    : kind{kind}, element{element}
{
}


template <typename ElementType>
bool SkipListKey<ElementType>::operator==(const SkipListKey& other) const
{
    return kind == other.kind
        && (kind != SkipListKind::Normal || element == other.element);
}


template <typename ElementType>
bool SkipListKey<ElementType>::operator<(const SkipListKey& other) const
{
    switch (kind)
    {
    case SkipListKind::NegInf:
        return other.kind != SkipListKind::NegInf;

    case SkipListKind::PosInf:
        return false;

    default: // SkipListKind::Normal
        return other.kind == SkipListKind::PosInf
            || (other.kind == SkipListKind::Normal && element < other.element);
    }
}

template <typename ElementType>
struct Node
{
    SkipListKey<ElementType> key;
    Node<ElementType>* bottom;
    Node<ElementType>* right;
};

// The SkipListLevelTester class represents the ability to decide whether
// a key placed on one level of the skip list should also occupy the next
// level.  This is the "coin flip," so to speak.  Note that this is an
// abstract base class with one implementation, RandomSkipListLevelTester,
// just below it.  RandomSkipListLevelTester is what it sounds like: It
// makes the decision at random (with a 50/50 chance of deciding whether
// a key should occupy the next level).  However, by setting things up
// this way, we have a way to control things more carefully in our
// testing (as you can, as well).
//
// DO NOT MAKE CHANGES TO THE SIGNATURES OF THE MEMBER FUNCTIONS OF
// THE "level tester" CLASSES.  You can add new member functions or even
// whole new level tester classes, but the ones declared below are part
// of how we test your implementation, so they need to maintain the
// parts of the interface that are declared already.

template <typename ElementType>
class SkipListLevelTester
{
public:
    virtual ~SkipListLevelTester() = default;

    virtual bool shouldOccupyNextLevel(const ElementType& element) = 0;
    virtual std::unique_ptr<SkipListLevelTester<ElementType>> clone() = 0;
};


template <typename ElementType>
class RandomSkipListLevelTester : public SkipListLevelTester<ElementType>
{
public:
    RandomSkipListLevelTester();

    bool shouldOccupyNextLevel(const ElementType& element) override;
    std::unique_ptr<SkipListLevelTester<ElementType>> clone() override;

private:
    std::default_random_engine engine;
    std::bernoulli_distribution distribution;
};


template <typename ElementType>
RandomSkipListLevelTester<ElementType>::RandomSkipListLevelTester()
    : engine{std::random_device{}()}, distribution{0.5}
{
}


template <typename ElementType>
bool RandomSkipListLevelTester<ElementType>::shouldOccupyNextLevel(const ElementType& element)
{
    return distribution(engine);
}


template <typename ElementType>
std::unique_ptr<SkipListLevelTester<ElementType>> RandomSkipListLevelTester<ElementType>::clone()
{
    return std::unique_ptr<SkipListLevelTester<ElementType>>{
        new RandomSkipListLevelTester<ElementType>};
}




template <typename ElementType>
class SkipListSet : public Set<ElementType>
{
public:
    // Initializes an SkipListSet to be empty, with or without a
    // "level tester" object that will decide, whenever a "coin flip"
    // is needed, whether a key should occupy the next level above.
    SkipListSet();
    explicit SkipListSet(std::unique_ptr<SkipListLevelTester<ElementType>> levelTester);

    // Cleans up the SkipListSet so that it leaks no memory.
    ~SkipListSet() noexcept override;

    // Initializes a new SkipListSet to be a copy of an existing one.
    SkipListSet(const SkipListSet& s);

    // Initializes a new SkipListSet whose contents are moved from an
    // expiring one.
    SkipListSet(SkipListSet&& s) noexcept;

    // Assigns an existing SkipListSet into another.
    SkipListSet& operator=(const SkipListSet& s);

    // Assigns an expiring SkipListSet into another.
    SkipListSet& operator=(SkipListSet&& s) noexcept;


    // isImplemented() should be modified to return true if you've
    // decided to implement a SkipListSet, false otherwise.
    bool isImplemented() const noexcept override;


    // add() adds an element to the set.  If the element is already in the set,
    // this function has no effect.  This function runs in an expected time
    // of O(log n) (i.e., over the long run, we expect the average to be
    // O(log n)) with very high probability.
    void add(const ElementType& element) override;


    // contains() returns true if the given element is already in the set,
    // false otherwise.  This function runs in an expected time of O(log n)
    // (i.e., over the long run, we expect the average to be O(log n))
    // with very high probability.
    bool contains(const ElementType& element) const override;


    // size() returns the number of elements in the set.
    unsigned int size() const noexcept override;


    // levelCount() returns the number of levels in the skip list.
    unsigned int levelCount() const noexcept;


    // elementsOnLevel() returns the number of elements that are stored
    // on the given level of the skip list.  Level 0 is the bottom level;
    // level 1 is the one above level 0; and so on.  If the given level
    // doesn't exist, this function returns 0.  (Note that the -INF
    // and +INF shouldn't be counted.)
    unsigned int elementsOnLevel(unsigned int level) const noexcept;


    // isElementOnLevel() returns true if the given element is on the
    // given level, false otherwise.  Level 0 is the bottom level; level 1
    // is the one above level 0; and so on.  If the given level doesn't
    // exist, this function returns false.
    bool isElementOnLevel(const ElementType& element, unsigned int level) const;


private:
    std::unique_ptr<SkipListLevelTester<ElementType>> levelTester;
    int sz = 0;
    int lv = 0;
    Node<ElementType>* head;
    Node<ElementType>* tail;
    Node<ElementType>* topHead;
    Node<ElementType>* topTail;
};



template <typename ElementType>
SkipListSet<ElementType>::SkipListSet()
    : levelTester{std::make_unique<RandomSkipListLevelTester<ElementType>>()}, head{nullptr}, tail{nullptr}
{
    try
    {
        tail = new Node<ElementType>{SkipListKey(SkipListKind::PosInf, ElementType()), nullptr, nullptr};
        head = new Node<ElementType>{SkipListKey(SkipListKind::NegInf, ElementType()), nullptr, tail};
    }
    catch(...)
    {
        delete head;
        delete tail;
        throw;
    }
    topHead = head;
    topTail = tail;
}


template <typename ElementType>
SkipListSet<ElementType>::SkipListSet(std::unique_ptr<SkipListLevelTester<ElementType>> levelTester)
    : levelTester{std::move(levelTester)}, head{nullptr}, tail{nullptr}
{
    try
    {
        tail = new Node<ElementType>{SkipListKey(SkipListKind::PosInf, ElementType()), nullptr, nullptr};
        head = new Node<ElementType>{SkipListKey(SkipListKind::NegInf, ElementType()), nullptr, tail}; 
    }
    catch(...)
    {
        delete head;
        delete tail;
        throw;
    }
    topHead = head;
    topTail = tail;
}


template <typename ElementType>
SkipListSet<ElementType>::~SkipListSet() noexcept
{
    for(int i = 0; i < lv + 1; i++)
    {
        Node<ElementType>* current = topHead;
        for(int j = 0; j < lv - i; j++)
        {
            current = current->bottom;
        }
        if(current == nullptr)
        {
            break;
        }
        Node<ElementType>* currentRight = current->right;
        while(currentRight != nullptr)
        {
            delete current;
            current = currentRight;
            currentRight = currentRight->right;
        }
        delete current;
    }
}


template <typename ElementType>
SkipListSet<ElementType>::SkipListSet(const SkipListSet& s)
{
    levelTester = s.levelTester->clone();
    sz = s.sz;
    lv = s.lv;
    head = new Node<ElementType>{s.head->key, nullptr, nullptr};
    tail = new Node<ElementType>{s.tail->key, nullptr, nullptr};
    topHead = head;
    topTail = tail;
    for(int i = 0; i < s.lv; i++)
    {
        Node<ElementType>* h = new Node<ElementType>{s.head->key, topHead, nullptr};
        Node<ElementType>* t = new Node<ElementType>{s.tail->key, topTail, nullptr};
        topHead = h;
        topTail = t;
    }

    for(int i = 0; i < s.lv + 1; i++)
    {
        Node<ElementType>* temp = s.topHead;
        Node<ElementType>* current = topHead;
        Node<ElementType>* currentTail = topTail;
        for (int j = 0; j < (s.lv - i); j++)
        {
            temp = temp->bottom;
            current = current->bottom;
            currentTail = currentTail->bottom;
        }
        temp = temp->right;
        while(! (temp->key == topTail->key))
        {
            Node<ElementType>* n = new Node<ElementType>{temp->key, nullptr, nullptr};
            current->right = n;
            current = current->right;
            temp = temp->right;
        }
        current->right = currentTail;
    }

    for(int i = 0; i < s.lv; i++)
    {
        Node<ElementType>* current = topHead;
        for (int j = 0; j < (s.lv - i - 1); j++)
        {
            current = current->bottom;
        }
        Node<ElementType>* currentBot = current->bottom;
        while(current != nullptr)
        {
            while(!(current->key == currentBot->key))
            {
                currentBot = currentBot->right;
            }
            current->bottom = currentBot;
            current = current->right;
        }
    }
}


template <typename ElementType>
SkipListSet<ElementType>::SkipListSet(SkipListSet&& s) noexcept
{
    levelTester = std::move(s.levelTester);
    sz = std::move(s.sz);
    lv = std::move(s.lv);
    head = s.head;
    tail = s.tail;
    topHead = s.topHead;
    topTail = s.topTail;
    s.head = nullptr;
    s.tail = nullptr;
    s.topHead = nullptr;
    s.topTail = nullptr;
}


template <typename ElementType>
SkipListSet<ElementType>& SkipListSet<ElementType>::operator=(const SkipListSet& s)
{
    if(this != &s)
    {
        for(int i = 0; i < lv + 1; i++)
        {
            Node<ElementType>* current = topHead;
            for(int j = 0; j < lv - i; j++)
            {
                current = current->bottom;
            }
            if(current == nullptr)
            {
                break;
            }
            Node<ElementType>* currentRight = current->right;
            while(currentRight != nullptr)
            {
                delete current;
                current = currentRight;
                currentRight = currentRight->right;
            }
            delete current;
        }
        levelTester = s.levelTester->clone();
        sz = s.sz;
        lv = s.lv;
        head = new Node<ElementType>{s.head->key, nullptr, nullptr};
        tail = new Node<ElementType>{s.tail->key, nullptr, nullptr};
        topHead = head;
        topTail = tail;
        for(int i = 0; i < s.lv; i++)
        {
            Node<ElementType>* h = new Node<ElementType>{s.head->key, topHead, nullptr};
            Node<ElementType>* t = new Node<ElementType>{s.tail->key, topTail, nullptr};
            topHead = h;
            topTail = t;
        }

        for(int i = 0; i < s.lv + 1; i++)
        {
            Node<ElementType>* temp = s.topHead;
            Node<ElementType>* current = topHead;
            Node<ElementType>* currentTail = topTail;
            for (int j = 0; j < (s.lv - i); j++)
            {
                temp = temp->bottom;
                current = current->bottom;
                currentTail = currentTail->bottom;
            }
            temp = temp->right;
            while(!(temp->key == topTail->key))
            {
                Node<ElementType>* n = new Node<ElementType>{temp->key, nullptr, nullptr};
                current->right = n;
                current = current->right;
                temp = temp->right;
            }
            current->right = currentTail;
        }

        for(int i = 0; i < s.lv; i++)
        {
            Node<ElementType>* current = topHead;
            for (int j = 0; j < (s.lv - i - 1); j++)
            {
                current = current->bottom;
            }
            Node<ElementType>* currentBot = current->bottom;
            while(current != nullptr)
            {
                while(!(current->key == currentBot->key))
                {
                    currentBot = currentBot->right;
                }
                current->bottom = currentBot;
                current = current->right;
            }
        }
    }
    return *this;
}


template <typename ElementType>
SkipListSet<ElementType>& SkipListSet<ElementType>::operator=(SkipListSet&& s) noexcept
{
    for(int i = 0; i < lv + 1; i++)
    {
        Node<ElementType>* current = topHead;
        for(int j = 0; j < lv - i; j++)
        {
            current = current->bottom;
        }
        if(current == nullptr)
        {
            break;
        }
        Node<ElementType>* currentRight = current->right;
        while(currentRight != nullptr)
        {
            delete current;
            current = currentRight;
            currentRight = currentRight->right;
        }
        delete current;
    }
    levelTester = std::move(s.levelTester);
    sz = std::move(s.sz);
    lv = std::move(s.lv);
    head = s.head;
    tail = s.tail;
    topHead = s.topHead;
    topTail = s.topTail;
    s.head = nullptr;
    s.tail = nullptr;
    s.topHead = nullptr;
    s.topTail = nullptr;
    return *this;
}


template <typename ElementType>
bool SkipListSet<ElementType>::isImplemented() const noexcept
{
    return true;
}


template <typename ElementType>
void SkipListSet<ElementType>::add(const ElementType& element)
{
    Node<ElementType>* current = topHead;
    SkipListKey s{SkipListKind::Normal, element};
    while(true)
    {
        if (current->key == s)
        {
            break;
        }
        else if (s < current->right->key)
        {
            if(current->bottom == nullptr)
            {
                Node<ElementType>* n = new Node<ElementType>{s, nullptr, current->right};
                current->right = n;
                break;
            }
            current = current->bottom;
        }
        else
        {
            current = current->right;
        }
    }
    int level = 0;
    if (current->right->key == s)
    {
        Node<ElementType>* c = current->right;
        while(levelTester->shouldOccupyNextLevel(element))
        {
            level++;
            if(level > lv)
            {
                lv = level;
                Node<ElementType>* newTopTail = new Node<ElementType>{SkipListKey(SkipListKind::PosInf, ElementType()), topTail, nullptr};
                Node<ElementType>* nl = new Node<ElementType>{s, c, newTopTail};
                Node<ElementType>* newTopHead = new Node<ElementType>{SkipListKey(SkipListKind::NegInf, ElementType()), topHead, nl};
                c = nl;
                topHead = newTopHead;
                topTail = newTopTail;
            }
            else
            {
                Node<ElementType>* temp = topHead;
                int count = 0;
                while(count < lv - level)
                {
                    if(temp->right->key < s)
                    {
                        temp = temp->right;
                    }
                    else
                    {
                        temp = temp->bottom;
                        count++;
                    } 
                }
                while(temp->right->key < s)
                {
                    temp = temp->right;
                }
                Node<ElementType>* nl = new Node<ElementType>{s, c, temp->right};
                temp->right = nl;
                c = nl;
            } 
        }
    }
    sz++;
}


template <typename ElementType>
bool SkipListSet<ElementType>::contains(const ElementType& element) const
{
    Node<ElementType>* current = topHead;
    SkipListKey s{SkipListKind::Normal, element};
    while(true)
    {
        if (current->key == s)
        {
            return true;
        }
        else if (s < current->right->key)
        {
            if(current->bottom == nullptr)
            {
                break;
            }
            current = current->bottom;
        }
        else
        {
            current = current->right;
        }
    }
    return false;
}


template <typename ElementType>
unsigned int SkipListSet<ElementType>::size() const noexcept
{
    return sz;
}


template <typename ElementType>
unsigned int SkipListSet<ElementType>::levelCount() const noexcept
{
    return lv + 1;
}


template <typename ElementType>
unsigned int SkipListSet<ElementType>::elementsOnLevel(unsigned int level) const noexcept
{
    if(level > lv)
    {
        return 0;
    }
    int result = 0;
    Node<ElementType>* temp = topHead;
    for (int i  = 0; i < (lv - level); i++)
    {
        temp = temp->bottom;
    }
    while(temp != nullptr)
    {
        result++;
        temp = temp->right;
    }
    return result - 2;
}


template <typename ElementType>
bool SkipListSet<ElementType>::isElementOnLevel(const ElementType& element, unsigned int level) const
{
    if(level > lv)
    {
        return false;
    }
    Node<ElementType>* temp = topHead;
    SkipListKey s{SkipListKind::Normal, element};
    for (int i  = 0; i < (lv - level); i++)
    {
        temp = temp->bottom;
    }
    while(temp != nullptr)
    {
        if(s == temp->key)
        {
            return true;
        }
        temp = temp->right;
    }
    return false;
}



#endif // SKIPLISTSET_HPP

