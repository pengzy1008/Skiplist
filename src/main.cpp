#include <string>

#include "skiplist.h"

int main()
{
    int cmd = 0;
    int key;
    std::string value;
    Skiplist<int, std::string> skiplist(10);

    bool stop = false;

    while (!stop)
    {
        std::cout << "+======== Skiplist ==========+" << std::endl;
        std::cout << "| Command list:              |" << std::endl;
        std::cout << "|   Insert Key: 1            |" << std::endl;
        std::cout << "|   Delete Key: 2            |" << std::endl;
        std::cout << "|   Search Key: 3            |" << std::endl;
        std::cout << "|   Print list: 9            |" << std::endl;
        std::cout << "|   Exit      : 0            |" << std::endl;
        std::cout << "+============================+" << std::endl;
        std::cout << "Input command: ";
        std::cin >> cmd;

        switch (cmd)
        {
        case 1:
        {
            std::cout << "Key: ";
            std::cin >> key;
            std::cout << "Value: ";
            std::cin >> value;
            skiplist.insert_node(key, value);
            continue;
        }

        case 2:
        {
            std::cout << "Key: ";
            std::cin >> key;
            skiplist.delete_node(key);
            continue;
        }

        case 3:
        {
            std::cout << "Key: ";
            std::cin >> key;
            skiplist.search_node(key);
            continue;
        }

        case 9:
        {
            skiplist.print_skiplist();
            continue;
        }

        case 0:
        {
            stop = true;
            std::cout << "Exiting..." << std::endl;
            break;
        }

        default:
            std::cout << "Unknown command!" << std::endl;
            continue;
        }
    }

    return 0;
}