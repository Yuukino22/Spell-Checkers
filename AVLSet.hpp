// AVLSet.hpp
//
// ICS 46 Winter 2021
// Project #4: Set the Controls for the Heart of the Sun
//
// An AVLSet is an implementation of a Set that is an AVL tree, which uses
// the algorithms we discussed in lecture to maintain balance every time a
// new element is added to the set.  The balancing is actually optional,
// with a bool parameter able to be passed to the constructor to explicitly
// turn the balancing on or off (on is default).  If the balancing is off,
// the AVL tree acts like a binary search tree (e.g., it will become
// degenerate if elements are added in ascending order).
//
// You are not permitted to use the containers in the C++ Standard Library
// (such as std::set, std::map, or std::vector) to store the information
// in your data structure.  Instead, you'll need to implement your AVL tree
// using your own dynamically-allocated nodes, with pointers connecting them,
// and with your own balancing algorithms used.

#ifndef AVLSET_HPP
#define AVLSET_HPP

#include <functional>
#include "Set.hpp"

template<typename ElementType>
struct NodeA
{
    ElementType element;
    NodeA<ElementType>* left;
    NodeA<ElementType>* right;
    NodeA<ElementType>* parent;
};


template <typename ElementType>
class AVLSet : public Set<ElementType>
{
public:
    // A VisitFunction is a function that takes a reference to a const
    // ElementType and returns no value.
    using VisitFunction = std::function<void(const ElementType&)>;

public:
    // Initializes an AVLSet to be empty, with or without balancing.
    explicit AVLSet(bool shouldBalance = true);

    // Cleans up the AVLSet so that it leaks no memory.
    ~AVLSet() noexcept override;

    // Initializes a new AVLSet to be a copy of an existing one.
    AVLSet(const AVLSet& s);

    // Initializes a new AVLSet whose contents are moved from an
    // expiring one.
    AVLSet(AVLSet&& s) noexcept;

    // Assigns an existing AVLSet into another.
    AVLSet& operator=(const AVLSet& s);

    // Assigns an expiring AVLSet into another.
    AVLSet& operator=(AVLSet&& s) noexcept;


    // isImplemented() should be modified to return true if you've
    // decided to implement an AVLSet, false otherwise.
    bool isImplemented() const noexcept override;


    // add() adds an element to the set.  If the element is already in the set,
    // this function has no effect.  This function always runs in O(log n) time
    // when there are n elements in the AVL tree.
    void add(const ElementType& element) override;


    // contains() returns true if the given element is already in the set,
    // false otherwise.  This function always runs in O(log n) time when
    // there are n elements in the AVL tree.
    bool contains(const ElementType& element) const override;


    // size() returns the number of elements in the set.
    unsigned int size() const noexcept override;


    // height() returns the height of the AVL tree.  Note that, by definition,
    // the height of an empty tree is -1.
    int height() const noexcept;


    // preorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by a preorder traversal of the AVL
    // tree.
    void preorder(VisitFunction visit) const;


    // inorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by an inorder traversal of the AVL
    // tree.
    void inorder(VisitFunction visit) const;


    // postorder() calls the given "visit" function for each of the elements
    // in the set, in the order determined by a postorder traversal of the AVL
    // tree.
    void postorder(VisitFunction visit) const;


private:
    // You'll no doubt want to add member variables and "helper" member
    // functions here.
    bool balance;
    int sz;
    NodeA<ElementType>* root;
    void deleteHelper(NodeA<ElementType>* node);
    void copyHelper(NodeA<ElementType>* node, NodeA<ElementType>* parent);
    int heightHelper(NodeA<ElementType>* node) const;
    void preorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const;
    void inorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const;
    void postorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const;
    NodeA<ElementType>* llRotation(NodeA<ElementType>* node);
    NodeA<ElementType>* lrRotation(NodeA<ElementType>* node);
    NodeA<ElementType>* rlRotation(NodeA<ElementType>* node);
    NodeA<ElementType>* rrRotation(NodeA<ElementType>* node);
};


template <typename ElementType>
AVLSet<ElementType>::AVLSet(bool shouldBalance)
    :balance{shouldBalance}, sz{0}, root{nullptr}
{
}


template <typename ElementType>
AVLSet<ElementType>::~AVLSet() noexcept
{
    deleteHelper(root);
}


template <typename ElementType>
AVLSet<ElementType>::AVLSet(const AVLSet& s)
{
    balance = s.balance;
    sz = s.sz;
    root = nullptr;
    if(s.root != nullptr)
    {
        root = new NodeA<ElementType>{s.root->element, nullptr, nullptr, nullptr};
        copyHelper(s.root->left, root);
        copyHelper(s.root->right, root);
    }
}


template <typename ElementType>
AVLSet<ElementType>::AVLSet(AVLSet&& s) noexcept
{
    balance = std::move(s.balance);
    sz = std::move(s.sz);
    root = s.root;
    s.root = nullptr;
}


template <typename ElementType>
AVLSet<ElementType>& AVLSet<ElementType>::operator=(const AVLSet& s)
{
    if(this != &s)
    {
        deleteHelper(root);
        balance = s.balance;
        sz = s.sz;
        root = nullptr;
        if(s.root != nullptr)
        {
            root = new NodeA<ElementType>{s.root->element, nullptr, nullptr, nullptr};
            copyHelper(s.root->left, root);
            copyHelper(s.root->right, root);
        }
    } 
    return *this;
}


template <typename ElementType>
AVLSet<ElementType>& AVLSet<ElementType>::operator=(AVLSet&& s) noexcept
{
    deleteHelper(root);
    balance = std::move(s.balance);
    sz = std::move(s.sz);
    root = s.root;
    s.root = nullptr;
    return *this;
}


template <typename ElementType>
bool AVLSet<ElementType>::isImplemented() const noexcept
{
    return true;
}


template <typename ElementType>
void AVLSet<ElementType>::add(const ElementType& element)
{
    bool f = false;
    NodeA<ElementType>* current = root;
    if(root == nullptr)
    {
        root = new NodeA<ElementType>{element, nullptr, nullptr, nullptr};
        sz++;
        f = true;
    }
    else
    {
        while(true)
        {
            if(current->element > element)
            {
                if(current->left == nullptr)
                {
                    current->left = new NodeA<ElementType>{element, nullptr, nullptr, current};
                    sz++;
                    f = true;
                    break;
                }
                else
                {
                    current = current->left;
                }  
            }
            else if(current->element < element)
            {
                if(current->right == nullptr)
                {
                    current->right = new NodeA<ElementType>{element, nullptr, nullptr, current};
                    sz++;
                    f = true;
                    break;
                }
                else
                {
                    current = current->right;
                }
            }
            else
            {
                break;
            } 
        }
    }
    if(balance)
    {   
        if(f)
        {
            while(current != nullptr)
            {
                int dif = heightHelper(current->left) - heightHelper(current->right);
                if(dif > 1 || dif < -1)
                {
                    if(current->element > element)
                    {
                        if(current->left->element > element)
                        {
                            if(current == root)
                            {
                                root = llRotation(root);
                            }
                            else
                            {
                                llRotation(current);
                            }
                            //std::cout << "ll" << std::endl;
                        }
                        else if(current->left->element < element)
                        {
                            if(current == root)
                            {
                                root = lrRotation(root);
                            }
                            else
                            {
                                lrRotation(current);
                            }
                            //std::cout << "lr" << std::endl;
                        }
                    }
                    else if(current->element < element)
                    {
                        if(current->right->element < element)
                        {
                            if(current == root)
                            {
                                root = rrRotation(root);
                            }
                            else
                            {
                                rrRotation(current);
                            }
                            //std::cout << "rr" << std::endl;
                        }
                        else if(current->right->element > element)
                        {
                            if(current == root)
                            {
                                root = rlRotation(root);
                            }
                            else
                            {
                                rlRotation(current);
                            }
                            //std::cout << "rl" << std::endl;
                        }
                    }
                }
                current = current->parent;
            }
        }
    }
}


template <typename ElementType>
bool AVLSet<ElementType>::contains(const ElementType& element) const
{
    NodeA<ElementType>* current = root;
    while(current != nullptr)
    {
        if(current->element == element)
        {
            return true;
        }
        else if (current->element > element)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        } 
    }
    return false;
}


template <typename ElementType>
unsigned int AVLSet<ElementType>::size() const noexcept
{
    return sz;
}


template <typename ElementType>
int AVLSet<ElementType>::height() const noexcept
{
    return heightHelper(root);
}


template <typename ElementType>
void AVLSet<ElementType>::preorder(VisitFunction visit) const
{
    if(root != nullptr)
    {
        preorderHelper(root, visit);
    }
}


template <typename ElementType>
void AVLSet<ElementType>::inorder(VisitFunction visit) const
{
    if(root != nullptr)
    {
        inorderHelper(root, visit);
    }
}


template <typename ElementType>
void AVLSet<ElementType>::postorder(VisitFunction visit) const
{
    if(root != nullptr)
    {
        postorderHelper(root, visit);
    }
}

template <typename ElementType>
void AVLSet<ElementType>::deleteHelper(NodeA<ElementType>* node)
{
    if(node != nullptr)
    {
        //std::cout << node->element << std::endl;
        deleteHelper(node->left);
        deleteHelper(node->right);
        delete node;
    }
}

template <typename ElementType>
void AVLSet<ElementType>::copyHelper(NodeA<ElementType>* node, NodeA<ElementType>* parent)
{
    if(node != nullptr)
    {
        NodeA<ElementType>* newNode = new NodeA<ElementType>{node->element, nullptr, nullptr, parent};
        if(node->element < parent->element)
        {
            parent->left = newNode;
        }
        else
        {
            parent->right = newNode;
        }
        copyHelper(node->left, newNode);
        copyHelper(node->right, newNode);
    }
}

template <typename ElementType>
int AVLSet<ElementType>::heightHelper(NodeA<ElementType>* node) const
{
    if(node == nullptr)
    {
        return -1;
    }
    else
    {
        int l = heightHelper(node->left);
        int r = heightHelper(node->right);
        if(l > r)
        {
            return l + 1;
        }
        else
        {
            return r + 1;
        }
    }
    return -1;
}

template <typename ElementType>
void AVLSet<ElementType>::preorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const
{
    visit(node->element);
    if(node->left != nullptr)
    {
        preorderHelper(node->left, visit);
    }
    if(node->right != nullptr)
    {
        preorderHelper(node->right, visit);
    }
}

template <typename ElementType>
void AVLSet<ElementType>::inorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const
{
    if(node->left != nullptr)
    {
        inorderHelper(node->left, visit);
    }
    visit(node->element);
    if(node->right != nullptr)
    {
        inorderHelper(node->right, visit);
    }
}

template <typename ElementType>
void AVLSet<ElementType>::postorderHelper(NodeA<ElementType>* node, VisitFunction& visit) const
{
    if(node->left != nullptr)
    {
        postorderHelper(node->left, visit);
    }
    if(node->right != nullptr)
    {
        postorderHelper(node->right, visit);
    }
    visit(node->element);
}

template <typename ElementType>
NodeA<ElementType>* AVLSet<ElementType>::llRotation(NodeA<ElementType>* node)
{
    NodeA<ElementType>* B = node;
    NodeA<ElementType>* A = node->left;
    NodeA<ElementType>* T2 = A->right;
    B->left = T2;
    if(T2 != nullptr)
    {
        T2->parent = B;
    }
    A->right = B;
    A->parent = B->parent;
    if(B->parent != nullptr)
    {
        if(B->parent->element > B->element)
        {
            B->parent->left = A;
        }
        else
        {
            B->parent->right = A;
        }
    }
    B->parent = A;
    return A;
}

template <typename ElementType>
NodeA<ElementType>* AVLSet<ElementType>::lrRotation(NodeA<ElementType>* node)
{
    NodeA<ElementType>* C = node;
    NodeA<ElementType>* B = node->left->right;
    NodeA<ElementType>* A = node->left;
    NodeA<ElementType>* T2 = B->left;
    NodeA<ElementType>* T3 = B->right;
    A->right = T2;
    C->left = T3;
    if(T2 != nullptr)
    {
        T2->parent = A;
    }
    if(T3 != nullptr)
    {
        T3->parent = C;
    }
    if(C->parent != nullptr)
    {
        if(C->parent->element > C->element)
        {
            C->parent->left = B;
        }
        else
        {
            C->parent->right = B;
        }
    }
    B->parent = C->parent;
    B->left = A;
    B->right = C;
    A->parent = B;
    C->parent = B;
    return B;
}

template <typename ElementType>
NodeA<ElementType>* AVLSet<ElementType>::rlRotation(NodeA<ElementType>* node)
{
    NodeA<ElementType>* A = node;
    NodeA<ElementType>* B = node->right->left;
    NodeA<ElementType>* C = node->right;
    NodeA<ElementType>* T2 = B->left;
    NodeA<ElementType>* T3 = B->right;
    A->right = T2;
    C->left = T3;
    if(T2 != nullptr)
    {
        T2->parent = A;
    }
    if(T3 != nullptr)
    {
        T3->parent = C;
    }
    if(A->parent != nullptr)
    {
        if(A->parent->element > A->element)
        {
            A->parent->left = B;
        }
        else
        {
            A->parent->right = B;
        }
    }
    B->parent = A->parent;
    B->left = A;
    B->right = C;
    A->parent = B;
    C->parent = B;
    return B;
}

template <typename ElementType>
NodeA<ElementType>* AVLSet<ElementType>::rrRotation(NodeA<ElementType>* node)
{
    NodeA<ElementType>* A = node;
    NodeA<ElementType>* B = node->right;
    NodeA<ElementType>* T2 = B->left;
    A->right = T2;
    if(T2 != nullptr)
    {
        T2->parent = A;
    }
    B->left = A;
    B->parent = A->parent;
    if(A->parent != nullptr)
    {
        if(A->parent->element > A->element)
        {
            A->parent->left = B;
        }
        else
        {
            A->parent->right = B;
        }
    }
    A->parent = B;
    return B;
}

#endif // AVLSET_HPP

