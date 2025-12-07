#ifndef CPR_COROUTINE_TASK_H
#define CPR_COROUTINE_TASK_H

#if __cplusplus >= 202002L

#include <coroutine>
#include <exception>
#include <variant>

#include "cpr/async.h"

namespace cpr::coroutine {

template <typename T>
class [[nodiscard]] Task {
public:
    class promise_type {
    public:

        Task get_return_object() noexcept
        {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_always initial_suspend() const noexcept { return {}; }

        auto final_suspend() const noexcept
        {
            struct final_awaiter {
                bool await_ready() const noexcept { return false; }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> coro) noexcept
                {
                    return coro.promise().m_continuation;
                } 

                void await_resume() noexcept {}
            };

            return final_awaiter{};
        }

        void unhandled_exception()
        {
            m_result = std::current_exception();
        }
        
        void return_value(T x)
        {
            m_result = std::move(x);
        }
        
        void set_continuation(std::coroutine_handle<> continuation) noexcept
        {
            m_continuation = continuation;
        }
        
        T result()
        {
            if (std::holds_alternative<std::exception_ptr>(m_result)) {
                std::rethrow_exception(std::get<std::exception_ptr>(m_result));
            }
            
            assert(std::holds_alternative<T>(m_result));

            return std::move(std::get<T>(m_result));
        }
        
    private:
        std::variant<std::monostate, T, std::exception_ptr> m_result;
        std::coroutine_handle<> m_continuation{ std::noop_coroutine() };

    }; // Task::promise_type
    
    struct Awaiter {
        bool await_ready() const noexcept { return !m_coro || m_coro.done(); }

        void await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
        {
            m_coro.promise().set_continuation(awaiting_coroutine);
            GlobalThreadPool::GetInstance()->CoSubmit(
                [this]() {
                    m_coro.resume();
                }
            );
        }

        T await_resume() noexcept
        {
            return m_coro.promise().result();
        }

        std::coroutine_handle<promise_type> m_coro;
    };
    Awaiter operator co_await() noexcept
    {
        return Awaiter{ m_handle };
    }

    explicit Task(std::coroutine_handle<promise_type> handle)
        : m_handle{ handle }
    {
    }
    
    ~Task()
    {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    
private:
    std::coroutine_handle<promise_type> m_handle;
};

template <>
class Task<void> {
public:
    class promise_type {
    public:
        Task get_return_object() noexcept
        {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_always initial_suspend() const noexcept { return {}; }

        auto final_suspend() const noexcept
        {
            struct final_awaiter {
                bool await_ready() const noexcept { return false; }

                std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> coro) noexcept
                {
                    return coro.promise().m_continuation;
                } 

                void await_resume() noexcept {}
            };

            return final_awaiter{};
        }

        void unhandled_exception()
        {
            m_exception = std::current_exception();
        }
        
        void return_void() {}
        
        void set_continuation(std::coroutine_handle<> continuation) noexcept
        {
            m_continuation = continuation;
        }
        
        void result()
        {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }
        
    private:
        std::exception_ptr m_exception;
        std::coroutine_handle<> m_continuation{ std::noop_coroutine() };

    }; // Task::promise_type
    
    struct Awaiter {
        bool await_ready() const noexcept { return !m_coro || m_coro.done(); }

        void await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept
        {
            m_coro.promise().set_continuation(awaiting_coroutine);
            GlobalThreadPool::GetInstance()->CoSubmit(
                [this]() {
                    m_coro.resume();
                }
            );
        }

        void await_resume() noexcept
        {
            m_coro.promise().result();
        }

        std::coroutine_handle<promise_type> m_coro;
    };
    Awaiter operator co_await() noexcept
    {
        return Awaiter{ m_handle };
    }

    explicit Task(std::coroutine_handle<promise_type> handle)
        : m_handle{ handle }
    {
    }
    
    ~Task()
    {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    
private:
    std::coroutine_handle<promise_type> m_handle;
};

} // namespace cpr::coroutine

#endif // __cplusplus >= 202002L

#endif // CPR_COROUTINE_TASK_H
