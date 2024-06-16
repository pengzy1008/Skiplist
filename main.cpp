#include "skiplist.h"

int main() {
    Skiplist<int, int> skiplist(10);
    std::cout << "Create Skiplist" << std::endl;
    skiplist.insert_node(1, 1);
    // skiplist.print_skiplist();
    skiplist.insert_node(2, 2);
    // skiplist.print_skiplist();
    skiplist.insert_node(3, 3);
    // skiplist.print_skiplist();
    skiplist.insert_node(4, 4);
    // skiplist.print_skiplist();
    skiplist.insert_node(5, 5);
    skiplist.print_skiplist();
    return 0;
}