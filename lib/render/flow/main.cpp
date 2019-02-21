#include "Stream.h"
#include <iostream>

using namespace VAPoR;
using namespace flow;

int main() {
    Stream s;
    std::vector<float> tmpV;
    s._properties.emplace("temp", tmpV);

    auto it = s._properties.find("temp");
    it->second.push_back(0.0f);
    it->second.push_back(1.0f);
    it->second.push_back(2.0f);

    for (auto v = it->second.cbegin(); v != it->second.cend(); v++)
        std::cout << *v << std::endl;
}
