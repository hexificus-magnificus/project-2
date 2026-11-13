#pragma once

namespace ECSEngine {

template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& eManager = engine->GetEntityManager();
		for (auto e = eManager.begin(); e != eManager.end(); e++) {
            if (!eManager.HasComponent<CollisionComponent>(e->GetID())) {
				continue;
			}
            CollisionComponent& c = eManager.GetComponent<CollisionComponent>(e->GetID());
            c.previousCollisionBox = c.collisionBox;

            auto& location = eManager.GetComponent<LocationComponent>(e->GetID());
            c.previousCollisionBox.topLeft =c.previousCollisionBox.topLeft + location.loc;
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
