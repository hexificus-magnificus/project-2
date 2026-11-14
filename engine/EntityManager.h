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

    // Creates an entity and returns its ID
    // Entity IDs remain valid until the entity is removed
    // Removed entity IDs may be reused by newly created entities
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
            mComponentIndices.push_back({});
        }

        for (int i = 0; i < sizeof...(Components); i++) {
            mComponentIndices[id][i] = kNullComponent;
        }

        return id;
    }

    void RemoveEntity(EntityID entity) {
        if (!IsValid(entity)) {
        return;
    }

    ([&] {
        if (HasComponent<Components>(entity)) {
            RemoveComponent<Components>(entity);
        }
    }(), ...); //lambda function provided by chatGPT to remove all components of the entity

        mEntitiesValid[entity] = false;
        mFreeEntities.push_back(entity);
    }

    // adds a component T to entity by copy
    // behaviour is undefined if entity is invalid or has a T already
    template <typename T>
    void AddComponent(EntityID entity, T component) {
        size_t componentIdx = pack_detail::packTypeToIndex<T, Components...>;
        ComponentStorage<T>& storage = GetComponentStorage<T>();
        size_t storageIdx = storage.mStorage.size();
        
        
        if (!storage.mFreeList.empty()) {
            storageIdx = storage.mFreeList.back();
            storage.mFreeList.pop_back();

            storage.mStorage[storageIdx] = component;
            storage.mValid[storageIdx] = true;
        }else{
            storageIdx = storage.mStorage.size();
            storage.mStorage.push_back(component);
            storage.mValid.push_back(true);
        }
        
        mComponentIndices[entity][componentIdx] = storageIdx;
    }

    // Returns a reference to component T associated with entity
    // Requires a valid entity that has component T
    // The reference becomes invalid if the component or entity is removed
    // Adding more components of type T may also invalidate the reference
    // if the component storage vector reallocates
    template <typename T>
    void RemoveComponent(EntityID entity) {
        size_t componentIdx = pack_detail::packTypeToIndex<T, Components...>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        ComponentStorage<T>& storage = GetComponentStorage<T>();
        storage.mValid[storageIdx] = false;
        storage.mFreeList.push_back(storageIdx);
        mComponentIndices[entity][componentIdx] = kNullComponent;
    }

    // returns true if entity has a component T associated with it, false otherwise
    // behaviour is undefined if entity is invalid
    template <typename T>
    bool HasComponent(EntityID entity) const {
        size_t componentIdx = Pack<Components...>::template index<T>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        return storageIdx != kNullComponent;
    }

    // get the component T associated with entity
    // behaviour is undefined if entity is invalid or doesn't have a T
    // returned component is valid during engine lifetime or until RemoveComponent is called
    template <typename T>
    T& GetComponent(EntityID entity) {
        size_t componentIdx = pack_detail::packTypeToIndex<T, Components...>;
        size_t storageIdx = mComponentIndices[entity][componentIdx];
        ComponentStorage<T>& storage = GetComponentStorage<T>();
        return storage.mStorage[storageIdx];
    }

    // get the storage associated with component T
    // returned storage is valid for the lifetime of the engine
    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        return std::get<ComponentStorage<T>>(mComponents);
    }

    // returns true if an entity is valid, false otherwise
    bool IsValid(EntityID entity) const {
        return mEntitiesValid[entity];
    }

    std::vector<Entity> mEntities;                                  // not all entities in this vector are guaranteed to be valid.
    using const_iterator = std::vector<Entity>::const_iterator;
    const_iterator cbegin() const { return mEntities.cbegin(); }
    const_iterator cend() const { return mEntities.cend(); }
    const_iterator begin() { return mEntities.begin(); }
    const_iterator end() { return mEntities.end(); }
private:
    std::tuple<ComponentStorage<Components>...> mComponents;
    std::vector<bool> mEntitiesValid;
    std::vector<size_t> mFreeEntities;
    std::vector<std::array<size_t, sizeof...(Components)>> mComponentIndices;
    Pack<Components...> mComponentPack;
};

}  // namespace ECSEngine
