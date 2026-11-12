#include "SpriteManager.h"

namespace ECSEngine {

SpriteID SpriteManager::RegisterTexture(const std::string& texturePath, const Rect& sourceRect) {
    sf::Texture tex;
    if (!mTextures.contains(texturePath)) {
        mTextures.emplace(texturePath, sf::Texture(texturePath));
    }
    tex = mTextures.at(texturePath);

    SpriteID id = mSprites.size();
    sf::IntRect bounds{{sourceRect.topLeft.x, sourceRect.topLeft.y}, {sourceRect.width, sourceRect.height}};
    mSprites.emplace_back(sf::Sprite(tex, bounds));
    return id;
}

sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    return mSprites[id];
}

}  // namespace ECSEngine
