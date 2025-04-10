#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;
};

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent)
    : Node<Key, Value>(key, value, parent), balance_(0) {}

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode() {}

template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const { return balance_; }

template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance) { balance_ = balance; }

template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff) { balance_ += diff; }

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const {
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const {
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const {
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item) override;
    virtual void remove(const Key& key) override;

protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);

    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalanceInsert(AVLNode<Key, Value>* node);
    void rebalanceRemove(AVLNode<Key, Value>* node, int8_t diff);
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    if(this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (curr != nullptr) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            curr->setValue(new_item.second); // overwrite
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    rebalanceInsert(newNode);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceInsert(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    
    while (parent != nullptr) {
        if (node == parent->getLeft()) parent->updateBalance(-1);
        else parent->updateBalance(1);

        int8_t balance = parent->getBalance();

        if (balance == 0) break;
        else if (balance == -2) {
            if (node->getBalance() == 1) rotateLeft(node);
            rotateRight(parent);
            break;
        }
        else if (balance == 2) {
            if (node->getBalance() == -1) rotateRight(node);
            rotateLeft(parent);
            break;
        }

        node = parent;
        parent = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    
    if (!node) return;

    AVLNode<Key, Value>* replaceWith = node;
    AVLNode<Key, Value>* child = nullptr;
    AVLNode<Key, Value>* fixNode = nullptr;
    int8_t diff = 0;

    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
    }

    replaceWith = static_cast<AVLNode<Key, Value>*>(node);
    AVLNode<Key, Value>* parent = replaceWith->getParent();

    if (replaceWith->getLeft() != nullptr) {
        child = replaceWith->getLeft();
    } else {
        child = replaceWith->getRight();
    }

    if (child != nullptr) {
        child->setParent(parent);
    }

    if (parent == nullptr) {
        this->root_ = child;
    } else {
        if (parent->getLeft() == replaceWith) {
            parent->setLeft(child);
            diff = 1;
        } else {
            parent->setRight(child);
            diff = -1;
        }
    }

    fixNode = parent;
    delete replaceWith;

    rebalanceRemove(fixNode, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceRemove(AVLNode<Key, Value>* node, int8_t diff)
{
    while (node != nullptr) {
        node->updateBalance(diff);
        int8_t balance = node->getBalance();

        if (balance == -2) {
            AVLNode<Key, Value>* left = node->getLeft();
            if (left->getBalance() <= 0) {
                rotateRight(node);
                if (left->getBalance() == 0) break;
            } else {
                rotateLeft(left);
                rotateRight(node);
            }
        }
        else if (balance == 2) {
            AVLNode<Key, Value>* right = node->getRight();
            if (right->getBalance() >= 0) {
                rotateLeft(node);
                if (right->getBalance() == 0) break;
            } else {
                rotateRight(right);
                rotateLeft(node);
            }
        }
        else if (balance != 0) {
            break;
        }

        if (node->getParent() != nullptr) {
            diff = (node == node->getParent()->getLeft()) ? 1 : -1;
        }
        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* yLeft = y->getLeft();

    y->setParent(x->getParent());
    if (x->getParent() == nullptr)
        this->root_ = y;
    else if (x == x->getParent()->getLeft())
        x->getParent()->setLeft(y);
    else
        x->getParent()->setRight(y);

    y->setLeft(x);
    x->setParent(y);
    x->setRight(yLeft);
    if (yLeft != nullptr)
        yLeft->setParent(x);

    int8_t yBal = y->getBalance();
    int8_t xBal = x->getBalance();

    if (yBal <= 0) {
        x->setBalance(xBal - 1);
        y->setBalance(yBal - 1);
    } else {
        x->setBalance(xBal - 1 - yBal);
        y->setBalance(yBal - 1 + std::min<int8_t>(x->getBalance(), 0));
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getLeft();
    AVLNode<Key, Value>* yRight = y->getRight();

    y->setParent(x->getParent());
    if (x->getParent() == nullptr)
        this->root_ = y;
    else if (x == x->getParent()->getLeft())
        x->getParent()->setLeft(y);
    else
        x->getParent()->setRight(y);

    y->setRight(x);
    x->setParent(y);
    x->setLeft(yRight);
    if (yRight != nullptr)
        yRight->setParent(x);

    int8_t yBal = y->getBalance();
    int8_t xBal = x->getBalance();

    if (yBal >= 0) {
        x->setBalance(xBal + 1);
        y->setBalance(yBal + 1);
    } else {
        x->setBalance(xBal + 1 - yBal);
        y->setBalance(yBal + 1 + std::max<int8_t>(x->getBalance(), 0));
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
