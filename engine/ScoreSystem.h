#pragma once

namespace ECSEngine {

template <typename... Components>
class ScoreSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("ScoreSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        auto& entityManager = engine->GetEntityManager();
        for (const Entity& entity: entityManager){
            if (entityManager.IsValid(entity.GetID()) && entityManager.HasComponent<ScoreComponent>(entity.GetID())){
                ScoreComponent& score = entityManager.GetComponent<ScoreComponent>(entity.GetID());

                int scoreNumber = score.score;
                int firstDigit = scoreNumber/100;
                int secondDigit = (scoreNumber/10)%10;
                int thirdDigit = (scoreNumber)%10;
                int digits[3] = {firstDigit, secondDigit, thirdDigit};
                for (int i = 0; i<3; i++){
                    EntityID entityID = score.digitID[i];
                    if (entityManager.IsValid(entityID)&&entityManager.HasComponent<SpriteComponent>(entityID)){
                        SpriteComponent& sprite = entityManager.GetComponent<SpriteComponent>(entityID);
                        sprite.textureID = score.digitTextureID[digits[i]];
                    }
               }
               break;
            }
        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
