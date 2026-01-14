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

/**
 * Główna klasa aplikacji
 */
class Application {
public:
    Application();
    ~Application();

    /**
     * Uruchamia aplikację
     */
    int run();

private:
    // Okno i zasoby
    sf::RenderWindow m_window;
    sf::Font m_font;

    // Komponenty
    std::unique_ptr<ImageProcessor> m_processor;
    std::unique_ptr<MaskLibrary> m_maskLibrary;
    std::unique_ptr<GUI> m_gui;

    // Sprite'y do wyświetlania (optional dla SFML 3)
    std::optional<sf::Sprite> m_sourceSprite;
    std::optional<sf::Sprite> m_maskSprite;
    std::optional<sf::Sprite> m_resultSprite;

    // Stan
    BlendModeType m_currentBlendMode;
    sf::Color m_transparentColor;
    bool m_useAlpha;
    sf::Vector2i m_maskOffset;

    // Tryb wyświetlania
    enum class ViewMode {
        Source,
        Mask,
        Result,
        SplitView
    };
    ViewMode m_viewMode;

    // Przeciąganie maski
    bool m_draggingMask;
    sf::Vector2i m_dragStart;
    sf::Vector2i m_dragStartOffset;

    // Inicjalizacja
    bool initialize();
    bool loadFont();
    void initializeCallbacks();
    void loadDefaultMasks();

    // Główna pętla
    void handleEvents();
    void update();
    void render();

    // Obsługa zdarzeń
    void handleKeyPress(sf::Keyboard::Key key);
    void handleMousePress(sf::Mouse::Button button, const sf::Vector2f& pos);
    void handleMouseRelease(sf::Mouse::Button button, const sf::Vector2f& pos);
    void handleMouseMove(const sf::Vector2f& pos);
    void handleMouseWheel(float delta);

    // Akcje
    void loadSourceImage();
    void loadMaskImage();
    void saveResult();
    void applyMask();
    void selectMaskFromLibrary(size_t index);

    // Pomocnicze
    std::string openFileDialog(const std::string& title, const std::string& filter);
    std::string saveFileDialog(const std::string& title, const std::string& filter);
    void updateSprites();
    void fitSpriteToArea(sf::Sprite& sprite, const sf::FloatRect& area);
    void setStatusMessage(const std::string& message);
};

} // namespace MaskOverlay
