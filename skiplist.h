#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

#include "node.h"

template <typename KeyType, typename ValueType>
class Skiplist
{
public:
    Skiplist(int max_level);
    ~Skiplist();

    bool insert_node(KeyType key, ValueType value); // 增加和修改
    void delete_node(KeyType key);                  // 删除
    bool search_node(KeyType key);                  // 查找

    void print_skiplist(); // 输出当前的skiplist，按层输出

private:
    Node<KeyType, ValueType> *create_node(KeyType key, ValueType value, int level);
    int get_random_node_level();
    void clear(Node<KeyType, ValueType> *node);

private:
    int max_level_;      // skiplist的最大层数
    int current_level_;  // skiplist当前的层数
    int elements_count_; // skiplist中元素的个数

    Node<KeyType, ValueType> *header_;
};

template <typename KeyType, typename ValueType>
Skiplist<KeyType, ValueType>::Skiplist(int max_level)
    : max_level_(max_level)
{
    KeyType key;
    ValueType value;
    header_ = new Node<KeyType, ValueType>(key, value, max_level);

    current_level_ = 0; // 默认初始状态下skiplist为一层
    max_level_ = max_level;
    elements_count_ = 0;
}

template <typename KeyType, typename ValueType>
Skiplist<KeyType, ValueType>::~Skiplist()
{
    if (header_ != nullptr)
    {
        clear(header_);
    }
    header_ = nullptr;
}

template <typename KeyType, typename ValueType>
bool Skiplist<KeyType, ValueType>::insert_node(KeyType key, ValueType value)
{
    // 先查找待插入元素的位置
    Node<KeyType, ValueType> *current_ptr = header_;
    Node<KeyType, ValueType> *update[max_level_]; // 记录待插入节点在每一个level的前驱
    memset(update, 0, sizeof(Node<KeyType, ValueType> *) * max_level_);

    // 从最高层开始，从头遍历跳表，获取待插入节点在每一层的前驱节点
    for (int current_level = current_level_; current_level >= 0; --current_level)
    {
        while (current_ptr->forward_[current_level] != NULL && current_ptr->forward_[current_level]->get_key() < key)
        {
            current_ptr = current_ptr->forward_[current_level];
        }
        update[current_level] = current_ptr;
    }

    current_ptr = current_ptr->forward_[0]; // current_ptr现在指向待插入节点前驱节点的下一个位置，待插入节点将在update[0]和current_ptr之间

    // update中已经保存了待插入节点在每一个level的前驱节点的位置，下面开始尝试插入节点

    // 首先检查current_ptr的值，current_ptr在上一轮结束后，现在指向的最底层待插入节点的前驱节点的下一个节点
    if (current_ptr == NULL || current_ptr->get_key() != key)
    {
        // 条件通过，表示数据库中没有这个待插入的键
        int new_node_level = get_random_node_level(); // 随机设置一个节点的level，这个level服从n重伯努利分布
        if (new_node_level > current_level_)
        {
            for (int level = current_level_ + 1; level <= new_node_level; ++level)
            {
                update[level] = header_;
            }
            current_level_ = new_node_level;
        }
        Node<KeyType, ValueType> *new_node = create_node(key, value, new_node_level);
        for (int level = 0; level <= new_node_level; ++level)
        {
            new_node->forward_[level] = update[level]->forward_[level];
            update[level]->forward_[level] = new_node;
        }
        std::cout << "Successfully insert key [" << key << "] at level [" << new_node_level << "]." << std::endl;
        return true;
    }
    std::cout << "Key [" << key << "] already exists, corresponding value [" << current_ptr->get_value() << "] insertion failed." << std::endl;
    return false;
}

template <typename KeyType, typename ValueType>
void Skiplist<KeyType, ValueType>::print_skiplist()
{
    for (int level = current_level_; level >= 0; level--)
    {
        std::cout << "At level [" << level << "]:" << std::endl;
        for (Node<KeyType, ValueType> *ptr = header_->forward_[level]; ptr != NULL; ptr = ptr->forward_[level])
        {
            std::cout << ptr->get_key() << ":" << ptr->get_value() << std::endl;
        }
        std::cout << "===================================" << std::endl;
    }
}

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType> *Skiplist<KeyType, ValueType>::create_node(KeyType key, ValueType value, int level)
{
    Node<KeyType, ValueType> *new_node = new Node<KeyType, ValueType>(key, value, level);
    return new_node;
}

template <typename KeyType, typename ValueType>
int Skiplist<KeyType, ValueType>::get_random_node_level()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, RAND_MAX);
    int level = 0; // 默认从0开始
    while (dis(gen) % 2 == 0)
    {
        level++;
    }
    return (level < max_level_) ? level : (max_level_ - 1);
}

template <typename KeyType, typename ValueType>
void Skiplist<KeyType, ValueType>::clear(Node<KeyType, ValueType> *node)
{
    if (node->forward_[0] != nullptr)
    {
        clear(node->forward_[0]);
    }
    delete node;
    node = nullptr;
}
