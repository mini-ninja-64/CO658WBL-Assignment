#pragma once

#include <unordered_map>
#include <optional>
#include <memory>
#include <functional>

// TODO: popularity ranking to know which node to eject for the cache
template<typename K, typename V>
class PullThroughCache {
private:
    std::unordered_map<K, V> cacheMap;
    std::function<V(const K&)> valuePuller;
    size_t maximumSize;

public:
    PullThroughCache(const std::function<V(const K &)> &valuePuller, size_t maximumSize) :
        valuePuller(valuePuller), maximumSize(maximumSize) {}

    V fetch(const K& key) {
        if (cacheMap.contains(key)) return cacheMap[key];

        auto value = valuePuller(key);
        populate(key, value);
        return value;
    }

    void populate(const K& key, const V& value) {
        if(maximumSize < cacheMap.size()) {
            cacheMap.erase(cacheMap.begin());
        }
        cacheMap[key] = value;
    }
};