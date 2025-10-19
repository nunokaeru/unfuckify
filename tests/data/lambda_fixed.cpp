#include <functional>

void foo()
{
    const std::function<int(float)> bar = [](float baz) { return 0; };
}
