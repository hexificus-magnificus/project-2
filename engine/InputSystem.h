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
const int kDashInterval = 60;
const float kWallFallingSpeed = 2;
const float kDownFallingAcceleration = 12;

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
            if (entityManager.IsValid(entity.GetID())&&entityManager.template HasComponent<InputComponent>(entity.GetID())){
                InputComponent& input = entityManager.template GetComponent<InputComponent>(entity.GetID());
                
                EntityID player = input.entity;

                if (entityManager.IsValid(player)&&entityManager.template HasComponent<MovementComponent>(player)&&entityManager.template HasComponent<CollisionComponent>(player)){
                    MovementComponent& movement = entityManager.template GetComponent<MovementComponent>(player);

                    CollisionComponent& collisionPlayer = entityManager.template GetComponent<CollisionComponent>(player);
                    float acceleration;

                    if (input.lastDash>0){
                        input.lastDash--;
                    }
                    if (input.lastDash == kDashInterval - kDashDuration) {
                        if (movement.velocity.x > kMaxRunVelocity) {
                            movement.velocity.x = kMaxRunVelocity;
                        } else if (movement.velocity.x < -kMaxRunVelocity) {
                            movement.velocity.x = -kMaxRunVelocity;
                        }
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

                    

                    if (input.keydown.test(Key::C) &&!input.lastKeydown.test(Key::C)&&input.lastDash==0){
                        // if (movement.velocity.x<0){
                        //     movement.velocity.x = -kDashVelocity;
                        // }else{
                        //     movement.velocity.x = kDashVelocity;
                        // }
                        // input.lastDash=kDashInterval;
                        if (entityManager.template HasComponent<GravityComponent>(player)) {
                            entityManager.template GetComponent<GravityComponent>(player).disabledTimer = kDashDuration;
                            movement.velocity.y = 0;
                        }
                        if (collisionPlayer.onLeftWall) {
                            movement.velocity.x = kDashVelocity;
                        } else if (collisionPlayer.onRightWall) {
                            movement.velocity.x = -kDashVelocity;
                        } else if (input.keydown.test(Key::Right)) {
                            movement.velocity.x = kDashVelocity;
                        } else if (input.keydown.test(Key::Left)) {
                            movement.velocity.x = -kDashVelocity;
                        } else if (movement.velocity.x != 0) {
                            movement.velocity.x =
                                movement.velocity.x > 0 ? kDashVelocity : -kDashVelocity;
                        }
                        input.lastDash = kDashInterval;
                    }

                    //jumping
                    if (input.keydown.test(Key::Z)&&!input.lastKeydown.test(Key::Z)){
                        
                        if (collisionPlayer.onGround){
                            movement.velocity.y = -kJumpVelocity;
                            engine->GetSoundManager().GetSound("jump").play();
                        } else if (collisionPlayer.onLeftWall){
                            movement.velocity.y = -kJumpVelocity;
                            movement.velocity.x = kMaxRunVelocity;
                            engine->GetSoundManager().GetSound("jump").play();
                        } else if (collisionPlayer.onRightWall){
                            movement.velocity.y = -kJumpVelocity;
                            movement.velocity.x = -kMaxRunVelocity;
                            engine->GetSoundManager().GetSound("jump").play();
                        }
                    }else if (!input.keydown.test(Key::Z)&&input.lastKeydown.test(Key::Z)&&movement.velocity.y <0){
                        movement.velocity.y = movement.velocity.y/2;
                    }
                    
                    if (!collisionPlayer.onGround && movement.velocity.y > 0) {
                        movement.velocity.y += 2.0f;
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
