#ifndef CPR_COROUTINE_SYNC_WAIT_H
#define CPR_COROUTINE_SYNC_WAIT_H

#if __cplusplus >= 202002L

#include <coroutine>
#include <exception>
#include <utility>

#include "cpr/coroutine/synchronization_event.h"
#include "cpr/coroutine/awaiter_traits.h"

namespace cpr::coroutine {

template <typename Result>
class [[nodiscard]] SyncWaitTask {
public:
    class promise_type {
    public:
        std::suspend_always initial_suspend() const noexcept { return {}; }
        auto final_suspend() const noexcept
        {
            struct CompletionNotifier {
                bool await_ready() const noexcept { return false; }
                void await_suspend(std::coroutine_handle<promise_type> coro) const noexcept 
                {
                    detail::SynchronizationEvent * const event = coro.promise().m_event;
                    if (event) {
                        event->set();
                    }
                }
                void await_resume() noexcept {}
            };
            return CompletionNotifier{};
        }
        
        SyncWaitTask get_return_object() noexcept
        {
            return SyncWaitTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        
        void unhandled_exception() noexcept
        {
            m_exception = std::current_exception();
        }

        auto yield_value(Result result) noexcept
        {
            m_result = std::move(result);
            return final_suspend();
        }

        void start(detail::SynchronizationEvent& event)
        {
            m_event = &event;
            std::coroutine_handle<promise_type>::from_promise(*this).resume();
        }
        
        Result result()
        {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }

            return std::move(m_result);
        }

    private:
        Result m_result;
        detail::SynchronizationEvent* m_event{ nullptr };
        std::exception_ptr m_exception;

    }; // SyncWaitTask::promise_type
    
    SyncWaitTask(std::coroutine_handle<promise_type> coro)
        : m_handle{ coro }
    {
    }
    
    SyncWaitTask(const SyncWaitTask&) = delete;
    SyncWaitTask& operator=(const SyncWaitTask&) = delete;
    
    SyncWaitTask(SyncWaitTask&& other) noexcept
        : m_handle{ std::exchange(other.m_handle, std::coroutine_handle<promise_type>{}) }
    {
    }
    
    ~SyncWaitTask()
    {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    
    void start(detail::SynchronizationEvent& event) noexcept
    {
        m_handle.promise().start(event);
    }
    
    Result result()
    {
        return m_handle.promise().result();
    }
    
private:
    std::coroutine_handle<promise_type> m_handle;
};

template <>
class [[nodiscard]] SyncWaitTask<void> {
public:
    class promise_type {
    public:
        std::suspend_always initial_suspend() const noexcept { return {}; }
        auto final_suspend() const noexcept
        {
            struct CompletionNotifier {
                bool await_ready() const noexcept { return false; }
                void await_suspend(std::coroutine_handle<promise_type> coro) const noexcept 
                {
                    detail::SynchronizationEvent * const event = coro.promise().m_event;
                    if (event) {
                        event->set();
                    }
                }
                void await_resume() noexcept {}
            };
            return CompletionNotifier{};
        }
        
        SyncWaitTask get_return_object() noexcept
        {
            return SyncWaitTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        
        void unhandled_exception() noexcept
        {
            m_exception = std::current_exception();
        }

        void start(detail::SynchronizationEvent& event)
        {
            m_event = &event;
            std::coroutine_handle<promise_type>::from_promise(*this).resume();
        }
        
        void result()
        {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }

    private:
        detail::SynchronizationEvent* m_event{ nullptr };
        std::exception_ptr m_exception;

    }; // SyncWaitTask<void>::promise_type
    
    SyncWaitTask(std::coroutine_handle<promise_type> coro)
        : m_handle{ coro }
    {
    }
    
    SyncWaitTask(const SyncWaitTask&) = delete;
    SyncWaitTask& operator=(const SyncWaitTask&) = delete;
    
    SyncWaitTask(SyncWaitTask&& other) noexcept
        : m_handle{ std::exchange(other.m_handle, std::coroutine_handle<promise_type>{}) }
    {
    }
    
    ~SyncWaitTask()
    {
        if (m_handle) {
            m_handle.destroy();
        }
    }
    
    void start(detail::SynchronizationEvent& event) noexcept
    {
        m_handle.promise().start(event);
    }
    
    void result()
    {
        m_handle.promise().result();
    }
    
private:
    std::coroutine_handle<promise_type> m_handle;
};

template <
    typename Awaitable,
    typename Result = detail::awaitable_result_t<Awaitable>,
    std::enable_if_t<!std::is_void_v<Result>, int> = 0>
auto make_sync_wait_task(Awaitable&& awaitable) -> SyncWaitTask<Result>
{
    co_yield co_await std::forward<Awaitable>(awaitable);
}

template <
    typename Awaitable,
    typename Result = detail::awaitable_result_t<Awaitable>,
    std::enable_if_t<std::is_void_v<Result>, int> = 0>
auto make_sync_wait_task(Awaitable&& awaitable) -> SyncWaitTask<void>
{
    co_await std::forward<Awaitable>(awaitable);
}

template <typename Awaitable>
auto sync_wait(Awaitable&& awaitable) -> detail::awaitable_result_t<Awaitable>
{
    detail::SynchronizationEvent event;
    auto wait_task = make_sync_wait_task(std::forward<Awaitable>(awaitable));
    wait_task.start(event);
    event.wait();
    
    return wait_task.result();
}

} // namespace cpr::coroutine

#endif // __cplusplus >= 202002L

#endif // CPR_COROUTINE_SYNC_WAIT_H
