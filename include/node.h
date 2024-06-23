#ifndef _SKIPLIST_NODE_
#define _SKIPLIST_NODE_
#endif

#include <stdint.h>

template <typename KeyType, typename ValueType>
class Node
{
public:
    Node() {}

    Node(KeyType key, ValueType value, int level);

    ~Node();

    KeyType get_key() const;

    void set_key(KeyType key);

    ValueType get_value() const;

    void set_value(ValueType value);

    Node<KeyType, ValueType> **forward_; // 后向指针数组，forward[i]指向该节点在第i层指向的后继节点

    int level_;

private:
    KeyType key_;
    ValueType value_;
};

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType value, int level)
    : key_(key), value_(value), level_(level)
{
    // 这里特别要注意！！level_从0开始
    // 所以在分配内存时，需要分配level_ + 1大小的内存
    // 否则当level为0时，分配的数组大小为0，会导致访问越界
    forward_ = new Node<KeyType, ValueType> *[level + 1];
    memset(forward_, 0, sizeof(Node<KeyType, ValueType> *) * (level + 1));
}

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::~Node()
{
    delete[] forward_;
}

template <typename KeyType, typename ValueType>
KeyType Node<KeyType, ValueType>::get_key() const
{
    return key_;
}

template <typename KeyType, typename ValueType>
void Node<KeyType, ValueType>::set_key(KeyType key)
{
    key_ = key;
}

template <typename KeyType, typename ValueType>
ValueType Node<KeyType, ValueType>::get_value() const
{
    return value_;
}

template <typename KeyType, typename ValueType>
void Node<KeyType, ValueType>::set_value(ValueType value)
{
    value_ = value;
}