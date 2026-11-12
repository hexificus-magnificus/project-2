#include "WindowManager.h"

namespace ECSEngine {

WindowManager::WindowManager(unsigned int width, unsigned int height, const std::string& title)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), title)), mWorldCenter(0, 0), mWorldScale(1.0f) {}

std::shared_ptr<sf::RenderWindow> WindowManager::GetWindow() const { return mWindow; }

Rect WindowManager::GetWindowBounds() const { 
    Rect windowBounds;
    windowBounds.topLeft = {0,0};
    windowBounds.width = static_cast<float>(mWindow->getSize().x);
    windowBounds.height = static_cast<float>(mWindow->getSize().y);
    return windowBounds;
}

// assumes DebugDrawRect takes rect with world coordinates
void WindowManager::DebugDrawRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(WorldToWindow(r.width), WorldToWindow(r.height)));
    sf::Color col = sf::Color(red, green, blue, alpha);
    rect.setFillColor(col);
    Point2D windowTopLeft = WorldToWindow(r.topLeft);
    rect.setPosition(sf::Vector2f(windowTopLeft.x, windowTopLeft.y));
    mWindow->draw(rect);
}

// assumes DebugFrameRect takes rect with world coordinates
void WindowManager::DebugFrameRect(const Rect& r, int red, int green, int blue, int alpha) {
    sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(WorldToWindow(r.width), WorldToWindow(r.height)));
    sf::Color col = sf::Color(red, green, blue, alpha);
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(col);
    rect.setOutlineThickness(1.0f);
    Point2D windowTopLeft = WorldToWindow(r.topLeft);
    rect.setPosition(sf::Vector2f(windowTopLeft.x, windowTopLeft.y));
    mWindow->draw(rect);
}

Rect WindowManager::WindowToWorld(const Rect& rect) const { 
    Rect worldRect;
    worldRect.topLeft = WindowToWorld(rect.topLeft);
    worldRect.height = WindowToWorld(rect.height);
    worldRect.width = WindowToWorld(rect.width);
    return worldRect; 
}

Point2D WindowManager::WindowToWorld(const Point2D& pt) const { 
    float x = (pt.x - mWindow->getSize().x/2)*mWorldScale+mWorldCenter.x;
    float y = (pt.y - mWindow->getSize().y/2)*mWorldScale+mWorldCenter.y;
    return Point2D(x, y);
}

Rect WindowManager::WorldToWindow(const Rect& rect) const {
    Rect windowRect;
    windowRect.topLeft = WorldToWindow(rect.topLeft);
    windowRect.height = WorldToWindow(rect.height);
    windowRect.width = WorldToWindow(rect.width);
    return windowRect; 
}

Point2D WindowManager::WorldToWindow(const Point2D& pt) const { 
    float x = (pt.x-mWorldCenter.x)/mWorldScale + (mWindow->getSize().x/2);
    float y = (pt.y-mWorldCenter.y)/mWorldScale + (mWindow->getSize().y/2);
    return Point2D(x, y); 
}

void WindowManager::SetCamera(const Point2D& worldPt, const Point2D& screenPt) {
    float x = -(screenPt.x-(mWindow->getSize().x/2))*mWorldScale+worldPt.x;
    float y = -(screenPt.y-(mWindow->getSize().y/2))*mWorldScale+worldPt.y;
    mWorldCenter = Point2D(x, y);
}

Point2D WindowManager::GetWorldCenter() const { 
    return mWorldCenter; 
}

float WindowManager::GetWorldScale() const { 
    return mWorldScale; 
}

void WindowManager::SetWorldCenter(const Point2D& worldCenter) {
    mWorldCenter = worldCenter;
}

void WindowManager::SetWorldScale(float worldUnitsPerPixel) {
    mWorldScale = worldUnitsPerPixel;
}

void WindowManager::SetWorldRect(const Rect& worldCameraRect) {
    mWorldCenter.x = worldCameraRect.topLeft.x + worldCameraRect.width/2;
    mWorldCenter.y = worldCameraRect.topLeft.y + worldCameraRect.height/2;
    mWorldScale = worldCameraRect.height/mWindow->getSize().y;
}

float WindowManager::WindowToWorld(float x) const { 
    return x*mWorldScale; 
}

float WindowManager::WorldToWindow(float x) const { 
    return x/mWorldScale; 
}

}  // namespace ECSEngine
