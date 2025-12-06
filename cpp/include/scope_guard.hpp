#pragma once

#include <functional>
#include <stack>

class ScopeGuard
{
public:
    ~ScopeGuard() noexcept
    {
        while (!functions_.empty())
        {
            functions_.top()();
            functions_.pop();
        }
    }

    void add(std::function<void()> func)
    {
        functions_.push(std::move(func));
    }

    template <typename T>
    void add_release(T *ptr)
    {
        functions_.push([ptr]()
                        { ptr->Release(); });
    }

private:
    std::stack<std::function<void()>> functions_;
};
