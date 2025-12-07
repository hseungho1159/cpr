#ifndef CPR_COROUTINE_AWAITER_TRAITS_H
#define CPR_COROUTINE_AWAITER_TRAITS_H

#if __cplusplus >= 202002L

#include <utility>
#include <type_traits>

namespace cpr::coroutine::detail {

template <typename T>
struct get_awaiter_type {
private:

    // Simply gets Awaiter from either member or non-member operator co_await().
    template <typename U>
    static auto test(U&& value) -> decltype(std::forward<U>(value).operator co_await());
    
    // Fallback: assumes that T is Awaiter itself.
    static auto test(...) -> T;

public:
    using type = decltype(test(std::declval<T>()));
};
template <typename T>
using get_awaiter_type_t = typename get_awaiter_type<T>::type;

template <typename T>
struct awaitable_result {
private:
    using Awaiter = get_awaiter_type_t<T>;
    
public:
    using type = std::invoke_result_t<decltype(&Awaiter::await_resume), Awaiter>;
};

template <typename T>
using awaitable_result_t = typename awaitable_result<T>::type;

} // namespace cpr::coroutine::detail

#endif // __cplusplus >= 202002L

#endif // CPR_COROUTINE_AWAITER_TRAITS_H
