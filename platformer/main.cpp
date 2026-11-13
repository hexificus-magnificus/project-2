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
    if (!map.is_open()) {
        std::cerr << "Cannot open: " << path + file << std::endl;
        return;
    }

    if (!std::getline(map, line) || !std::regex_match(line, m, dict)) {
        std::cerr << "Invalid map header: " << file << std::endl;
        return;
    }
    int type = std::stoi(m[1].str());
    // get char-tile maps
    // tile line is of the form:
    // [char] [filename] [spritesheet top-left x] [spritesheet top-left y] [spritesheet width] [spritesheet height]
    // if dict type is 2, then also has
    // [bounding box top-left x] [bounding box top-left y] [bounding box width] [bounding box height]
    std::unordered_map<char, ECSEngine::SpriteComponent> tiles;
    std::unordered_map<char, ECSEngine::Rect> tileBB;
    std::regex tile;
    if (type == 1) {
        tile = std::regex("(.) ([^ ]*) (\\d*) (\\d*) (\\d*) (\\d*)");
    } else {
        tile = std::regex("(.) ([^ ]+) (\\d+) (\\d+) (\\d+) (\\d+) (-?\\d+) (-?\\d+) (\\d+) (\\d+)");
    }
    std::getline(map, line);
    

    while (std::regex_match(line, m, tile)) {
        std::string tileChar = m[1].str();
        std::string filename = m[2].str();
        int tlx = std::stoi(m[3].str());
        int tly = std::stoi(m[4].str());
        int w = std::stoi(m[5].str());
        int h = std::stoi(m[6].str());
        ECSEngine::Rect sBounds{{tlx, tly}, static_cast<float>(w), static_cast<float>(h)};
        ECSEngine::SpriteID sId = engine.GetSpriteManager().RegisterTexture(path + filename, sBounds);
        tiles[tileChar[0]] = {sId, ECSEngine::Rect{{0.f, 0.f}, static_cast<float>(w), static_cast<float>(h)}, true};

        if (type == 2) {
            int bbx = std::stoi(m[7].str());
            int bby = std::stoi(m[8].str());
            int bbw = std::stoi(m[9].str());
            int bbh = std::stoi(m[10].str());
            ECSEngine::Rect bb{{bbx, bby}, static_cast<float>(bbw), static_cast<float>(bbh)};
            tileBB[tileChar[0]] = bb;
        }
        std::getline(map, line);
    }

    // size is in tiles
    
    const std::regex mapDim("map origin (-?\\d+) (-?\\d+) tile (\\d+) (\\d+) size (\\d+) (\\d+)"); //const std::regex mapDim("map origin (\\d*) (\\d*) tile (\\d*) (\\d*) size (\\d*) (\\d*)");
    if (!std::regex_match(line, m, mapDim)) {
        std::cout << "Failed to parse map dimensions: [" << line << "]" << std::endl;
        return;
    }
    
    int ox = std::stoi(m[1].str());
    int oy = std::stoi(m[2].str());
    int tw = std::stoi(m[3].str());
    int th = std::stoi(m[4].str());
    int mw = std::stoi(m[5].str());
    int mh = std::stoi(m[6].str());
    ECSEngine::Point2D origin(ox, oy);
    ECSEngine::Point2D tileSize(tw, th);
    ECSEngine::SpriteID star = engine.GetSpriteManager().RegisterTexture(path + "spritesheet-tiles-default.png", {{640.f, 320.f}, 64.f, 64.f});

    std::getline(map, line);
    for (int row = 0; row < mh; row++) {
        for (int col = 0; col < mw; col++) {
            char cTile = line[col];
            if (tiles.find(cTile)==tiles.end()){
                continue;
            }
            ECSEngine::EntityID eTile = engine.GetEntityManager().CreateEntity(std::string("tile ") + cTile);
            ECSEngine::Point2D pos(origin.x+col*tileSize.x, origin.y+row*tileSize.y); //= origin + tileSize * ECSEngine::Point2D{row, col};
            engine.GetEntityManager().AddComponent<ECSEngine::LocationComponent>(eTile, {pos});
            engine.GetEntityManager().AddComponent<ECSEngine::SpriteComponent>(eTile, tiles[cTile]);

            if (type == 2) {
                uint8_t dmg = cTile == 'x' ? 1 : 0;
                ECSEngine::CollisionComponent coll{1, dmg, 0, 0, 0, 0, 0, 0, 0, 0, tileBB[cTile], tileBB[cTile]};
                engine.GetEntityManager().AddComponent<ECSEngine::CollisionComponent>(eTile, coll);
            }
            if (cTile == 'S') {
                // this is a spawner
                ECSEngine::SpawnComponent sp{eTile, "what does this even do", star, 120, 60, 10};
                engine.GetEntityManager().AddComponent<ECSEngine::SpawnComponent>(eTile, sp);
            }
        }

        std::getline(map, line);
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
    engine.GetWindowManager().SetWorldCenter({0.f, 384.f});

    // Load sounds
    engine.GetSoundManager().RegisterSound(gResourcePath + "footstep_grass_003.ogg", "land");
    engine.GetSoundManager().RegisterSound(gResourcePath + "footstep_snow_001.ogg", "wall");
    engine.GetSoundManager().RegisterSound(gResourcePath + "sfx_gem.ogg", "sget");
    engine.GetSoundManager().RegisterSound(gResourcePath + "sfx_jump.ogg", "jump");

    // Load maps
    LoadMap(engine, gResourcePath, "sky.map");
    LoadMap(engine, gResourcePath, "world.map");

    // Place player
    ECSEngine::EntityID player = engine.GetEntityManager().CreateEntity("player");
    engine.GetEntityManager().AddComponent<ECSEngine::LocationComponent>(player, {{800.f, 510.f}});
    engine.GetEntityManager().AddComponent<ECSEngine::RespawnComponent>(player, {{800.f, 510.f}});
    engine.GetEntityManager().AddComponent<ECSEngine::MovementComponent>(player, {{0, 0}});
    engine.GetEntityManager().AddComponent<ECSEngine::GravityComponent>(player, {{0.f, 2.f}, 0});
    engine.GetEntityManager().AddComponent<ECSEngine::ScoreComponent>(player, {0});
    engine.GetEntityManager().AddComponent<ECSEngine::CollisionComponent>(player, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ECSEngine::Rect{ECSEngine::Point2D(34.f, 32.f), 62.f, 96.f}, ECSEngine::Rect{ECSEngine::Point2D(32.f, 32.f), 64.f, 96.f}});
    ECSEngine::SpriteID playerSprite = engine.GetSpriteManager().RegisterTexture(gResourcePath + "spritesheet-characters-default.png", ECSEngine::Rect{{0.f, 0.f}, 128.f, 128.f});
    engine.GetEntityManager().AddComponent<ECSEngine::SpriteComponent>(player, {playerSprite, ECSEngine::Rect{ECSEngine::Point2D(32.f, 32.f), 64.f, 64.f}, true});
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
