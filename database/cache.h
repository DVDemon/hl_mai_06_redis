#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <stack>
#include <atomic>

namespace database
{
    class Cache
    {
        private:
            Cache();

            using s_type = std::shared_ptr<std::iostream> ;
            std::stack<s_type> _streams;
       

        public:
            static Cache get();
            void put(long id, const std::string& val);
            bool get(long id, std::string& val);

    };
}

#endif