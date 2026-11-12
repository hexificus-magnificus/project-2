#pragma once

#include <unordered_map>
#include "CameraComponent.h"
#include "Entity.h"
#include "EntityManager.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

template <typename... Components>
class CameraSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name = "CameraSystem";
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        auto& entityManager = engine->GetEntityManager();
        auto& windowManager = engine->GetWindowManager();
        for (const Entity& entity: entityManager){
            if (entityManager.IsValid(entity.GetID())&&entityManager.HasComponent<CameraFollower>(entity.GetID())&&entityManager.HasComponent<CameraComponent>(entity.GetID())){
                CameraFollower& follower = entityManager.GetComponent<CameraFollower>(entity.GetID());
                CameraComponent& camera = entityManager.GetComponent<CameraComponent>(entity.GetID());
                windowManager.SetWorldScale(camera.worldUnitsPerPixel);
                if (entityManager.IsValid(follower.entityID) && entityManager.HasComponent<LocationComponent>(follower.entityID)){
                    LocationComponent& location = entityManager.GetComponent<LocationComponent>(follower.entityID);
                    Point2D pos = windowManager.WorldToWindow(location.loc);

                    float hardLeft = camera.hardExclusion.topLeft.x;
                    float hardRight = hardLeft + camera.hardExclusion.width;
                    float softLeft = camera.softExclusion.topLeft.x;
                    float softRight = softLeft + camera.softExclusion.width;

                    float cameraChange = 0;
                    if (pos.x < hardLeft){
                        cameraChange = pos.x-hardLeft;
                    }else if(pos.x <softLeft){
                        cameraChange = camera.speed* (pos.x-softLeft);
                    }else if(pos.x >hardRight){
                        cameraChange = pos.x-hardRight;
                    }else if(pos.x > softRight){
                        cameraChange = camera.speed*(pos.x-softRight);
                    }

                    Point2D center = windowManager.GetWorldCenter();
                    center.x += windowManager.WindowToWorld(cameraChange);
                    windowManager.SetWorldCenter(center);
                    break;
                }
            }
        }
        return kContinue;

    }
};

}  // namespace ECSEngine
