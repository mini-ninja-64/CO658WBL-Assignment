#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

// TODO: popularity ranking to know which node to eject for the cache
template <typename K, typename V, size_t MAX_SIZE> class PullThroughCache {
private:
  std::unordered_map<K, V> cacheMap;
  std::function<V(const K &)> valuePuller;

public:
  PullThroughCache(const std::function<V(const K &)> &valuePuller)
      : valuePuller(valuePuller) {}

  V fetch(const K &key) {
    if (cacheMap.contains(key))
      return cacheMap[key];

    auto value = valuePuller(key);
    populate(key, value);
    return value;
  }

  void populate(const K &key, const V &value) {
    if (MAX_SIZE < cacheMap.size()) {
      cacheMap.erase(cacheMap.begin());
    }
    cacheMap[key] = value;
  }
};