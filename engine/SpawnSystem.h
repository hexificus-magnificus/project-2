#pragma once
#include <random>
#include <vector>
#include <cstdlib>
namespace ECSEngine {

template <typename... Components>
class SpawnSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpawnSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& entityManager = engine->GetEntityManager();
        std::vector<EntityID> spawners;
        for (const Entity& entity: entityManager){
            EntityID id = entity.GetID();
            if (entityManager.IsValid(id)&&entityManager.HasComponent<SpawnComponent>(id)){
                SpawnComponent& spawn = entityManager.GetComponent<SpawnComponent>(entity.GetID());

                if (spawn.totalSpawns>0){
                    spawn.timeToNextSpawn--;
                    if (spawn.timeToNextSpawn<=0){
                        spawners.push_back(id);
                        spawn.timeToNextSpawn = spawn.timeBetweenSpawns;
                        spawn.totalSpawns--;
                    }

                }
            }
        }
        for (EntityID spawnerID : spawners){

            auto spawn = entityManager.GetComponent<SpawnComponent>(spawnerID);
            LocationComponent spawnLocation = entityManager.GetComponent<LocationComponent>(spawnerID);

            EntityID spawnedID = entityManager.CreateEntity(spawn.spawnType);

            MovementComponent movement;
            int randVelocityx = (rand() % 51) - 25; //https://stackoverflow.com/a/12657984
            int randVelocityy = (rand() % 31) - 10; 
            movement.velocity = Point2D(randVelocityx, randVelocityy);
            entityManager.AddComponent<MovementComponent>(spawnedID, movement);

            LocationComponent location;
            location.loc = spawnLocation.loc;
            if (entityManager.HasComponent<CollisionComponent>(spawnerID)) {
                int velXDir = randVelocityx >= 0 ? 1 : -1;
                float bboxWidth = entityManager.GetComponent<CollisionComponent>(spawnerID).collisionBox.width;
                location.loc = location.loc + Point2D{bboxWidth * velXDir, 0.f};
            }
            entityManager.AddComponent<LocationComponent>(spawnedID, location);

            GravityComponent grav{{0.f, 2.f}, 0};
            entityManager.AddComponent<GravityComponent>(spawnedID, grav);

            SpriteComponent spriteComponent;
            spriteComponent.textureID = spawn.spawnTextureID;
            spriteComponent.worldSpace = true;
            auto& sprite = engine->GetSpriteManager().GetSprite(spawn.spawnTextureID);
            auto bounds = sprite.getLocalBounds();
            spriteComponent.bounds.topLeft = {bounds.position.x, bounds.position.y};
            spriteComponent.bounds.width = bounds.size.x;
            spriteComponent.bounds.height = bounds.size.y;
            entityManager.AddComponent<SpriteComponent>(spawnedID, spriteComponent);

            CollisionComponent collision{}; //initializes collision flags to 0 ChatGPT (may change)
            collision.staticEntity = false;
            collision.doesDamage = false;
            collision.collisionBox = spriteComponent.bounds;
            collision.previousCollisionBox = collision.collisionBox;
            entityManager.AddComponent<CollisionComponent>(spawnedID, collision);       
            
        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
