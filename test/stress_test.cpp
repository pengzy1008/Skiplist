#include <iostream>
#include <thread>
#include <chrono>

#include "skiplist.h"

#define THREAD_NUMS 1
#define TOTAL_ELEM_NUMS 1000000

Skiplist<int, std::string> skiplist(18);

// 批量插入数据的函数，根据传入的线程id来设置要插入的键的范围
void insert_elements(int thread_id)
{
    int elememt_nums = TOTAL_ELEM_NUMS / THREAD_NUMS;
    int bottom_range = elememt_nums * thread_id;    // 该线程负责插入的键的下界
    int top_range = elememt_nums * (thread_id + 1); // 改线程负责插入的键的上界，注意这里没有没有处理最后没有除尽的情况
    for (int i = bottom_range; i < top_range; ++i)
    {
        skiplist.insert_node(i, "pengzy1008");
    }
}

// 批量获取数据的函数，根据传入的线程id来检索范围内的键对应的值
void search_elements(int thread_id)
{
    int elememt_nums = TOTAL_ELEM_NUMS / THREAD_NUMS;
    int bottom_range = elememt_nums * thread_id;
    int top_range = elememt_nums * (thread_id + 1);
    for (int i = bottom_range; i < top_range; ++i)
    {
        skiplist.search_node(i);
    }
}

// 批量删除数据的函数
void delete_elements(int thread_id)
{
    int elememt_nums = TOTAL_ELEM_NUMS / THREAD_NUMS;
    int bottom_range = elememt_nums * thread_id;
    int top_range = elememt_nums * (thread_id + 1);
    for (int i = bottom_range; i < top_range; ++i)
    {
        skiplist.delete_node(i);
    }
}

int main()
{
    std::thread threads[THREAD_NUMS];
    // 数据插入
    auto start_timestamp = std::chrono::high_resolution_clock::now();
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id] = std::thread(insert_elements, thread_id);
    }
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id].join();
    }
    auto end_timestamp = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_timestamp - start_timestamp;
    std::cout << "insert elapsed: " << elapsed.count() << std::endl;

    // 数据查询
    start_timestamp = std::chrono::high_resolution_clock::now();
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id] = std::thread(search_elements, thread_id);
    }
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id].join();
    }
    end_timestamp = std::chrono::high_resolution_clock::now();
    elapsed = end_timestamp - start_timestamp;
    std::cout << "search elapsed: " << elapsed.count() << std::endl;

    // 数据删除
    start_timestamp = std::chrono::high_resolution_clock::now();
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id] = std::thread(delete_elements, thread_id);
    }
    for (int thread_id = 0; thread_id < THREAD_NUMS; ++thread_id) {
        threads[thread_id].join();
    }
    end_timestamp = std::chrono::high_resolution_clock::now();
    elapsed = end_timestamp - start_timestamp;
    std::cout << "delete elapsed: " << elapsed.count() << std::endl;
}