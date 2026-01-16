#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <optional>
#include "ImageProcessor.h"
#include "MaskLibrary.h"
#include "GUI.h"
#include "BlendMode.h"

namespace MaskOverlay {

class Application {
public:
    Application();
    ~Application();

    int run();

private:
    sf::RenderWindow m_window;
    sf::Font m_font;

    std::unique_ptr<ImageProcessor> m_processor;
    std::unique_ptr<MaskLibrary> m_maskLibrary;
    std::unique_ptr<GUI> m_gui;

    std::optional<sf::Sprite> m_sourceSprite;
    std::optional<sf::Sprite> m_maskSprite;
    std::optional<sf::Sprite> m_resultSprite;

    BlendModeType m_currentBlendMode;
    sf::Color m_transparentColor;
    bool m_useAlpha;
    sf::Vector2i m_maskOffset;

    enum class ViewMode {
        Source,
        Mask,
        Result,
        SplitView
    };
    ViewMode m_viewMode;

    bool m_draggingMask;
    sf::Vector2i m_dragStart;
    sf::Vector2i m_dragStartOffset;

    bool initialize();
    bool loadFont();
    void initializeCallbacks();
    void loadDefaultMasks();

    void handleEvents();
    void update();
    void render();

    void handleKeyPress(sf::Keyboard::Key key);
    void handleMousePress(sf::Mouse::Button button, const sf::Vector2f& pos);
    void handleMouseRelease(sf::Mouse::Button button, const sf::Vector2f& pos);
    void handleMouseMove(const sf::Vector2f& pos);
    void handleMouseWheel(float delta);

    void loadSourceImage();
    void loadMaskImage();
    void saveResult();
    void applyMask();
    void selectMaskFromLibrary(size_t index);

    std::string openFileDialog(const std::string& title, const std::string& filter);
    std::string saveFileDialog(const std::string& title, const std::string& filter);
    void updateSprites();
    void fitSpriteToArea(sf::Sprite& sprite, const sf::FloatRect& area);
    void setStatusMessage(const std::string& message);
};

}
