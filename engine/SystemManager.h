#pragma once

#include <vector>
// #include "ECSEngine.h"

namespace ECSEngine {

enum tSystemReturn { kContinue, kExit };

template <typename... Components>
class EngineView;

template <typename... Components>
class System {
public:
    virtual ~System() = default;
    virtual tSystemReturn Run(EngineView<Components...>* scene) = 0;    // will be called every frame by the engine
    virtual const std::string& GetName() const = 0;
};

template <typename... Components>
class SystemManager {
public:
    // calls run on each system in the order they were added
    bool Run(EngineView<Components...>* scene) {
        for (auto& system : mSystems) {
            if (system->Run(scene) == kExit)  // system wants to exit
            {
                std::cout << system->GetName() << " requested exit from system manager\n";
                return false;
            }
        }
        return true;
    }

    // adds a system to be run by the engine
    void AddSystem(std::unique_ptr<System<Components...>>&& system) {
        mSystems.push_back(std::move(system));
    }

    // removes all systems
    void ClearSystems() { mSystems.clear(); }

private:
    std::vector<std::unique_ptr<System<Components...>>> mSystems;
};

}  // namespace ECSEngine
