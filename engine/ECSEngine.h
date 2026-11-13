#pragma once

#include <vector>

#include "CameraComponent.h"
#include "CollisionComponent.h"
#include "EntityManager.h"
#include "Keyboard.h"
#include "InputComponent.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "Keyboard.h"
#include "ScoreComponent.h"
#include "SoundManager.h"
#include "SpawnComponent.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace ECSEngine {

template <typename... Components>
class EngineView {
public:
    virtual SoundManager& GetSoundManager() = 0;
    virtual SpriteManager& GetSpriteManager() = 0;
    virtual EntityManager<Components...>& GetEntityManager() = 0;
    virtual WindowManager& GetWindowManager() = 0;
};

template <typename... Components>
class ECSEngine : public EngineView<Components...> {
public:
    ECSEngine(unsigned int width, unsigned int height, const std::string& name);
    ~ECSEngine();

    void Run();

    SoundManager& GetSoundManager() override { return mSoundManager; }

    SpriteManager& GetSpriteManager() override { return mSpriteManager; }

    EntityManager<Components...>& GetEntityManager() override { return mEntityManager; }

    WindowManager& GetWindowManager() override { return mWindowManager; }

    SystemManager<Components...>& GetSystemManager() { return mSystemManager; }

private:
    bool ProcessEvents();
    void DebugDraw();
    void PressKey(unsigned int k);
    void ReleaseKey(unsigned int k);

    EntityManager<Components...> mEntityManager;
    SystemManager<Components...> mSystemManager;
    SpriteManager mSpriteManager;
    SoundManager mSoundManager;
    WindowManager mWindowManager;

    // For debugging - pause and step the engine
    bool running = true; 
    bool step = false;
    int debug = 0;
};

template <typename... Components>
ECSEngine<Components...>::ECSEngine(unsigned int width, unsigned int height,
                                    const std::string& name)
    : mWindowManager(width, height, name) {}

template <typename... Components>
ECSEngine<Components...>::~ECSEngine() {}

template <typename... Components>
void ECSEngine<Components...>::Run() {
    while (mWindowManager.GetWindow()->isOpen()) {
        ProcessEvents();

        mWindowManager.GetWindow()->clear();
        if (running || step) {
            mSystemManager.Run(this);
            if (step) {
                step = false;
            }
        }

        if (debug) {
            DebugDraw();
        }
        mWindowManager.GetWindow()->display();
    }
}

template <typename... Components>
bool ECSEngine<Components...>::ProcessEvents() {
    while (const std::optional event = mWindowManager.GetWindow()->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            mWindowManager.GetWindow()->close();
        } else if (event->is<sf::Event::Resized>()) {
            
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            sf::Keyboard::Scan key = keyPressed->scancode;
            if (key == sf::Keyboard::Scan::Escape) {
                debug = !debug;
                PressKey(Key::Escape);
            } else if (key == sf::Keyboard::Scan::Right) {
                PressKey(Key::Right);
            } else if (key == sf::Keyboard::Scan::Left) {
                PressKey(Key::Left);
            } else if (key == sf::Keyboard::Scan::Up) {
                PressKey(Key::Up);
            } else if (key == sf::Keyboard::Scan::Down) {
                PressKey(Key::Down);
            } else if (key == sf::Keyboard::Scan::Z) {
                PressKey(Key::Z);
            } else if (key == sf::Keyboard::Scan::C) {
                PressKey(Key::C);
            }
        } else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            sf::Keyboard::Scan key = keyReleased->scancode;
            if (key == sf::Keyboard::Scan::Escape) {
                ReleaseKey(Key::Escape);
            } else if (key == sf::Keyboard::Scan::Right) {
                ReleaseKey(Key::Right);
            } else if (key == sf::Keyboard::Scan::Left) {
                ReleaseKey(Key::Left);
            } else if (key == sf::Keyboard::Scan::Up) {
                ReleaseKey(Key::Up);
            } else if (key == sf::Keyboard::Scan::Down) {
                ReleaseKey(Key::Down);
            } else if (key == sf::Keyboard::Scan::Z) {
                ReleaseKey(Key::Z);
            } else if (key == sf::Keyboard::Scan::C) {
                ReleaseKey(Key::C);
            }
        }
    }
    return true;
}

template <typename... Components>
void ECSEngine<Components...>::DebugDraw() {
    //mWindowManager.DebugFrameRect(mWindowManager.GetWindowBounds(), 255, 0, 0);

    for (Entity e : mEntityManager.mEntities) {
        if (!mEntityManager.HasComponent<CollisionComponent>(e.GetID()))
            continue;
        
        CollisionComponent c = mEntityManager.GetComponent<CollisionComponent>(e.GetID());
        LocationComponent l = mEntityManager.GetComponent<LocationComponent>(e.GetID());

        mWindowManager.DebugFrameRect(c.collisionBox + l.loc, 255, 0, 0);
    }
}

template <typename... Components>
void ECSEngine<Components...>::PressKey(unsigned int k) {
    ComponentStorage<InputComponent>& storage = mEntityManager.GetComponentStorage<InputComponent>();
    for (int i = 0; i < storage.mStorage.size(); i++) {
        if (!storage.mValid[i]) {
            continue;
        }
        storage.mStorage[i].keydown.set(k, true);
    }
}

template <typename... Components>
void ECSEngine<Components...>::ReleaseKey(unsigned int k) {
    ComponentStorage<InputComponent>& storage = mEntityManager.GetComponentStorage<InputComponent>();
    for (int i = 0; i < storage.mStorage.size(); i++) {
        if (!storage.mValid[i]) {
            continue;
        }
        storage.mStorage[i].keydown.set(k, false);
    }
}

}  // namespace ECSEngine
