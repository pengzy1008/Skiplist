#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <mutex>

#include "node.h"

template <typename KeyType, typename ValueType>
class Skiplist
{
public:
    Skiplist(int max_level);
    ~Skiplist();

    bool insert_node(KeyType key, ValueType value); // 增加和修改
    bool delete_node(KeyType key);                  // 删除
    ValueType search_node(KeyType key);             // 查找

    void print_skiplist(); // 输出当前的skiplist，按层输出

private:
    Node<KeyType, ValueType> *create_node(KeyType key, ValueType value, int level);
    int get_random_node_level();
    void clear(Node<KeyType, ValueType> *node);

private:
    int max_level_;      // skiplist的最大层数
    int current_level_;  // skiplist当前的层数, current_level_为多少，数组下标就可以取到多少，这个是考虑到的
    int elements_count_; // skiplist中元素的个数

    Node<KeyType, ValueType> *header_;

    std::mutex mutex_;
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
    clear(header_);
}

template <typename KeyType, typename ValueType>
bool Skiplist<KeyType, ValueType>::insert_node(const KeyType key, const ValueType value)
{
    mutex_.lock();
    // 先查找待插入元素的位置
    Node<KeyType, ValueType> *current_ptr = header_;
    Node<KeyType, ValueType> *update[max_level_]; // 记录待插入节点在每一个level的前驱
    memset(update, 0, sizeof(Node<KeyType, ValueType> *) * max_level_);

    // 从最高层开始，从头遍历跳表，获取待插入节点在每一层的前驱节点
    for (int current_level = current_level_; current_level >= 0; --current_level)
    {
        while (current_ptr->forward_[current_level] != nullptr && current_ptr->forward_[current_level]->get_key() < key)
        {
            current_ptr = current_ptr->forward_[current_level];
        }
        update[current_level] = current_ptr;
    }

    current_ptr = current_ptr->forward_[0]; // current_ptr现在指向待插入节点前驱节点的下一个位置，待插入节点将在update[0]和current_ptr之间

    // update中已经保存了待插入节点在每一个level的前驱节点的位置，下面开始尝试插入节点

    // 首先检查current_ptr的值，current_ptr在上一轮结束后，现在指向的最底层待插入节点的前驱节点的下一个节点
    if (current_ptr == nullptr || current_ptr->get_key() != key)
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
        elements_count_++; // 元素数量增加
        mutex_.unlock();
        return true;
    }
    mutex_.unlock();
    std::cout << "Key [" << key << "] already exists, corresponding value [" << current_ptr->get_value() << "], insertion failed." << std::endl;
    return false;
}

template <typename KeyType, typename ValueType>
bool Skiplist<KeyType, ValueType>::delete_node(const KeyType key)
{
    mutex_.lock(); // 上锁，互斥访问

    // 删除元素和插入元素的过程基本相同，也是先找待删除元素的前一个元素的位置，再将待删除元素删除
    Node<KeyType, ValueType> *current_ptr = header_;
    Node<KeyType, ValueType> *update[current_level_ + 1];
    memset(update, 0, sizeof(Node<KeyType, ValueType> *) * (current_level_ + 1));

    // 1. 寻找潜在的待删除节点在跳表中的每一层的前驱节点的位置，加入到update数组中。
    // 为了利用跳表的查询高效性，从最高层开始遍历
    for (int level = current_level_; level >= 0; --level)
    {
        while (current_ptr->forward_[level] != nullptr && current_ptr->forward_[level]->get_key() < key)
        {
            current_ptr = current_ptr->forward_[level];
        }
        update[level] = current_ptr;
    }

    // current_ptr继续往前指，即可之前这个潜在的将要被删除的节点
    current_ptr = current_ptr->forward_[0];

    // 检查current_ptr指向的是否就是我们要删除的
    if (current_ptr != nullptr && current_ptr->get_key() == key)
    {
        // 从底层到高层删除这个节点
        for (int level = 0; level <= current_ptr->level_; ++level)
        {
            // update[level]表示这个待删除节点的前驱节点
            update[level]->forward_[level] = current_ptr->forward_[level];
            current_ptr->forward_[level] = nullptr;
        }
        ValueType deleted_node_value = current_ptr->get_value();
        int deleted_node_level = current_ptr->level_;
        delete current_ptr;

        // 检查Skiplist的顶层，如果顶层的header_->forward[level]指向了nullptr，就将skiplist的层数降低
        for (int level = current_level_; level >= 0; --level)
        {
            if (header_->forward_[level] == nullptr)
            {
                current_level_--;
            }
            else
            {
                break;
            }
        }
        elements_count_--;
        mutex_.unlock();
        std::cout << "Successfully delete key [" << key << "] with value [" << deleted_node_value << "], level [" << deleted_node_level << "]." << std::endl;
        return true;
    }

    // 没有要删除的节点，返回失败
    mutex_.unlock();
    std::cout << "Key [" << key << "] does not exist, deletion failed." << std::endl;
    return false;
}

template <typename KeyType, typename ValueType>
ValueType Skiplist<KeyType, ValueType>::search_node(const KeyType key)
{
    mutex_.lock();

    Node<KeyType, ValueType> *current_ptr = header_;

    for (int level = current_level_; level >= 0; level--)
    {
        while (current_ptr->forward_[level] != nullptr && current_ptr->forward_[level]->get_key() < key)
        {
            current_ptr = current_ptr->forward_[level];
        }
    }

    current_ptr = current_ptr->forward_[0];

    if (current_ptr != nullptr && current_ptr->get_key() == key)
    {
        ValueType value = current_ptr->get_value();
        mutex_.unlock();
        std::cout << "Successfully get value for key [" << key << "], corresponding value [" << value << "]." << std::endl;
        return value;
    }

    mutex_.unlock();
    std::cout << "Key [" << key << "] does not exist, searching failed." << std::endl;
    return 0;
}

template <typename KeyType, typename ValueType>
void Skiplist<KeyType, ValueType>::print_skiplist()
{
    mutex_.lock();
    for (int level = current_level_; level >= 0; level--)
    {
        std::cout << "At level [" << level << "]:" << std::endl;
        for (Node<KeyType, ValueType> *ptr = header_->forward_[level]; ptr != NULL; ptr = ptr->forward_[level])
        {
            std::cout << ptr->get_key() << ":" << ptr->get_value() << std::endl;
        }
        std::cout << "===================================" << std::endl;
    }
    mutex_.unlock();
}

template <typename KeyType, typename ValueType>
Node<KeyType, ValueType> *Skiplist<KeyType, ValueType>::create_node(const KeyType key, const ValueType value, int level)
{
    Node<KeyType, ValueType> *node = new Node<KeyType, ValueType>(key, value, level);
    return node;
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

