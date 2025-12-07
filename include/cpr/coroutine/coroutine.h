#ifndef CPR_COROUTINE_H
#define CPR_COROUTINE_H

#if __cplusplus >= 202002L

#include "cpr/api.h"
#include "cpr/coroutine/task.h"

namespace cpr::coroutine {

template <typename... Ts>
auto CoGetAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Get(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoGetCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Get(std::move(ts)...));
}

template <typename... Ts>
auto CoPostAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Post(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoPostCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Post(std::move(ts)...));
}

template <typename... Ts>
auto CoPutAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Put(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoPutCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Put(std::move(ts)...));
}

template <typename... Ts>
auto CoHeadAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Head(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoHeadCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Head(std::move(ts)...));
}

template <typename... Ts>
auto CoDeleteAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Delete(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoDeleteCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Delete(std::move(ts)...));
}

template <typename... Ts>
auto CoOptionsAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Options(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoOptionsCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Options(std::move(ts)...));
}

template <typename... Ts>
auto CoPatchAsync(Ts... ts) -> Task<cpr::Response>
{
    co_return cpr::Patch(std::move(ts)...);
}

template <typename Then, typename... Ts>
auto CoPatchCallback(Then then, Ts... ts) -> Task<std::invoke_result_t<Then, cpr::Response>>
{
    co_return then(cpr::Patch(std::move(ts)...));
}

template <typename... Ts>
auto CoDownloadAsync(fs::path local_path, Ts... ts) -> Task<cpr::Response>
{
    std::ofstream f{ std::move(local_path) };
    co_return cpr::Download(f, std::move(ts)...);
}

} // namespace cpr::coroutine

#endif // __cplusplus >= 202002L

#endif // CPR_COROUTINE_H