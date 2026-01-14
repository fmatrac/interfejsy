#include "Application.h"
#include <iostream>
#include <cstdlib>
#include <optional>

#ifdef __APPLE__
#include <unistd.h>
#endif

namespace MaskOverlay {

Application::Application()
    : m_window(sf::VideoMode({1200, 800}), "Nakladanie masek - Projekt")
    , m_currentBlendMode(BlendModeType::Replace)
    , m_transparentColor(sf::Color::Magenta)
    , m_useAlpha(true)
    , m_maskOffset(0, 0)
    , m_viewMode(ViewMode::Source)
    , m_draggingMask(false)
    , m_dragStart(0, 0)
    , m_dragStartOffset(0, 0)
{
    m_window.setFramerateLimit(60);
}

Application::~Application() = default;

int Application::run() {
    if (!initialize()) {
        std::cerr << "Nie udalo sie zainicjalizowac aplikacji!" << std::endl;
        return 1;
    }
    
    while (m_window.isOpen()) {
        handleEvents();
        update();
        render();
    }
    
    return 0;
}

bool Application::initialize() {
    if (!loadFont()) {
        return false;
    }
    
    m_processor = std::make_unique<ImageProcessor>();
    m_maskLibrary = std::make_unique<MaskLibrary>();
    m_gui = std::make_unique<GUI>(m_window, m_font);
    
    initializeCallbacks();
    loadDefaultMasks();
    
    setStatusMessage("Gotowy - wczytaj obraz i maske");
    
    return true;
}

bool Application::loadFont() {
    std::vector<std::string> fontPaths = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/SFNSMono.ttf",
        "/System/Library/Fonts/Monaco.ttf",
        "/Library/Fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf"
    };
    
    for (const auto& path : fontPaths) {
        if (m_font.openFromFile(path)) {
            std::cout << "Zaladowano font: " << path << std::endl;
            return true;
        }
    }
    
    std::cerr << "Nie udalo sie zaladowac zadnego fontu!" << std::endl;
    return false;
}

void Application::initializeCallbacks() {
    m_gui->setOnLoadSource([this]() { loadSourceImage(); });
    m_gui->setOnLoadMask([this]() { loadMaskImage(); });
    m_gui->setOnSaveResult([this]() { saveResult(); });
    m_gui->setOnApplyMask([this]() { applyMask(); });
    
    m_gui->setOnBlendModeChange([this](BlendModeType mode) {
        m_currentBlendMode = mode;
        setStatusMessage("Tryb nakladania: " + BlendMode::getModeName(mode));
    });
    
    m_gui->setOnTransparentColorChange([this](const sf::Color& color) {
        m_transparentColor = color;
    });
    
    m_gui->setOnUseAlphaChange([this](bool use) {
        m_useAlpha = use;
        setStatusMessage(use ? "Kanal alfa: wlaczony" : "Kanal alfa: wylaczony");
    });
    
    m_gui->setOnMaskSelect([this](size_t index) {
        selectMaskFromLibrary(index);
    });
    
    m_gui->setOnMaskOffsetChange([this](int x, int y) {
        m_maskOffset = sf::Vector2i(x, y);
        m_processor->setMaskOffset(x, y);
    });
}

void Application::loadDefaultMasks() {
    std::vector<std::string> maskDirs = {
        "masks",
        "../masks",
        "./masks"
    };
    
    for (const auto& dir : maskDirs) {
        if (std::filesystem::exists(dir)) {
            m_maskLibrary->scanDirectory(dir);
            break;
        }
    }
    
    if (!m_maskLibrary->isEmpty()) {
        std::vector<std::pair<std::string, sf::Texture>> masks;
        for (const auto& entry : m_maskLibrary->getAllMasks()) {
            if (entry.loaded) {
                masks.push_back({entry.name, entry.thumbnail});
            }
        }
        m_gui->setMaskThumbnails(masks);
    }
}

void Application::handleEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            return;
        }
        
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            handleKeyPress(keyPressed->code);
        }
        
        if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f pos(static_cast<float>(mousePressed->position.x), 
                           static_cast<float>(mousePressed->position.y));
            handleMousePress(mousePressed->button, pos);
            m_gui->handleMouseClick(pos);
        }
        
        if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            sf::Vector2f pos(static_cast<float>(mouseReleased->position.x), 
                           static_cast<float>(mouseReleased->position.y));
            handleMouseRelease(mouseReleased->button, pos);
        }
        
        if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f pos(static_cast<float>(mouseMoved->position.x), 
                           static_cast<float>(mouseMoved->position.y));
            handleMouseMove(pos);
        }
        
        if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
            sf::Vector2f pos(static_cast<float>(sf::Mouse::getPosition(m_window).x), 
                           static_cast<float>(sf::Mouse::getPosition(m_window).y));
            handleMouseWheel(mouseWheel->delta);
            m_gui->handleMouseWheel(pos, mouseWheel->delta);
        }
        
        if (event->is<sf::Event::Resized>()) {
            m_gui->handleResize();
        }
    }
}

void Application::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Num1:
            m_viewMode = ViewMode::Source;
            setStatusMessage("Widok: obraz zrodlowy");
            break;
        case sf::Keyboard::Key::Num2:
            m_viewMode = ViewMode::Mask;
            setStatusMessage("Widok: maska");
            break;
        case sf::Keyboard::Key::Num3:
            m_viewMode = ViewMode::Result;
            setStatusMessage("Widok: wynik");
            break;
        case sf::Keyboard::Key::Num4:
            m_viewMode = ViewMode::SplitView;
            setStatusMessage("Widok: podzielony");
            break;
        case sf::Keyboard::Key::Space:
            applyMask();
            break;
        case sf::Keyboard::Key::S:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                saveResult();
            }
            break;
        case sf::Keyboard::Key::O:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl)) {
                loadSourceImage();
            }
            break;
        case sf::Keyboard::Key::R:
            m_processor->resetMaskOffset();
            m_maskOffset = sf::Vector2i(0, 0);
            setStatusMessage("Przesuniecie maski zresetowane");
            break;
        case sf::Keyboard::Key::Escape:
            m_window.close();
            break;
        default:
            break;
    }
}

void Application::handleMousePress(sf::Mouse::Button button, const sf::Vector2f& pos) {
    sf::FloatRect previewArea = m_gui->getPreviewArea();
    
    if (button == sf::Mouse::Button::Left && previewArea.contains(pos)) {
        if (m_processor->hasMask() && m_processor->hasSourceImage()) {
            sf::Vector2u maskSize = m_processor->getMaskSize();
            sf::Vector2u sourceSize = m_processor->getSourceSize();
            
            if (maskSize.x > sourceSize.x || maskSize.y > sourceSize.y) {
                m_draggingMask = true;
                m_dragStart = sf::Vector2i(static_cast<int>(pos.x), static_cast<int>(pos.y));
                m_dragStartOffset = m_maskOffset;
            }
        }
    }
}

void Application::handleMouseRelease(sf::Mouse::Button button, const sf::Vector2f& pos) {
    if (button == sf::Mouse::Button::Left) {
        m_draggingMask = false;
    }
}

void Application::handleMouseMove(const sf::Vector2f& pos) {
    if (m_draggingMask) {
        int dx = static_cast<int>(pos.x) - m_dragStart.x;
        int dy = static_cast<int>(pos.y) - m_dragStart.y;
        
        m_maskOffset.x = m_dragStartOffset.x - dx;
        m_maskOffset.y = m_dragStartOffset.y - dy;
        
        m_processor->setMaskOffset(m_maskOffset.x, m_maskOffset.y);
        
        if (m_processor->hasSourceImage() && m_processor->hasMask()) {
            applyMask();
        }
    }
}

void Application::handleMouseWheel(float delta) {
}

void Application::update() {
    m_gui->update();
    updateSprites();
}

void Application::render() {
    m_window.clear(sf::Color(60, 60, 65));
    
    sf::FloatRect previewArea = m_gui->getPreviewArea();
    sf::RectangleShape previewBg(sf::Vector2f(previewArea.size.x, previewArea.size.y));
    previewBg.setPosition(sf::Vector2f(previewArea.position.x, previewArea.position.y));
    previewBg.setFillColor(sf::Color(50, 50, 55));
    m_window.draw(previewBg);
    
    switch (m_viewMode) {
        case ViewMode::Source:
            if (m_processor->hasSourceImage() && m_sourceSprite.has_value()) {
                m_window.draw(*m_sourceSprite);
            }
            break;
            
        case ViewMode::Mask:
            if (m_processor->hasMask() && m_maskSprite.has_value()) {
                m_window.draw(*m_maskSprite);
            }
            break;
            
        case ViewMode::Result:
            if (m_processor->hasResult() && m_resultSprite.has_value()) {
                m_window.draw(*m_resultSprite);
            } else if (m_processor->hasSourceImage() && m_sourceSprite.has_value()) {
                m_window.draw(*m_sourceSprite);
            }
            break;
            
        case ViewMode::SplitView:
            if (m_processor->hasSourceImage() && m_sourceSprite.has_value()) {
                sf::Sprite leftSprite = *m_sourceSprite;
                fitSpriteToArea(leftSprite, sf::FloatRect(
                    sf::Vector2f(previewArea.position.x, previewArea.position.y),
                    sf::Vector2f(previewArea.size.x / 2 - 5, previewArea.size.y)
                ));
                m_window.draw(leftSprite);
            }
            
            if (m_processor->hasResult() && m_resultSprite.has_value()) {
                sf::Sprite rightSprite = *m_resultSprite;
                fitSpriteToArea(rightSprite, sf::FloatRect(
                    sf::Vector2f(previewArea.position.x + previewArea.size.x / 2 + 5, previewArea.position.y),
                    sf::Vector2f(previewArea.size.x / 2 - 5, previewArea.size.y)
                ));
                m_window.draw(rightSprite);
            }
            break;
    }
    
    m_gui->draw();
    
    sf::Text helpText(m_font, "1-Zrodlo  2-Maska  3-Wynik  4-Podziel  Spacja-Zastosuj  R-Reset  Ctrl+S-Zapisz", 11);
    helpText.setFillColor(sf::Color(150, 150, 150));
    helpText.setPosition(sf::Vector2f(previewArea.position.x + 10, static_cast<float>(m_window.getSize().y) - 25));
    m_window.draw(helpText);
    
    m_window.display();
}

void Application::loadSourceImage() {
    std::string path = openFileDialog("Wybierz obraz zrodlowy", "*.bmp;*.png;*.jpg");
    
    if (!path.empty()) {
        if (m_processor->loadSourceImage(path)) {
            m_gui->setHasSource(true);
            m_gui->setSourceSize(m_processor->getSourceSize());
            updateSprites();
            setStatusMessage("Wczytano obraz: " + std::filesystem::path(path).filename().string());
            m_viewMode = ViewMode::Source;
        } else {
            setStatusMessage("Blad wczytywania obrazu!");
        }
    }
}

void Application::loadMaskImage() {
    std::string path = openFileDialog("Wybierz maske", "*.bmp;*.png;*.jpg");
    
    if (!path.empty()) {
        if (m_processor->loadMask(path)) {
            m_gui->setHasMask(true);
            m_gui->setMaskSize(m_processor->getMaskSize());
            m_maskOffset = sf::Vector2i(0, 0);
            updateSprites();
            setStatusMessage("Wczytano maske: " + std::filesystem::path(path).filename().string());
            m_viewMode = ViewMode::Mask;
        } else {
            setStatusMessage("Blad wczytywania maski!");
        }
    }
}

void Application::saveResult() {
    if (!m_processor->hasResult()) {
        setStatusMessage("Brak wyniku do zapisania!");
        return;
    }
    
    std::string path = saveFileDialog("Zapisz wynik", "*.bmp;*.png");
    
    if (!path.empty()) {
        if (path.find('.') == std::string::npos) {
            path += ".bmp";
        }
        
        if (m_processor->saveResult(path)) {
            setStatusMessage("Zapisano: " + std::filesystem::path(path).filename().string());
        } else {
            setStatusMessage("Blad zapisywania wyniku!");
        }
    }
}

void Application::applyMask() {
    if (!m_processor->hasSourceImage() || !m_processor->hasMask()) {
        setStatusMessage("Wczytaj obraz i maske!");
        return;
    }
    
    m_processor->applyMask(m_currentBlendMode, m_transparentColor, m_useAlpha);
    m_gui->setHasResult(true);
    updateSprites();
    m_viewMode = ViewMode::Result;
    setStatusMessage("Zastosowano maske w trybie: " + BlendMode::getModeName(m_currentBlendMode));
}

void Application::selectMaskFromLibrary(size_t index) {
    std::string path = m_maskLibrary->getMaskPath(index);
    
    if (!path.empty()) {
        if (m_processor->loadMask(path)) {
            m_gui->setHasMask(true);
            m_gui->setMaskSize(m_processor->getMaskSize());
            m_maskOffset = sf::Vector2i(0, 0);
            updateSprites();
            setStatusMessage("Wybrano maske: " + m_maskLibrary->getMask(index).name);
            m_viewMode = ViewMode::Mask;
        }
    }
}

std::string Application::openFileDialog(const std::string& title, const std::string& filter) {
#ifdef __APPLE__
    std::string command = "osascript -e 'POSIX path of (choose file with prompt \"" + title + "\")'";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    char buffer[1024];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
#else
    std::cout << title << ": ";
    std::string path;
    std::getline(std::cin, path);
    return path;
#endif
}

std::string Application::saveFileDialog(const std::string& title, const std::string& filter) {
#ifdef __APPLE__
    std::string command = "osascript -e 'POSIX path of (choose file name with prompt \"" + title + "\" default name \"wynik.bmp\")'";
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    char buffer[1024];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
#else
    std::cout << title << ": ";
    std::string path;
    std::getline(std::cin, path);
    return path;
#endif
}

void Application::updateSprites() {
    sf::FloatRect previewArea = m_gui->getPreviewArea();
    
    if (m_processor->hasSourceImage()) {
        m_sourceSprite.emplace(m_processor->getSourceTexture());
        fitSpriteToArea(*m_sourceSprite, previewArea);
    }
    
    if (m_processor->hasMask()) {
        m_maskSprite.emplace(m_processor->getMaskTexture());
        fitSpriteToArea(*m_maskSprite, previewArea);
    }
    
    if (m_processor->hasResult()) {
        m_resultSprite.emplace(m_processor->getResultTexture());
        fitSpriteToArea(*m_resultSprite, previewArea);
    }
}

void Application::fitSpriteToArea(sf::Sprite& sprite, const sf::FloatRect& area) {
    sf::Vector2u texSize = sprite.getTexture().getSize();
    
    float scaleX = area.size.x / texSize.x;
    float scaleY = area.size.y / texSize.y;
    float scale = std::min(scaleX, scaleY);
    
    sprite.setScale(sf::Vector2f(scale, scale));
    
    float spriteWidth = texSize.x * scale;
    float spriteHeight = texSize.y * scale;
    
    float x = area.position.x + (area.size.x - spriteWidth) / 2;
    float y = area.position.y + (area.size.y - spriteHeight) / 2;
    
    sprite.setPosition(sf::Vector2f(x, y));
}

void Application::setStatusMessage(const std::string& message) {
    m_gui->setStatusMessage(message);
    std::cout << "[STATUS] " << message << std::endl;
}

} // namespace MaskOverlay
