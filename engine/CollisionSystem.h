#pragma once

#include "ScoreComponent.h"
#include "LocationComponent.h"

namespace ECSEngine {

template <typename... Components>
class CollisionSystem : public System<Components...> {
public:
    CollisionSystem() : starBounceHeight(-1) {}

    const std::string& GetName() const override {
        static std::string name("CollisionSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        EntityManager<Components...> eManager = engine->GetEntityManager();
        /*
        for (int i = 0; i < eManager.mEntities.size(); i++) {
            Entity e1 = eManager.mEntities[i];
            if (!eManager.IsValid(e1.GetID())) {
                continue;
            }
            if (!eManager.HasComponent<CollisionComponent>(e1.GetID())) {
				continue;
			}
            CollisionComponent& c1 = eManager.GetComponent<CollisionComponent>(e1.GetID());

            c1.onCeiling = 0;
            c1.onGround = 0;
            c1.onLeftWall = 0;
            c1.onRightWall = 0;
        }
        */

		for (int i = 0; i < eManager.mEntities.size(); i++) {
            Entity& e1 = eManager.mEntities[i];
            if (!eManager.IsValid(e1.GetID())) {
                continue;
            }
            if (!eManager.HasComponent<CollisionComponent>(e1.GetID())) {
				continue;
			}
            CollisionComponent& c1 = eManager.GetComponent<CollisionComponent>(e1.GetID());

            for (auto j = i + 1; i < eManager.mEntities.size(); j++) {
                Entity& e2 = eManager.mEntities[j];
                if (!eManager.IsValid(e2.GetID())) {
                    continue;
                }
                if (!eManager.HasComponent<CollisionComponent>(e2.GetID())) {
                    continue;
                }
                CollisionComponent& c2 = eManager.GetComponent<CollisionComponent>(e2.GetID());
                
                std::vector<bool> overlaps{false, false, false, false};
                RectOverlap(c1.collisionBox, c2.collisionBox, overlaps);
                if (!(overlaps[0] || overlaps[1] || overlaps[2] || overlaps[3])) {
                    // collision
                    if (c1.staticEntity) {
                        if (c2.staticEntity) {
                            continue;
                        } else {
                            // c1 static, c2 not
                            StaticDynamicCollide(e1, e2, engine);
                            if (eManager.HasComponent<ScoreComponent>(e2.GetID())) {
                                if (c1.doesDamage) {
                                    Damage(e2, eManager);
                                    if (eManager.HasComponent<ScoreComponent>(e1.GetID())) {
                                        eManager.GetComponent<ScoreComponent>(e1.GetID()).score += 1;
                                    }
                                }
                            } else {
                                // not the player, bounce
                                MovementComponent& m2 = eManager.GetComponent<MovementComponent>(e2.GetID());
                                m2.velocity = {m2.velocity.x * -0.9f, starBounceHeight};
                            }
                        }
                    } else {
                        if (c2.staticEntity) {
                            // c1 not, c2 static
                            StaticDynamicCollide(e2, e1, engine);
                            if (eManager.HasComponent<ScoreComponent>(e1.GetID())) {
                                if (c2.doesDamage) {
                                    Damage(e1, eManager);
                                    if (eManager.HasComponent<ScoreComponent>(e2.GetID())) {
                                        eManager.GetComponent<ScoreComponent>(e2.GetID()).score += 1;
                                    }
                                }
                            } else {
                                // not the player, bounce
                                MovementComponent& m1 = eManager.GetComponent<MovementComponent>(e1.GetID());
                                m1.velocity = {m1.velocity.x * -0.9f, starBounceHeight};
                            }
                        } else {
                            // both dynamic
                            if (eManager.HasComponent<ScoreComponent>(e1.GetID())) {
                                // collect e2
                                eManager.RemoveEntity(e2.GetID());
                                eManager.GetComponent<ScoreComponent>(e1.GetID()).score++;
                                engine->GetSoundManager().GetSound("sget").play();
                            } else if (eManager.HasComponent<ScoreComponent>(e2.GetID())) {
                                // collect e1
                                eManager.RemoveEntity(e1.GetID());
                                eManager.GetComponent<ScoreComponent>(e1.GetID()).score++;
                                engine->GetSoundManager().GetSound("sget").play();
                            } else {
                                // 2 stars - bounce
                                MovementComponent& m1 = eManager.GetComponent<MovementComponent>(e1.GetID());
                                MovementComponent& m2 = eManager.GetComponent<MovementComponent>(e2.GetID());
                                m1.velocity = {m1.velocity.x * -0.9f, starBounceHeight};
                                m2.velocity = {m2.velocity.x * -0.9f, starBounceHeight};
                            }
                        }
                    }
                }
            }
        }
        return kContinue;
    }

private:
    // to be called after detecting a collision between 2 entities
    // will push the dynamic entity out of the static one based on
    // the previous frame's bounding boxes
    void StaticDynamicCollide(Entity staticO, Entity dynamic, EngineView<Components...>* engine) {
        EntityManager<Components...> eManager = engine->GetEntityManager();
        CollisionComponent& c1 = eManager.GetComponent<CollisionComponent>(staticO.GetID());
        CollisionComponent& c2 = eManager.GetComponent<CollisionComponent>(dynamic.GetID());

        std::vector<bool> oldOverlaps{false, false, false, false};
        RectOverlap(c1.previousCollisionBox, c2.previousCollisionBox, oldOverlaps);
        if (oldOverlaps[2]) {
            c2.onCeiling = 1;
            float diff = c2.collisionBox.topLeft.y - (c1.collisionBox.topLeft.y + c1.collisionBox.height);
            LocationComponent l2 = eManager.GetComponent<LocationComponent>(dynamic.GetID());
            l2.loc = l2.loc - Point2D(0.f, diff - 1);
        } else if (oldOverlaps[3]) {
            if (!c2.onGround) {
                // just landed
                engine->GetSoundManager().GetSound("land").play();
            }
            c2.onGround = 1;
            float diff = c2.collisionBox.topLeft.y + c2.collisionBox.height - c1.collisionBox.topLeft.y;
            LocationComponent l2 = eManager.GetComponent<LocationComponent>(dynamic.GetID());
            l2.loc = l2.loc - Point2D(0.f, diff + 1);
        }
        if (oldOverlaps[0]) {
            if (!c2.onRightWall) {
                // just hit wall
                engine->GetSoundManager().GetSound("wall").play();
            }
            c2.onRightWall = 1;
            float diff = c2.collisionBox.topLeft.x + c2.collisionBox.width - c1.collisionBox.topLeft.x;
            LocationComponent l2 = eManager.GetComponent<LocationComponent>(dynamic.GetID());
            l2.loc = l2.loc - Point2D{diff + 1, 0.f};
        } else if (oldOverlaps[1]) {
            if (!c2.onLeftWall) {
                // just hit wall
            }
            c2.onLeftWall = 1;
            float diff = c2.collisionBox.topLeft.x - (c1.collisionBox.topLeft.x + c1.collisionBox.width);
            LocationComponent l2 = eManager.GetComponent<LocationComponent>(dynamic.GetID());
            l2.loc = l2.loc - Point2D{diff - 1, 0.f};
        }
    }

    void Damage(Entity e, EntityManager<Components...> eManager) {
        if (eManager.HasComponent<RespawnComponent>(e.GetID())) {
            RespawnComponent r = eManager.GetComponent<RespawnComponent>(e.GetID());
            LocationComponent l = eManager.GetComponent<LocationComponent>(e.GetID());
            MovementComponent m = eManager.GetComponent<MovementComponent>(e.GetID());

            l.loc = r.loc;
            m.velocity = {0, 0};
        } else {
            eManager.RemoveEntity(e.GetID());
        }
    }

    // detects overlaps between rects and returns whether there is a
    // separation in each direction using ret, in the following order:
    // ret[0]: r1 is to the left of r2
    // ret[1]: r1 is to the right of r2
    // ret[2]: r1 is above r2
    // ret[3]: r1 is below r2
    void RectOverlap(Rect r1, Rect r2, std::vector<bool> &ret) {
        ret[0] = ret[1] = ret[2] = ret[3] = false;
        if (r1.topLeft.x > r2.topLeft.x + r2.width) {
            ret[1] = true;
        }
        if (r1.topLeft.x + r2.width < r2.topLeft.x) {
            ret[0] = true;
        }
        if (r1.topLeft.y + r1.height < r2.topLeft.y) {
            ret[2] = true;
        }
        if (r1.topLeft.y > r2.topLeft.y + r2.height) {
            ret[3] = true;
        }
    }

    float starBounceHeight;
};

}  // namespace ECSEngine
