// HashSet.hpp
//
// ICS 46 Winter 2021
// Project #4: Set the Controls for the Heart of the Sun
//
// A HashSet is an implementation of a Set that is a separately-chained
// hash table, implemented as a dynamically-allocated array of linked
// lists.  At any given time, the HashSet has a "size" indicating
// how many elements are stored within it, along with a "capacity"
// indicating the size of the array.
//
// As elements are added to the HashSet and the proportion of the HashSet's
// size to its capacity exceeds 0.8 (i.e., there are more than 80% as many
// elements as there are array cells), the HashSet should be resized so
// that it is twice as large as it was before.
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the information
// in your data structure.  Instead, you'll need to use a dynamically-
// allocated array and your own linked list implemenation; the linked list
// doesn't have to be its own class, though you can do that, if you'd like.

#ifndef HASHSET_HPP
#define HASHSET_HPP

#include <functional>
#include "Set.hpp"

template <typename ElementType>
struct NodeH
{
    ElementType element;
    NodeH<ElementType>* next;
};

template <typename ElementType>
class HashSet : public Set<ElementType>
{
public:
    // The default capacity of the HashSet before anything has been
    // added to it.
    static constexpr unsigned int DEFAULT_CAPACITY = 10;

    // A HashFunction is a function that takes a reference to a const
    // ElementType and returns an unsigned int.
    using HashFunction = std::function<unsigned int(const ElementType&)>;

public:
    // Initializes a HashSet to be empty, so that it will use the given
    // hash function whenever it needs to hash an element.
    explicit HashSet(HashFunction hashFunction);

    // Cleans up the HashSet so that it leaks no memory.
    ~HashSet() noexcept override;

    // Initializes a new HashSet to be a copy of an existing one.
    HashSet(const HashSet& s);

    // Initializes a new HashSet whose contents are moved from an
    // expiring one.
    HashSet(HashSet&& s) noexcept;

    // Assigns an existing HashSet into another.
    HashSet& operator=(const HashSet& s);

    // Assigns an expiring HashSet into another.
    HashSet& operator=(HashSet&& s) noexcept;


    // isImplemented() should be modified to return true if you've
    // decided to implement a HashSet, false otherwise.
    bool isImplemented() const noexcept override;


    // add() adds an element to the set.  If the element is already in the set,
    // this function has no effect.  This function triggers a resizing of the
    // array when the ratio of size to capacity would exceed 0.8, in which case
    // the new capacity should be determined by this formula:
    //
    //     capacity * 2 + 1
    //
    // In the case where the array is resized, this function runs in linear
    // time (with respect to the number of elements, assuming a good hash
    // function); otherwise, it runs in constant time (again, assuming a good
    // hash function).  The amortized running time is also constant.
    void add(const ElementType& element) override;


    // contains() returns true if the given element is already in the set,
    // false otherwise.  This function runs in constant time (with respect
    // to the number of elements, assuming a good hash function).
    bool contains(const ElementType& element) const override;


    // size() returns the number of elements in the set.
    unsigned int size() const noexcept override;


    // elementsAtIndex() returns the number of elements that hashed to a
    // particular index in the array.  If the index is out of the boundaries
    // of the array, this function returns 0.
    unsigned int elementsAtIndex(unsigned int index) const;


    // isElementAtIndex() returns true if the given element hashed to a
    // particular index in the array, false otherwise.  If the index is
    // out of the boundaries of the array, this functions returns 0.
    bool isElementAtIndex(const ElementType& element, unsigned int index) const;


private:
    HashFunction hashFunction;
    NodeH<ElementType>** arr;
    int sz;
    int cp;
};



namespace impl_
{
    template <typename ElementType>
    unsigned int HashSet__undefinedHashFunction(const ElementType& element)
    {
        return 0;
    }
}


template <typename ElementType>
HashSet<ElementType>::HashSet(HashFunction hashFunction)
    : hashFunction{hashFunction}, arr{nullptr}, sz{0}, cp{DEFAULT_CAPACITY}
{
    try
    {
        arr = new NodeH<ElementType>*[DEFAULT_CAPACITY]{nullptr};
    }
    catch(...)
    {
        delete[] arr;
        throw;
    }   
}


template <typename ElementType>
HashSet<ElementType>::~HashSet() noexcept
{
    if(arr != nullptr)
    {
        for(int i = 0; i < cp; i++)
        {
            NodeH<ElementType>* current = arr[i];
            while(current != nullptr)
            {
                NodeH<ElementType>* c = current->next;
                delete current;
                current = c;
            }
        }
    }
    delete[] arr;
}


template <typename ElementType>
HashSet<ElementType>::HashSet(const HashSet& s)
    : hashFunction{s.hashFunction}, arr{nullptr},sz{s.sz}, cp{s.cp}
{
    try
    {
        arr = new NodeH<ElementType>*[cp]{nullptr};
    }
    catch(...)
    {
        delete[] arr;
        throw;
    }
    for(int i = 0; i < cp; i++)
    {
        NodeH<ElementType>* current = arr[i];
        NodeH<ElementType>* source = s.arr[i];
        if(source != nullptr)
        {
            current = new NodeH<ElementType>{source->element, nullptr};
            source = source->next;
        }
        while(source != nullptr)
        {
            current->next = new NodeH<ElementType>{source->element, nullptr};
            current = current->next;
            source = source->next;
        }
    }
}


template <typename ElementType>
HashSet<ElementType>::HashSet(HashSet&& s) noexcept
{
    hashFunction = std::move(s.hashFunction);
    sz = std::move(s.sz);
    cp = std::move(s.cp);
    arr = s.arr;
    s.arr = nullptr;
}


template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(const HashSet& s)
{
    if(this != &s)
    {
        for(int i = 0; i < cp; i++)
        {
            NodeH<ElementType>* current = arr[i];
            while(current != nullptr)
            {
                NodeH<ElementType>* c = current->next;
                delete current;
                current = c;
            }
        }
        delete[] arr;
        hashFunction = s.hashFunction;
        sz = s.sz;
        cp = s.cp;
        arr = new NodeH<ElementType>*[cp]{nullptr};
        for(int i = 0; i < cp; i++)
        {
            NodeH<ElementType>* current = arr[i];
            NodeH<ElementType>* source = s.arr[i];
            if(source != nullptr)
            {
                current = new NodeH<ElementType>{source->element, nullptr};
                source = source->next;
            }
            while(source != nullptr)
            {
                current->next = new NodeH<ElementType>{source->element, nullptr};
                current = current->next;
                source = source->next;
            }
        }
    }
    return *this;
}


template <typename ElementType>
HashSet<ElementType>& HashSet<ElementType>::operator=(HashSet&& s) noexcept
{
    for(int i = 0; i < cp; i++)
    {
        NodeH<ElementType>* current = arr[i];
        while(current != nullptr)
        {
            NodeH<ElementType>* c = current->next;
            delete current;
            current = c;
        }
    }
    delete[] arr;
    hashFunction = std::move(s.hashFunction);
    sz = std::move(s.sz);
    cp = std::move(s.cp);
    arr = s.arr;
    s.arr = nullptr;
    return *this;
}


template <typename ElementType>
bool HashSet<ElementType>::isImplemented() const noexcept
{
    return true;
}


template <typename ElementType>
void HashSet<ElementType>::add(const ElementType& element)
{
    if(!contains(element))
    {
        sz++;
    
        if((double(sz) / double(cp)) > 0.8)
        {
            int oldCp = cp;
            cp = cp * 2 + 1;
            NodeH<ElementType>** newArr = new NodeH<ElementType>*[cp]{nullptr};
            for(int i = 0; i < oldCp; i++)
            {
                NodeH<ElementType>* current = arr[i];
                while(current != nullptr)
                {
                    int newIndex = hashFunction(current->element) % cp;
                    NodeH<ElementType>* newCur = newArr[newIndex];
                    if(newCur == nullptr)
                    {
                        newArr[newIndex] = new NodeH<ElementType>{current->element, nullptr};
                    }
                    else
                    {
                        while(newCur-> next != nullptr)
                        {
                            newCur = newCur->next;
                        }
                        newCur->next = new NodeH<ElementType>{current->element, nullptr};
                    }
                    current = current->next;
                }
            }
            for(int i = 0; i < oldCp; i++)
            {
                NodeH<ElementType>* current = arr[i];
                while(current != nullptr)
                {
                    NodeH<ElementType>* c = current->next;
                    delete current;
                    current = c;
                }
            }
            delete[] arr;
            arr = newArr;
        }
        int index = hashFunction(element) % cp;
        NodeH<ElementType>* current = arr[index];
        if(current == nullptr)
        {
            arr[index] = new NodeH<ElementType>{element, nullptr};
        }
        else
        {
            while(current->next != nullptr)
            {
                current = current->next;
            }
            current->next = new NodeH<ElementType>{element, nullptr};
        }
    }
}


template <typename ElementType>
bool HashSet<ElementType>::contains(const ElementType& element) const
{
    int index = hashFunction(element) % cp;
    NodeH<ElementType>* current = arr[index];
    while(current != nullptr)
    {
        if(current->element == element)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}


template <typename ElementType>
unsigned int HashSet<ElementType>::size() const noexcept
{
    return sz;
}


template <typename ElementType>
unsigned int HashSet<ElementType>::elementsAtIndex(unsigned int index) const
{
    int result = 0;
    if (index > cp)
    {
        return result;
    }
    NodeH<ElementType>* current = arr[index];
    while(current != nullptr)
    {
        result++;
        current = current->next;
    }
    return result;
}


template <typename ElementType>
bool HashSet<ElementType>::isElementAtIndex(const ElementType& element, unsigned int index) const
{
    if(index > cp)
    {
        return false;
    }
    NodeH<ElementType>* current = arr[index];
    while(current != nullptr)
    {
        if(current->element == element)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}



#endif // HASHSET_HPP

