#include <fstream>
#include <regex>

#include "CameraSystem.h"
#include "CollisionComponent.h"
#include "CollisionLateSystem.h"
#include "CollisionSystem.h"
#include "ECSEngine.h"
#include "EntityManager.h"
#include "GravitySystem.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "MovementSystem.h"
#include "ScoreComponent.h"
#include "ScoreSystem.h"
#include "SpawnComponent.h"
#include "SpawnSystem.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"

std::string gResourcePath = "assets/";
#define COMPONENTS                                                                             \
    ECSEngine::LocationComponent, ECSEngine::MovementComponent, ECSEngine::CollisionComponent, \
        ECSEngine::SpriteComponent, ECSEngine::SpawnComponent, ECSEngine::CameraComponent,     \
        ECSEngine::CameraFollower, ECSEngine::InputComponent, ECSEngine::GravityComponent,     \
        ECSEngine::CameraShake, ECSEngine::ScoreComponent, ECSEngine::RespawnComponent
using Engine = ECSEngine::ECSEngine<COMPONENTS>;

void LoadMap(Engine& engine, const std::string& path, const std::string& file) {
    std::ifstream map(path + file);
    std::string line;
    std::smatch m;

    const std::regex dict("dictionary (\\d)");
    std::getline(map, line);
    std::regex_match(line, m, dict);
    int type = std::stoi(m[1].str());

    std::unordered_map<std::string, ECSEngine::SpriteComponent> tiles;
    if (type == 1) {
        const std::regex tile("(.) ([^ ]*) (\\d*) (\\d*) (\\d*) (\\d*)");
    } else {
        
    }
}

int main(int argc, char* argv[]) {
    if (argc >= 3 && argv[1] == std::string("-path")) {
        gResourcePath = argv[2];
    }
    std::cout << "Usage: " << argv[0] << " [-path resource_path]\n";
    std::cout << "Using resource path: " << gResourcePath << "\n";

    // Engine test code
    Engine engine(1024, 768, "Test Engine");
    engine.GetWindowManager().GetWindow()->setFramerateLimit(60);

    // Load sounds
    engine.GetSoundManager().RegisterSound(gResourcePath + "footstep_grass_003.ogg", "land");
    engine.GetSoundManager().RegisterSound(gResourcePath + "footstep_snow_001.ogg", "wall");
    engine.GetSoundManager().RegisterSound(gResourcePath + "sfx_gem.ogg", "sget");
    engine.GetSoundManager().RegisterSound(gResourcePath + "sfx_jump.ogg", "jump");

    // Load maps
    // LoadMap(engine, gResourcePath, "sky.map");
    // LoadMap(engine, gResourcePath, "world.map");

    // Place player
    ECSEngine::EntityID player = engine.GetEntityManager().CreateEntity("player");
    engine.GetEntityManager().AddComponent<ECSEngine::LocationComponent>(player, {{0, 0}});
    engine.GetEntityManager().AddComponent<ECSEngine::MovementComponent>(player, {{0, 0}});
    engine.GetEntityManager().AddComponent<ECSEngine::GravityComponent>(player, {{0.f, 0.01f}, 0});
    engine.GetEntityManager().AddComponent<ECSEngine::ScoreComponent>(player, {0});
    engine.GetEntityManager().AddComponent<ECSEngine::CollisionComponent>(player, {0, 0, 0, 0, 0, 0, ECSEngine::Rect{ECSEngine::Point2D(0.f, 0.f), 128.f, 128.f}, ECSEngine::Rect{ECSEngine::Point2D(0.f, 0.f), 128.f, 128.f}});
    ECSEngine::SpriteID playerSprite = engine.GetSpriteManager().RegisterTexture(gResourcePath + "spritesheet-characters-default.png", ECSEngine::Rect{{0.f, 0.f}, 128.f, 128.f});
    engine.GetEntityManager().AddComponent<ECSEngine::SpriteComponent>(player, {playerSprite, ECSEngine::Rect{ECSEngine::Point2D(0.f, 0.f), 128.f, 128.f}, true});
    engine.GetEntityManager().AddComponent<ECSEngine::InputComponent>(player, {player});
    

    // Setup score component
    ECSEngine::ScoreComponent& score = engine.GetEntityManager().GetComponent<ECSEngine::ScoreComponent>(player);
    ECSEngine::Point2D start(832.f, 832.f);
    for (int i = 0; i < 10; i++) {
        ECSEngine::Rect r{start, 64, 64};
        r.topLeft = r.topLeft - ECSEngine::Point2D(0.f, i * 64.f);
        score.digitTextureID[i] = engine.GetSpriteManager().RegisterTexture(gResourcePath + "spritesheet-tiles-default.png", r);
    }

    for (int i = 0; i<3; i++){
        ECSEngine::EntityID digit = engine.GetEntityManager().CreateEntity("digit");
        engine.GetEntityManager().AddComponent<ECSEngine::LocationComponent>(digit, {{10+(i*64), 10}});
        engine.GetEntityManager().AddComponent<ECSEngine::SpriteComponent>(digit, {score.digitTextureID[0], ECSEngine::Rect{{0.f, 0.f}, 64.f, 64.f}, false});
        score.digitID[i] = digit;

    }
    

    // Setup Camera
    auto windowSize = engine.GetWindowManager().GetWindow()->getSize();
    float width = static_cast<float>(windowSize.x);
    float height = static_cast<float>(windowSize.y);
    ECSEngine::EntityID camera = engine.GetEntityManager().CreateEntity("camera");
    ECSEngine::Rect hardRect{{width*0.1f, 0.f}, width*0.8f, height};
    ECSEngine::Rect softRect{{width*0.3f, 0.f}, width*0.4f, height};
    engine.GetEntityManager().AddComponent<ECSEngine::CameraComponent>(camera, {0, 1, hardRect, softRect, 0.1f});
    engine.GetEntityManager().AddComponent<ECSEngine::CameraFollower>(camera, {player});
    engine.GetEntityManager().AddComponent<ECSEngine::CameraShake>(camera, {});

    // Add all systems
    
    auto& sm = engine.GetSystemManager();
    sm.AddSystem(std::make_unique<ECSEngine::CollisionLateSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::InputSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::GravitySystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::MovementSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CollisionSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::ScoreSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CameraSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpriteSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpawnSystem<COMPONENTS>>());
    
    engine.Run();
    return 0;
}
