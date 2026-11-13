#pragma once

#include <unordered_map>

#include <SFML/Audio.hpp>

namespace ECSEngine {

class SoundManager {
public:
    SoundManager() : nullBuffer{}, nullSound{nullBuffer} {};
    ~SoundManager() = default;
    // creates and stores an sf::Sound from soundPath
    // it can be retrieved later using SoundManager::GetSound(soundName)
    void RegisterSound(const std::string& soundPath, const std::string& soundName);
    // returns the sound registered with soundName
    sf::Sound& GetSound(const std::string& soundName);

private:
    std::unordered_map<std::string, sf::Sound> mSounds;
    sf::SoundBuffer nullBuffer;
    sf::Sound nullSound;
    std::unordered_map<std::string, sf::SoundBuffer> mSoundBuffers;
    std::unordered_map<std::string, sf::Sound> mSoundObjects;
};

}  // namespace ECSEngine
