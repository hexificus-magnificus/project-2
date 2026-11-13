#include "SoundManager.h"

namespace ECSEngine {

void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName) {
    mSoundBuffers.emplace(soundName, soundPath);
    sf::Sound sound{mSoundBuffers.at(soundName)};
    mSoundObjects.emplace(soundName, sound);
}

sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    if (mSoundObjects.contains(soundName)) {
        return mSoundObjects.at(soundName);
    } else {
        return nullSound;
    }
}

}  // namespace ECSEngine
