#pragma once

#include "Keyboard.h"
#include <algorithm>

namespace ECSEngine {

const float kMaxRunVelocity = 10;
const float kDashVelocity = 20;
const int kDashDuration = 10;
const float kJumpVelocity = 25;
const float kGroundRunAcceleration = 5;
const float kAirRunAcceleration = 4;
const int kDashInterval = 25;
const float kWallFallingSpeed = 3;
const float kDownFallingAcceleration = 3;

template <typename... Components>
class InputSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("InputSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& entityManager = engine->GetEntityManager();
        for (const Entity& entity: entityManager){
            if (entityManager.IsValid(entity.GetID())&&entityManager.HasComponent<InputComponent>(entity.GetID())){
                InputComponent& input = entityManager.GetComponent<InputComponent>(entity.GetID());
                
                EntityID player = input.entity;

                if (entityManager.IsValid(player)&&entityManager.HasComponent<MovementComponent>(player)&&entityManager.HasComponent<CollisionComponent>(player)){
                    MovementComponent& movement = entityManager.GetComponent<MovementComponent>(player);

                    CollisionComponent& collisionPlayer = entityManager.GetComponent<CollisionComponent>(player);
                    float acceleration;

                    if (input.lastDash>0){
                        input.lastDash--;
                    }
                    if (input.lastDash == kDashInterval - kDashDuration) {
                        int dir = 2 * (int)signbit(movement.velocity.x) - 1;
                        movement.velocity.x -= dir * kDashVelocity * 0.5f;
                    }

                    if (collisionPlayer.onGround){
                        acceleration = kGroundRunAcceleration;
                    }else{
                        acceleration = kAirRunAcceleration;
                    }

                    //horizontal movement
                    if (input.keydown.test(Key::Right)){
                        if (movement.velocity.x + acceleration <= kMaxRunVelocity) {
                            movement.velocity.x += acceleration;
                        }
                    }else if (input.keydown.test(Key::Left)){
                        if (movement.velocity.x - acceleration >= -kMaxRunVelocity) {
                            movement.velocity.x -= acceleration;
                        }
                    }else{
                        if (movement.velocity.x <0){
                            movement.velocity.x = std::min(acceleration+movement.velocity.x, 0.0f);
                        }else if (movement.velocity.x >0){
                            movement.velocity.x = std::max(movement.velocity.x-acceleration, 0.0f);
                        }    
                    }

                    if (input.keydown.test(Key::Down)) {
                        movement.velocity.y += kDownFallingAcceleration;
                    }

                    if (input.keydown.test(Key::C) &&!input.lastKeydown.test(Key::C)&&input.lastDash==0&&movement.velocity.x!=0){
                        if (movement.velocity.x<0){
                            movement.velocity.x = -kDashVelocity;
                        }else{
                            movement.velocity.x = kDashVelocity;
                        }
                        input.lastDash=kDashInterval;
                        if (entityManager.HasComponent<GravityComponent>(player)) {
                            entityManager.GetComponent<GravityComponent>(player).disabledTimer = kDashDuration;
                            movement.velocity.y = 0;
                        }
                    }

                    //jumping
                    if (input.keydown.test(Key::Z)&&!input.lastKeydown.test(Key::Z)){
                        
                        if (collisionPlayer.onGround){
                            movement.velocity.y = -kJumpVelocity;
                        } else if (collisionPlayer.onLeftWall){
                            movement.velocity.y = -kJumpVelocity;
                            movement.velocity.x = kMaxRunVelocity;
                        } else if (collisionPlayer.onRightWall){
                            movement.velocity.y = -kJumpVelocity;
                            movement.velocity.x = -kMaxRunVelocity;
                        }
                    }else if (!input.keydown.test(Key::Z)&&input.lastKeydown.test(Key::Z)&&movement.velocity.y <0){
                        movement.velocity.y = movement.velocity.y/2;
                    }

                    if (input.keydown.test(Key::Down) && !collisionPlayer.onGround && movement.velocity.y>0){
                        movement.velocity.y += kDownFallingAcceleration;
                    }

                    //wall sliding
                    if (!collisionPlayer.onGround && movement.velocity.y>0 &&((collisionPlayer.onLeftWall &&input.keydown.test(Key::Left))||(collisionPlayer.onRightWall&&input.keydown.test(Key::Right)))){
                        movement.velocity.y = std::min(movement.velocity.y, kWallFallingSpeed);
                    }
                    
                }
                input.lastKeydown = input.keydown;

            }

        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
