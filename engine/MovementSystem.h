#pragma once

namespace ECSEngine {

template <typename... Components>
class MovementSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("MovementSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        auto& entityManager = engine->GetEntityManager();
        for (const Entity& entity: entityManager){
            if (entityManager.IsValid(entity.GetID()) && entityManager.template HasComponent<LocationComponent>(entity.GetID()) &&entityManager.template HasComponent<MovementComponent>(entity.GetID())){
                LocationComponent& location = entityManager.template GetComponent<LocationComponent>(entity.GetID());
                MovementComponent& movement = entityManager.template GetComponent<MovementComponent>(entity.GetID());
                location.loc.x += movement.velocity.x;
                location.loc.y += movement.velocity.y;
                
            }
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
