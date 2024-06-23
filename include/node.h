#ifndef _SKIPLIST_NODE_
#define _SKIPLIST_NODE_
#endif

#include <stdint.h>

template <typename KeyType, typename ValueType>
class Node
{
public:
    Node(KeyType key, ValueType value, uint32_t level);

    ~Node();

    KeyType get_key() const;

    void set_key(KeyType key);

    ValueType get_value() const;

    void set_value(ValueType value);

    Node<KeyType, ValueType> **forward_; // 后向指针数组，forward[i]表示该节点在第i层指向的后继节点

    uint32_t level_;

private:
    KeyType key_;
    ValueType value_;
};

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType>::Node(KeyType key, ValueType value, uint32_t level)
    : key_(key), value_(value), level_(level)
{
    forward_ = new Node<KeyType, ValueType> *[level];
    memset(forward_, 0, sizeof(Node<KeyType, ValueType> *) * level);
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