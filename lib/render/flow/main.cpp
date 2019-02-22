#include "Particle.h"
#include <cstdlib>
#include <iostream>

#include <deque>
#include <forward_list>
#include <list>
#include <queue>
#include <stack>
#include <vector>

using namespace flow;

int main() {
    Particle p;
    std::cout << sizeof(p) << std::endl;
    std::cout << sizeof(Particle) << std::endl;

    std::cout << "vector size: " << sizeof(std::vector<float>) << std::endl;
    std::cout << "list size: " << sizeof(std::list<float>) << std::endl;
    std::cout << "forward_list size: " << sizeof(std::forward_list<float>) << std::endl;
    std::cout << "deque size: " << sizeof(std::deque<float>) << std::endl;
    std::cout << "stack size: " << sizeof(std::stack<float>) << std::endl;
    std::cout << "queue size: " << sizeof(std::queue<float>) << std::endl;
}
