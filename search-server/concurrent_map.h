// Copyright 2022
// 16:21 11/06/2022

#pragma once

#include <algorithm>
#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
private:
    struct Safe {
        std::mutex mutex;
        std::map<Key, Value> map;
    };
public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);

    struct Access {
        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;
        Access(const Key& key, Safe& safe)
            : guard(safe.mutex), ref_to_value(safe.map[key]) {
        }
    };

    explicit ConcurrentMap(size_t bucket_count)
        : data_(bucket_count) {
    }

    Access operator[](const Key& key) {
        auto& Save = data_[static_cast<uint64_t>(key) % data_.size()];
        return { key, Save };
    }

    std::map<Key, Value> BuildOrdinaryMap() {
        std::map<Key, Value> result;
        for (auto& [mutex, map] : data_) {
            std::lock_guard g(mutex);
            result.insert(map.begin(), map.end());
        }
        return result;
    }

private:
    std::vector<Safe> data_;
};
