#include "SoundManager.h"

namespace ECSEngine {

void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName) {
    sf::SoundBuffer buffer{soundPath};
    sf::Sound sound{buffer};
    mSounds.emplace(soundName, sound);
}

sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    if (mSounds.contains(soundName)) {
        return mSounds.at(soundName);
    } else {
        return nullSound;
    }
}

}  // namespace ECSEngine
