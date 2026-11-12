#pragma once

#include <unordered_map>

#include <SFML/Audio.hpp>

namespace ECSEngine {

class SoundManager {
public:
    SoundManager() : nullBuffer{""}, nullSound{nullBuffer} {};
    ~SoundManager() = default;
    void RegisterSound(const std::string& soundPath, const std::string& soundName);
    sf::Sound& GetSound(const std::string& soundName);

private:
    std::unordered_map<std::string, sf::Sound> mSounds;
    sf::SoundBuffer nullBuffer;
    sf::Sound nullSound;
};

}  // namespace ECSEngine
