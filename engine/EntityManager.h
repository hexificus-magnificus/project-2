#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <tuple>
#include <type_traits>

#include "Component.h"
#include "Entity.h"
#include "Pack.h"

namespace ECSEngine {

template <typename... Components>
class EntityManager {
public:
    EntityManager() {}

    static constexpr size_t kNullComponent = std::numeric_limits<size_t>::max();
    [[nodiscard]] EntityID CreateEntity(const std::string& name) {
        EntityID id = mEntities.size();
        if (!mFreeEntities.empty()) {
            id = mFreeEntities.back();
            mFreeEntities.pop_back();
            mEntities[id] = Entity{id, name};
            mEntitiesValid[id] = true;
        } else {
            mEntities.push_back({id, name});
            mEntitiesValid.push_back(true);
        }

        for (int i = 0; i < sizeof...(Components); i++) {
            mComponentIndices[id][i] = kNullComponent;
        }

        return id;
    }

    void RemoveEntity(EntityID entity) {
        mEntitiesValid[entity] = false;
        mFreeEntities.push_back(entity);
    }

    template <typename T>
    void AddComponent(EntityID entity, T component) {
        size_t componentIdx = mComponentPack.index<T>;
        ComponentStorage<T> storage = GetComponentStorage<T>();
        size_t storageIdx = storage.mStorage.size();
        
        
        if (!storage.mFreeList.empty()) {
            storageIdx = storage.mFreeList.back();
            storage.mFreeList.pop_back();
        }
        storage.mStorage[storageIdx] = component;
        storage.mValid[storageIdx] = true;

        mComponentIndices[entity][componentIdx] = storageIdx;
    }

    template <typename T>
    void RemoveComponent(EntityID entity) {
        size_t componentIdx = mComponentPack.index<T>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        ComponentStorage<T> storage = GetComponentStorage<T>();
        storage.mValid[storageIdx] = false;
        mComponentIndices[entity][componentIdx] = kNullComponent;
    }

    template <typename T>
    bool HasComponent(EntityID entity) const {
        size_t componentIdx = mComponentPack.index<T>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        return storageIdx != kNullComponent;
    }

    template <typename T>
    T& GetComponent(EntityID entity) {
        size_t componentIdx = mComponentPack.index<T>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        ComponentStorage<T> storage = GetComponentStorage<T>();
        return storage.mStorage[storageIdx];
    }

    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        return std::get<ComponentStorage<T>>(mComponents);
    }

    bool IsValid(EntityID entity) const {

    }

    using const_iterator = std::vector<Entity>::const_iterator;
    const_iterator cbegin() const { return mEntities.cbegin(); }
    const_iterator cend() const { return mEntities.cend(); }
    const_iterator begin() { return mEntities.begin(); }
    const_iterator end() { return mEntities.end(); }
private:
    std::tuple<ComponentStorage<Components>...> mComponents;
    std::vector<Entity> mEntities;
    std::vector<bool> mEntitiesValid;
    std::vector<size_t> mFreeEntities;
    std::vector<std::array<size_t, sizeof...(Components)>> mComponentIndices;
    Pack<Components...> mComponentPack;
};

}  // namespace ECSEngine
