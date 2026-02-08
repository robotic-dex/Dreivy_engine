#pragma once
#include <vector>
#include <unordered_map>
#include <cassert>

using Entity = uint32_t;

class World {
public:
    Entity CreateEntity() {
        return ++m_next;
    }

    template<typename T>
    void AddComponent(Entity e, T component = {}) {
        GetStorage<T>()[e] = component;
    }
    Entity EntityCount() const {
        return m_next;
    }
    template<typename T>
    bool HasComponent(Entity e) const {
        const auto& storage = GetStorage<T>();
        return storage.find(e) != storage.end();
    }

    template<typename T>
    T& GetComponent(Entity e) {
        auto& storage = GetStorage<T>();
        assert(storage.find(e) != storage.end());
        return storage[e];
    }

    template<typename T>
    T* TryGetComponent(Entity e) {
        auto& storage = GetStorage<T>();
        auto it = storage.find(e);
        return it != storage.end() ? &it->second : nullptr;
    }

private:
    template<typename T>
    std::unordered_map<Entity, T>& GetStorage() const {
        static std::unordered_map<Entity, T> storage;
        return storage;
    }

    Entity m_next = 0;
};
