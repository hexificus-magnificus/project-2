#pragma once

#include <bitset>
#include <unordered_map>

#include "CameraComponent.h"
#include "EntityManager.h"
#include "MathUtil.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

template <typename... Components>
class SpriteSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpriteSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& eManager = engine->GetEntityManager();
        auto& wManager = engine->GetWindowManager();
        for (auto e = eManager.cbegin(); e != eManager.cend(); e++) {
            if (!eManager.IsValid(e->GetID()) || !eManager.HasComponent<SpriteComponent>(e->GetID())) {
                continue;
            }
            SpriteComponent spriteC = eManager.GetComponent<SpriteComponent>(e->GetID());
            // in provided files, it says SpriteComponent only exists when LocationComponent does
            LocationComponent loc = eManager.GetComponent<LocationComponent>(e->GetID());

            sf::Sprite sprite = engine->GetSpriteManager().GetSprite(spriteC.textureID);
            if (spriteC.worldSpace) {
                float scale = 1.0f / wManager.GetWorldScale();
                sprite.setScale({scale, scale});
                Point2D wLoc = wManager.WorldToWindow(loc.loc);
                sprite.setPosition({wLoc.x, wLoc.y});
            } else {
                sprite.setPosition({loc.loc.x, loc.loc.y});
            }

            wManager.GetWindow()->draw(sprite);
        }
        return kContinue;
    }
};

}  // namespace ECSEngine
