#include "SpriteManager.h"

namespace ECSEngine {

SpriteID SpriteManager::RegisterTexture(const std::string& texturePath, const Rect& sourceRect) {
    if (mTextures.find(texturePath) ==mTextures.end()) {
        mTextures.emplace(texturePath, sf::Texture(texturePath));
    }
    sf::Texture& tex = mTextures.at(texturePath);

    SpriteID id = mSprites.size();
    sf::IntRect bounds{{static_cast<int>(sourceRect.topLeft.x), static_cast<int>(sourceRect.topLeft.y)}, {static_cast<int>(sourceRect.width), static_cast<int>(sourceRect.height)}};
    mSprites.emplace_back(sf::Sprite(tex, bounds));
    return id;
}

sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    return mSprites[id];
}

}  // namespace ECSEngine
