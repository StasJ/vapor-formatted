#pragma once

#include <functional>
#include <string>

namespace Progress {

extern std::function<void(std::string, long, bool)> Begin;
extern std::function<bool(long)> Update;
extern std::function<void()> Finish;

} // namespace Progress
