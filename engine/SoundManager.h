#pragma once

#include <unordered_map>

#include <SFML/Audio.hpp>

namespace ECSEngine {

class SoundManager {
public:
    SoundManager() : nullBuffer{}, nullSound{nullBuffer} {};
    ~SoundManager() = default;
    // creates and stores an sf::Sound from soundPath
    // it can be retrieved later using SoundManager::GetSound(soundName) during the lifetime of the engine
    void RegisterSound(const std::string& soundPath, const std::string& soundName);
    // returns the sound registered with soundName
    // this is valid for the lifetime of the engine or until another sound is registered with the same name
    sf::Sound& GetSound(const std::string& soundName);

private:
    std::unordered_map<std::string, sf::Sound> mSounds;
    sf::SoundBuffer nullBuffer;
    sf::Sound nullSound;
    std::unordered_map<std::string, sf::SoundBuffer> mSoundBuffers;
    std::unordered_map<std::string, sf::Sound> mSoundObjects;
};

}  // namespace ECSEngine
