#include "GUI.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace MaskOverlay {

// ==================== Button ====================

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, 
               const std::string& label, const sf::Font& font)
    : m_font(&font)
    , m_selected(false)
    , m_enabled(true)
    , m_hovered(false)
    , m_normalColor(sf::Color(70, 70, 70))
    , m_hoverColor(sf::Color(90, 90, 90))
    , m_selectedColor(sf::Color(50, 120, 200))
    , m_disabledColor(sf::Color(50, 50, 50))
{
    m_shape.setPosition(position);
    m_shape.setSize(size);
    m_shape.setFillColor(m_normalColor);
    m_shape.setOutlineThickness(1);
    m_shape.setOutlineColor(sf::Color(100, 100, 100));
    
    m_text.emplace(font, label, 14);
    m_text->setFillColor(sf::Color::White);
    
    // Wyśrodkuj tekst
    sf::FloatRect textBounds = m_text->getLocalBounds();
    m_text->setOrigin({textBounds.position.x + textBounds.size.x / 2,
                       textBounds.position.y + textBounds.size.y / 2});
    m_text->setPosition({position.x + size.x / 2, position.y + size.y / 2});
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
    if (m_text) {
        window.draw(*m_text);
    }
}

bool Button::isClicked(const sf::Vector2f& mousePos) const {
    return m_enabled && m_shape.getGlobalBounds().contains(mousePos);
}

bool Button::contains(const sf::Vector2f& point) const {
    return m_shape.getGlobalBounds().contains(point);
}

void Button::setSelected(bool selected) {
    m_selected = selected;
    update(sf::Vector2f(-1, -1));
}

bool Button::isSelected() const {
    return m_selected;
}

void Button::setEnabled(bool enabled) {
    m_enabled = enabled;
    update(sf::Vector2f(-1, -1));
}

bool Button::isEnabled() const {
    return m_enabled;
}

void Button::setLabel(const std::string& label) {
    if (m_text) {
        m_text->setString(label);
        sf::FloatRect textBounds = m_text->getLocalBounds();
        m_text->setOrigin({textBounds.position.x + textBounds.size.x / 2,
                           textBounds.position.y + textBounds.size.y / 2});
    }
}

std::string Button::getLabel() const {
    if (m_text) {
        return m_text->getString();
    }
    return "";
}

void Button::setPosition(const sf::Vector2f& position) {
    m_shape.setPosition(position);
    sf::Vector2f size = m_shape.getSize();
    if (m_text) {
        sf::FloatRect textBounds = m_text->getLocalBounds();
        m_text->setPosition({position.x + size.x / 2, position.y + size.y / 2});
    }
}

void Button::setSize(const sf::Vector2f& size) {
    m_shape.setSize(size);
    sf::Vector2f position = m_shape.getPosition();
    if (m_text) {
        m_text->setPosition({position.x + size.x / 2, position.y + size.y / 2});
    }
}

void Button::setColors(const sf::Color& normal, const sf::Color& hover,
                      const sf::Color& selected, const sf::Color& disabled) {
    m_normalColor = normal;
    m_hoverColor = hover;
    m_selectedColor = selected;
    m_disabledColor = disabled;
    update(sf::Vector2f(-1, -1));
}

void Button::update(const sf::Vector2f& mousePos) {
    m_hovered = m_shape.getGlobalBounds().contains(mousePos);
    
    if (!m_enabled) {
        m_shape.setFillColor(m_disabledColor);
        if (m_text) m_text->setFillColor(sf::Color(150, 150, 150));
    } else if (m_selected) {
        m_shape.setFillColor(m_selectedColor);
        if (m_text) m_text->setFillColor(sf::Color::White);
    } else if (m_hovered) {
        m_shape.setFillColor(m_hoverColor);
        if (m_text) m_text->setFillColor(sf::Color::White);
    } else {
        m_shape.setFillColor(m_normalColor);
        if (m_text) m_text->setFillColor(sf::Color::White);
    }
}

// ==================== Slider ====================

Slider::Slider(const sf::Vector2f& position, float width,
               float minValue, float maxValue, const sf::Font& font,
               const std::string& label)
    : m_font(&font)
    , m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_value(minValue)
    , m_dragging(false)
    , m_position(position)
    , m_width(width)
{
    m_track.setPosition({position.x, position.y + 20});
    m_track.setSize(sf::Vector2f(width, 6));
    m_track.setFillColor(sf::Color(50, 50, 50));
    m_track.setOutlineThickness(1);
    m_track.setOutlineColor(sf::Color(80, 80, 80));
    
    m_thumb.setSize(sf::Vector2f(14, 18));
    m_thumb.setFillColor(sf::Color(100, 150, 200));
    m_thumb.setOutlineThickness(1);
    m_thumb.setOutlineColor(sf::Color(150, 200, 255));
    
    m_label.emplace(font, label, 12);
    m_label->setFillColor(sf::Color::White);
    m_label->setPosition({position.x, position.y});
    
    m_valueText.emplace(font, "", 12);
    m_valueText->setFillColor(sf::Color(200, 200, 200));
    m_valueText->setPosition({position.x + width + 10, position.y + 17});
    
    updateThumbPosition();
}

void Slider::draw(sf::RenderWindow& window) {
    if (m_label) window.draw(*m_label);
    window.draw(m_track);
    window.draw(m_thumb);
    if (m_valueText) window.draw(*m_valueText);
}

bool Slider::handleEvent(const sf::Vector2f& mousePos, bool mousePressed, bool mouseReleased) {
    if (mousePressed) {
        if (m_thumb.getGlobalBounds().contains(mousePos) ||
            m_track.getGlobalBounds().contains(mousePos)) {
            m_dragging = true;
            
            // Ustaw wartość na pozycję kliknięcia
            float trackX = m_track.getPosition().x;
            float trackWidth = m_track.getSize().x;
            float relativeX = std::clamp(mousePos.x - trackX, 0.0f, trackWidth);
            m_value = m_minValue + (relativeX / trackWidth) * (m_maxValue - m_minValue);
            updateThumbPosition();
            return true;
        }
    }
    
    if (mouseReleased) {
        m_dragging = false;
    }
    
    if (m_dragging) {
        float trackX = m_track.getPosition().x;
        float trackWidth = m_track.getSize().x;
        float relativeX = std::clamp(mousePos.x - trackX, 0.0f, trackWidth);
        m_value = m_minValue + (relativeX / trackWidth) * (m_maxValue - m_minValue);
        updateThumbPosition();
        return true;
    }
    
    return false;
}

float Slider::getValue() const {
    return m_value;
}

void Slider::setValue(float value) {
    m_value = std::clamp(value, m_minValue, m_maxValue);
    updateThumbPosition();
}

void Slider::setLabel(const std::string& label) {
    if (m_label) {
        m_label->setString(label);
    }
}

void Slider::updateThumbPosition() {
    float trackX = m_track.getPosition().x;
    float trackY = m_track.getPosition().y;
    float trackWidth = m_track.getSize().x;
    
    float ratio = (m_value - m_minValue) / (m_maxValue - m_minValue);
    float thumbX = trackX + ratio * trackWidth - m_thumb.getSize().x / 2;
    
    m_thumb.setPosition({thumbX, trackY - 6});
    
    if (m_valueText) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << m_value;
        m_valueText->setString(ss.str());
    }
}

// ==================== ColorPicker ====================

ColorPicker::ColorPicker(const sf::Vector2f& position, const sf::Font& font)
    : m_position(position)
    , m_font(&font)
    , m_color(sf::Color::Magenta)
{
    m_label.emplace(font, "Kolor przezroczysty:", 13);
    m_label->setFillColor(sf::Color::White);
    m_label->setPosition({position.x, position.y});
    
    m_preview.setPosition({position.x + 170, position.y + 80});
    m_preview.setSize(sf::Vector2f(40, 40));
    m_preview.setOutlineThickness(2);
    m_preview.setOutlineColor(sf::Color::White);
    
    m_redSlider = std::make_unique<Slider>(sf::Vector2f(position.x, position.y + 30), 150, 0, 255, font, "R:");
    m_greenSlider = std::make_unique<Slider>(sf::Vector2f(position.x, position.y + 75), 150, 0, 255, font, "G:");
    m_blueSlider = std::make_unique<Slider>(sf::Vector2f(position.x, position.y + 120), 150, 0, 255, font, "B:");
    m_alphaSlider = std::make_unique<Slider>(sf::Vector2f(position.x, position.y + 165), 150, 0, 255, font, "A:");
    
    // Ustaw domyślny kolor (magenta - często używany jako przezroczysty)
    m_redSlider->setValue(255);
    m_greenSlider->setValue(0);
    m_blueSlider->setValue(255);
    m_alphaSlider->setValue(255);
    
    updatePreview();
}

void ColorPicker::draw(sf::RenderWindow& window) {
    if (m_label) window.draw(*m_label);
    window.draw(m_preview);
    if (m_redSlider) m_redSlider->draw(window);
    if (m_greenSlider) m_greenSlider->draw(window);
    if (m_blueSlider) m_blueSlider->draw(window);
    if (m_alphaSlider) m_alphaSlider->draw(window);
}

bool ColorPicker::handleEvent(const sf::Vector2f& mousePos, bool mousePressed, bool mouseReleased) {
    bool handled = false;
    
    if (m_redSlider) handled |= m_redSlider->handleEvent(mousePos, mousePressed, mouseReleased);
    if (m_greenSlider) handled |= m_greenSlider->handleEvent(mousePos, mousePressed, mouseReleased);
    if (m_blueSlider) handled |= m_blueSlider->handleEvent(mousePos, mousePressed, mouseReleased);
    if (m_alphaSlider) handled |= m_alphaSlider->handleEvent(mousePos, mousePressed, mouseReleased);
    
    if (handled) {
        m_color = sf::Color(
            static_cast<std::uint8_t>(m_redSlider->getValue()),
            static_cast<std::uint8_t>(m_greenSlider->getValue()),
            static_cast<std::uint8_t>(m_blueSlider->getValue()),
            static_cast<std::uint8_t>(m_alphaSlider->getValue())
        );
        updatePreview();
    }
    
    return handled;
}

sf::Color ColorPicker::getColor() const {
    return m_color;
}

void ColorPicker::setColor(const sf::Color& color) {
    m_color = color;
    if (m_redSlider) m_redSlider->setValue(color.r);
    if (m_greenSlider) m_greenSlider->setValue(color.g);
    if (m_blueSlider) m_blueSlider->setValue(color.b);
    if (m_alphaSlider) m_alphaSlider->setValue(color.a);
    updatePreview();
}

void ColorPicker::updatePreview() {
    m_preview.setFillColor(m_color);
}

// ==================== GUI ====================

GUI::GUI(sf::RenderWindow& window, const sf::Font& font)
    : m_window(window)
    , m_font(font)
    , m_alphaCheckbox(nullptr)
    , m_offsetXSlider(nullptr)
    , m_offsetYSlider(nullptr)
    , m_selectedMaskIndex(-1)
    , m_maskScrollOffset(0)
    , m_hasSource(false)
    , m_hasMask(false)
    , m_hasResult(false)
    , m_currentBlendMode(BlendModeType::Replace)
    , m_transparentColor(sf::Color::Magenta)
    , m_useAlpha(true)
    , m_maskOffset(0, 0)
    , m_sourceSize(0, 0)
    , m_maskSize(0, 0)
{
    initializePanels();
    initializeButtons();
    initializeModeButtons();
    initializeColorPicker();
    initializeSliders();
    updateLayout();
}

void GUI::initializePanels() {
    sf::Vector2u windowSize = m_window.getSize();
    
    // Lewy panel (przyciski główne i tryby)
    m_leftPanel.background.setSize(sf::Vector2f(250, static_cast<float>(windowSize.y)));
    m_leftPanel.background.setPosition({0, 0});
    m_leftPanel.background.setFillColor(sf::Color(40, 40, 45));
    
    // Prawy panel (biblioteka masek)
    m_rightPanel.background.setSize(sf::Vector2f(200, static_cast<float>(windowSize.y)));
    m_rightPanel.background.setPosition({static_cast<float>(windowSize.x - 200), 0});
    m_rightPanel.background.setFillColor(sf::Color(40, 40, 45));
    
    // Górny pasek
    m_topBar.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 40));
    m_topBar.setPosition({0, 0});
    m_topBar.setFillColor(sf::Color(30, 30, 35));
    
    // Dolny pasek (status)
    m_bottomBar.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 30));
    m_bottomBar.setPosition({0, static_cast<float>(windowSize.y - 30)});
    m_bottomBar.setFillColor(sf::Color(30, 30, 35));
    
    // Tekst statusu
    m_statusText.emplace(m_font, "Gotowy", 12);
    m_statusText->setFillColor(sf::Color(200, 200, 200));
    m_statusText->setPosition({260, static_cast<float>(windowSize.y - 22)});
    
    // Informacje o rozmiarach
    m_sizeInfoText.emplace(m_font, "", 11);
    m_sizeInfoText->setFillColor(sf::Color(150, 150, 150));
    m_sizeInfoText->setPosition({15, static_cast<float>(windowSize.y - 70)});
}

void GUI::initializeButtons() {
    float startY = 50;
    float buttonWidth = 220;
    float buttonHeight = 35;
    float spacing = 10;
    float x = 15;
    
    // Przyciski główne
    m_mainButtons.emplace_back(
        sf::Vector2f(x, startY),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Wczytaj obraz zrodlowy",
        m_font
    );
    
    m_mainButtons.emplace_back(
        sf::Vector2f(x, startY + buttonHeight + spacing),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Wczytaj maske",
        m_font
    );
    
    m_mainButtons.emplace_back(
        sf::Vector2f(x, startY + 2 * (buttonHeight + spacing)),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Zastosuj maske",
        m_font
    );
    m_mainButtons[2].setEnabled(false);
    
    m_mainButtons.emplace_back(
        sf::Vector2f(x, startY + 3 * (buttonHeight + spacing)),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Zapisz wynik",
        m_font
    );
    m_mainButtons[3].setEnabled(false);
    
    // Checkbox alfa
    m_mainButtons.emplace_back(
        sf::Vector2f(x, startY + 4 * (buttonHeight + spacing) + 20),
        sf::Vector2f(buttonWidth, buttonHeight),
        "Uzyj kanalu alfa [X]",
        m_font
    );
    m_mainButtons[4].setSelected(true);
}

void GUI::initializeModeButtons() {
    float startY = 300;
    float buttonWidth = 105;
    float buttonHeight = 30;
    float spacing = 5;
    float x = 15;
    
    // Tytuł sekcji
    m_leftPanel.title.emplace(m_font, "Tryb nakladania:", 13);
    m_leftPanel.title->setFillColor(sf::Color(200, 200, 200));
    m_leftPanel.title->setPosition({x, startY - 25});
    
    auto modes = BlendMode::getAllModes();
    int col = 0;
    int row = 0;
    
    for (const auto& mode : modes) {
        float posX = x + col * (buttonWidth + spacing);
        float posY = startY + row * (buttonHeight + spacing);
        
        m_modeButtons.emplace_back(
            sf::Vector2f(posX, posY),
            sf::Vector2f(buttonWidth, buttonHeight),
            BlendMode::getModeName(mode),
            m_font
        );
        
        col++;
        if (col >= 2) {
            col = 0;
            row++;
        }
    }
    
    // Pierwszy tryb domyślnie wybrany
    if (!m_modeButtons.empty()) {
        m_modeButtons[0].setSelected(true);
    }
}

void GUI::initializeColorPicker() {
    m_colorPicker = std::make_unique<ColorPicker>(sf::Vector2f(15, 470), m_font);
}

void GUI::initializeSliders() {
    // Suwaki będą inicjalizowane gdy będzie potrzebne przesunięcie maski
}

void GUI::updateLayout() {
    sf::Vector2u windowSize = m_window.getSize();
    
    m_leftPanel.background.setSize(sf::Vector2f(250, static_cast<float>(windowSize.y)));
    m_rightPanel.background.setPosition({static_cast<float>(windowSize.x - 200), 0});
    m_rightPanel.background.setSize(sf::Vector2f(200, static_cast<float>(windowSize.y)));
    
    m_topBar.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 40));
    m_bottomBar.setSize(sf::Vector2f(static_cast<float>(windowSize.x), 30));
    m_bottomBar.setPosition({0, static_cast<float>(windowSize.y - 30)});
    
    if (m_statusText) {
        m_statusText->setPosition({260, static_cast<float>(windowSize.y - 22)});
    }
    if (m_sizeInfoText) {
        m_sizeInfoText->setPosition({15, static_cast<float>(windowSize.y - 70)});
    }
}

void GUI::draw() {
    // Panele
    m_window.draw(m_leftPanel.background);
    m_window.draw(m_rightPanel.background);
    m_window.draw(m_bottomBar);
    
    // Tytuł biblioteki masek
    sf::Text libraryTitle(m_font, "Biblioteka masek:", 13);
    libraryTitle.setFillColor(sf::Color(200, 200, 200));
    libraryTitle.setPosition({m_rightPanel.background.getPosition().x + 10, 50});
    m_window.draw(libraryTitle);
    
    // Tytuł sekcji trybów
    if (m_leftPanel.title) {
        m_window.draw(*m_leftPanel.title);
    }
    
    // Przyciski główne
    for (auto& btn : m_mainButtons) {
        btn.draw(m_window);
    }
    
    // Przyciski trybów
    for (auto& btn : m_modeButtons) {
        btn.draw(m_window);
    }
    
    // Color picker
    if (m_colorPicker) {
        m_colorPicker->draw(m_window);
    }
    
    // Miniaturki masek
    float maskStartY = 80;
    float maskX = m_rightPanel.background.getPosition().x + 10;
    float maskSpacing = 75;
    
    for (size_t i = 0; i < m_maskSprites.size(); ++i) {
        float y = maskStartY + i * maskSpacing - m_maskScrollOffset;
        
        if (y > 50 && y < m_window.getSize().y - 100) {
            m_maskSprites[i].setPosition({maskX, y});
            
            // Podświetlenie wybranej maski
            if (static_cast<int>(i) == m_selectedMaskIndex) {
                sf::RectangleShape highlight;
                highlight.setPosition({maskX - 5, y - 5});
                highlight.setSize(sf::Vector2f(180, 70));
                highlight.setFillColor(sf::Color(50, 100, 150, 100));
                highlight.setOutlineThickness(2);
                highlight.setOutlineColor(sf::Color(100, 150, 200));
                m_window.draw(highlight);
            }
            
            m_window.draw(m_maskSprites[i]);
        }
    }
    
    // Status
    if (m_statusText) m_window.draw(*m_statusText);
    if (m_sizeInfoText) m_window.draw(*m_sizeInfoText);
}

void GUI::handleMouseClick(const sf::Vector2f& mousePos) {
    // Przyciski główne
    if (m_mainButtons[0].isClicked(mousePos) && m_onLoadSource) {
        m_onLoadSource();
    }
    else if (m_mainButtons[1].isClicked(mousePos) && m_onLoadMask) {
        m_onLoadMask();
    }
    else if (m_mainButtons[2].isClicked(mousePos) && m_mainButtons[2].isEnabled() && m_onApplyMask) {
        m_onApplyMask();
    }
    else if (m_mainButtons[3].isClicked(mousePos) && m_mainButtons[3].isEnabled() && m_onSaveResult) {
        m_onSaveResult();
    }
    else if (m_mainButtons[4].isClicked(mousePos)) {
        m_useAlpha = !m_useAlpha;
        m_mainButtons[4].setLabel(m_useAlpha ? "Uzyj kanalu alfa [X]" : "Uzyj kanalu alfa [ ]");
        if (m_onUseAlphaChange) {
            m_onUseAlphaChange(m_useAlpha);
        }
    }
    
    // Przyciski trybów
    auto modes = BlendMode::getAllModes();
    for (size_t i = 0; i < m_modeButtons.size() && i < modes.size(); ++i) {
        if (m_modeButtons[i].isClicked(mousePos)) {
            // Odznacz wszystkie
            for (auto& btn : m_modeButtons) {
                btn.setSelected(false);
            }
            m_modeButtons[i].setSelected(true);
            m_currentBlendMode = modes[i];
            
            if (m_onBlendModeChange) {
                m_onBlendModeChange(m_currentBlendMode);
            }
        }
    }
    
    // Wybór maski z biblioteki
    float maskStartY = 80;
    float maskX = m_rightPanel.background.getPosition().x + 10;
    float maskSpacing = 75;
    
    for (size_t i = 0; i < m_maskSprites.size(); ++i) {
        float y = maskStartY + i * maskSpacing - m_maskScrollOffset;
        sf::FloatRect maskBounds({maskX - 5, y - 5}, {180, 70});
        
        if (maskBounds.contains(mousePos)) {
            m_selectedMaskIndex = static_cast<int>(i);
            if (m_onMaskSelect) {
                m_onMaskSelect(i);
            }
            break;
        }
    }
    
    // Color picker - handle jako mouse pressed
    if (m_colorPicker) {
        if (m_colorPicker->handleEvent(mousePos, true, false)) {
            m_transparentColor = m_colorPicker->getColor();
            if (m_onTransparentColorChange) {
                m_onTransparentColorChange(m_transparentColor);
            }
        }
    }
}

void GUI::handleMouseWheel(const sf::Vector2f& mousePos, float delta) {
    if (m_rightPanel.background.getGlobalBounds().contains(mousePos)) {
        m_maskScrollOffset -= delta * 20;
        m_maskScrollOffset = std::max(0.0f, m_maskScrollOffset);
        float maxScroll = std::max(0.0f, static_cast<float>(m_maskSprites.size()) * 75 - 400);
        m_maskScrollOffset = std::min(m_maskScrollOffset, maxScroll);
    }
}

void GUI::handleResize() {
    updateLayout();
}

void GUI::update() {
    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(m_window));
    
    for (auto& btn : m_mainButtons) {
        btn.update(mousePos);
    }
    for (auto& btn : m_modeButtons) {
        btn.update(mousePos);
    }
    
    // Update color picker dragging
    if (m_colorPicker) {
        bool leftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if (m_colorPicker->handleEvent(mousePos, false, !leftPressed)) {
            m_transparentColor = m_colorPicker->getColor();
            if (m_onTransparentColorChange) {
                m_onTransparentColorChange(m_transparentColor);
            }
        }
    }
}

void GUI::setOnLoadSource(std::function<void()> callback) {
    m_onLoadSource = callback;
}

void GUI::setOnLoadMask(std::function<void()> callback) {
    m_onLoadMask = callback;
}

void GUI::setOnSaveResult(std::function<void()> callback) {
    m_onSaveResult = callback;
}

void GUI::setOnApplyMask(std::function<void()> callback) {
    m_onApplyMask = callback;
}

void GUI::setOnBlendModeChange(std::function<void(BlendModeType)> callback) {
    m_onBlendModeChange = callback;
}

void GUI::setOnTransparentColorChange(std::function<void(const sf::Color&)> callback) {
    m_onTransparentColorChange = callback;
}

void GUI::setOnUseAlphaChange(std::function<void(bool)> callback) {
    m_onUseAlphaChange = callback;
}

void GUI::setOnMaskSelect(std::function<void(size_t)> callback) {
    m_onMaskSelect = callback;
}

void GUI::setOnMaskOffsetChange(std::function<void(int, int)> callback) {
    m_onMaskOffsetChange = callback;
}

void GUI::setHasSource(bool has) {
    m_hasSource = has;
    m_mainButtons[2].setEnabled(m_hasSource && m_hasMask);
}

void GUI::setHasMask(bool has) {
    m_hasMask = has;
    m_mainButtons[2].setEnabled(m_hasSource && m_hasMask);
}

void GUI::setHasResult(bool has) {
    m_hasResult = has;
    m_mainButtons[3].setEnabled(has);
}

void GUI::setBlendMode(BlendModeType mode) {
    m_currentBlendMode = mode;
    auto modes = BlendMode::getAllModes();
    for (size_t i = 0; i < m_modeButtons.size() && i < modes.size(); ++i) {
        m_modeButtons[i].setSelected(modes[i] == mode);
    }
}

void GUI::setTransparentColor(const sf::Color& color) {
    m_transparentColor = color;
    if (m_colorPicker) {
        m_colorPicker->setColor(color);
    }
}

void GUI::setUseAlpha(bool use) {
    m_useAlpha = use;
    m_mainButtons[4].setLabel(m_useAlpha ? "Uzyj kanalu alfa [X]" : "Uzyj kanalu alfa [ ]");
}

void GUI::setMaskOffset(int x, int y) {
    m_maskOffset = sf::Vector2i(x, y);
}

void GUI::setSourceSize(const sf::Vector2u& size) {
    m_sourceSize = size;
    std::stringstream ss;
    ss << "Obraz: " << size.x << "x" << size.y;
    if (m_maskSize.x > 0) {
        ss << " | Maska: " << m_maskSize.x << "x" << m_maskSize.y;
    }
    if (m_sizeInfoText) {
        m_sizeInfoText->setString(ss.str());
    }
}

void GUI::setMaskSize(const sf::Vector2u& size) {
    m_maskSize = size;
    std::stringstream ss;
    if (m_sourceSize.x > 0) {
        ss << "Obraz: " << m_sourceSize.x << "x" << m_sourceSize.y << " | ";
    }
    ss << "Maska: " << size.x << "x" << size.y;
    if (m_sizeInfoText) {
        m_sizeInfoText->setString(ss.str());
    }
}

void GUI::setMaskThumbnails(const std::vector<std::pair<std::string, sf::Texture>>& masks) {
    m_maskTextures.clear();
    m_maskSprites.clear();
    
    for (const auto& mask : masks) {
        m_maskTextures.push_back(mask.second);
    }
    
    for (size_t i = 0; i < m_maskTextures.size(); ++i) {
        sf::Sprite sprite(m_maskTextures[i]);
        
        // Skaluj do miniaturki
        sf::Vector2u texSize = m_maskTextures[i].getSize();
        float scale = std::min(64.0f / texSize.x, 64.0f / texSize.y);
        sprite.setScale({scale, scale});
        
        m_maskSprites.push_back(sprite);
    }
}

sf::FloatRect GUI::getPreviewArea() const {
    return sf::FloatRect(
        {250, 0},  // position - Po lewym panelu
        {static_cast<float>(m_window.getSize().x - 450),   // size.x - Minus oba panele
         static_cast<float>(m_window.getSize().y - 30)}    // size.y - Minus dolny pasek
    );
}

void GUI::setStatusMessage(const std::string& message) {
    if (m_statusText) {
        m_statusText->setString(message);
    }
}

BlendModeType GUI::getCurrentBlendMode() const {
    return m_currentBlendMode;
}

sf::Color GUI::getTransparentColor() const {
    return m_transparentColor;
}

bool GUI::getUseAlpha() const {
    return m_useAlpha;
}

} // namespace MaskOverlay
