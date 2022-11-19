#include "cache.h"
#include "../config/config.h"

#include <exception>
#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace database
{
    std::mutex _mtx;
    Cache::Cache()
    {
        std::string host = Config::get().get_cache_servers().substr(0,
                                                                    Config::get().get_cache_servers().find(':'));
        std::string port = Config::get().get_cache_servers().substr(Config::get().get_cache_servers().find(':') + 1);

        for (size_t i = 0; i < 1000; ++i)
            _streams.push(rediscpp::make_stream(host, port));
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    void Cache::put([[maybe_unused]] long id, [[maybe_unused]] const std::string &val)
    {


        while (_streams.empty())
            std::this_thread::yield();
        s_type stream;
        {
            std::lock_guard<std::mutex> lck(_mtx);
            stream = _streams.top();
            _streams.pop();
        }
        rediscpp::value response = rediscpp::execute(*stream, "set",
                                                     std::to_string(id),
                                                     val, "ex", "60");
        {
            std::lock_guard<std::mutex> lck(_mtx);
            _streams.push(stream);
        }
    }

    bool Cache::get([[maybe_unused]] long id, [[maybe_unused]] std::string &val)
    {
        while (_streams.empty())
            std::this_thread::yield();
        s_type stream;
        {
            std::lock_guard<std::mutex> lck(_mtx);
            stream = _streams.top();
            _streams.pop();
        }

        rediscpp::value response = rediscpp::execute(*stream, "get", std::to_string(id));

        {
            std::lock_guard<std::mutex> lck(_mtx);
           _streams.push(stream);
        }

        if (response.is_error_message())
            return false;
        if (response.empty())
            return false;

        val = response.as<std::string>();
        return true;
    }
}