#ifndef CPR_COROUTINE_SYNCHRONIZATION_EVENT_H
#define CPR_COROUTINE_SYNCHRONIZATION_EVENT_H

#if __cplusplus >= 202002L

#include <atomic>

namespace cpr::coroutine::detail {
    
class SynchronizationEvent {
public:
    void set()
    {
        m_flag.test_and_set();
        m_flag.notify_all();      
    }
    
    void wait()
    {
        m_flag.wait(false);
    }
    
private:
    std::atomic_flag m_flag;
};

} // namespace cpr::coroutine::detail

#endif // __cplusplus >= 202002L

#endif // CPR_COROUTINE_SYNCHRONIZATION_EVENT_H