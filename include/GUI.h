#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include "BlendMode.h"

namespace MaskOverlay {

class Button {
public:
    Button(const sf::Vector2f& position, const sf::Vector2f& size, 
           const std::string& label, const sf::Font& font);

    void draw(sf::RenderWindow& window);
    bool isClicked(const sf::Vector2f& mousePos) const;
    bool contains(const sf::Vector2f& point) const;
    
    void setSelected(bool selected);
    bool isSelected() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;

    void setLabel(const std::string& label);
    std::string getLabel() const;

    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);

    void setColors(const sf::Color& normal, const sf::Color& hover, 
                  const sf::Color& selected, const sf::Color& disabled);

    void update(const sf::Vector2f& mousePos);

private:
    sf::RectangleShape m_shape;
    std::optional<sf::Text> m_text;
    const sf::Font* m_font;
    bool m_selected;
    bool m_enabled;
    bool m_hovered;
    
    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_selectedColor;
    sf::Color m_disabledColor;
};

class Slider {
public:
    Slider(const sf::Vector2f& position, float width, 
           float minValue, float maxValue, const sf::Font& font,
           const std::string& label = "");

    void draw(sf::RenderWindow& window);
    bool handleEvent(const sf::Vector2f& mousePos, bool mousePressed, bool mouseReleased);
    
    float getValue() const;
    void setValue(float value);

    void setLabel(const std::string& label);

private:
    sf::RectangleShape m_track;
    sf::RectangleShape m_thumb;
    std::optional<sf::Text> m_label;
    std::optional<sf::Text> m_valueText;
    const sf::Font* m_font;
    
    float m_minValue;
    float m_maxValue;
    float m_value;
    bool m_dragging;
    sf::Vector2f m_position;
    float m_width;
    
    void updateThumbPosition();
};

class ColorPicker {
public:
    ColorPicker(const sf::Vector2f& position, const sf::Font& font);

    void draw(sf::RenderWindow& window);
    bool handleEvent(const sf::Vector2f& mousePos, bool mousePressed, bool mouseReleased);
    
    sf::Color getColor() const;
    void setColor(const sf::Color& color);

private:
    sf::Vector2f m_position;
    sf::RectangleShape m_preview;
    std::optional<sf::Text> m_label;
    const sf::Font* m_font;
    std::unique_ptr<Slider> m_redSlider;
    std::unique_ptr<Slider> m_greenSlider;
    std::unique_ptr<Slider> m_blueSlider;
    std::unique_ptr<Slider> m_alphaSlider;
    sf::Color m_color;
    
    void updatePreview();
};

struct GUIPanel {
    sf::RectangleShape background;
    std::vector<Button> buttons;
    std::optional<sf::Text> title;
};

class GUI {
public:
    GUI(sf::RenderWindow& window, const sf::Font& font);

    void draw();
    void handleMouseClick(const sf::Vector2f& mousePos);
    void handleMouseWheel(const sf::Vector2f& mousePos, float delta);
    void handleResize();
    void update();

    void setOnLoadSource(std::function<void()> callback);
    void setOnLoadMask(std::function<void()> callback);
    void setOnSaveResult(std::function<void()> callback);
    void setOnApplyMask(std::function<void()> callback);
    void setOnBlendModeChange(std::function<void(BlendModeType)> callback);
    void setOnTransparentColorChange(std::function<void(const sf::Color&)> callback);
    void setOnUseAlphaChange(std::function<void(bool)> callback);
    void setOnMaskSelect(std::function<void(size_t)> callback);
    void setOnMaskOffsetChange(std::function<void(int, int)> callback);

    void setHasSource(bool has);
    void setHasMask(bool has);
    void setHasResult(bool has);
    void setBlendMode(BlendModeType mode);
    void setTransparentColor(const sf::Color& color);
    void setUseAlpha(bool use);
    void setMaskOffset(int x, int y);
    void setSourceSize(const sf::Vector2u& size);
    void setMaskSize(const sf::Vector2u& size);

    void setMaskThumbnails(const std::vector<std::pair<std::string, sf::Texture>>& masks);

    sf::FloatRect getPreviewArea() const;

    void setStatusMessage(const std::string& message);

    BlendModeType getCurrentBlendMode() const;

    sf::Color getTransparentColor() const;

    bool getUseAlpha() const;

private:
    sf::RenderWindow& m_window;
    const sf::Font& m_font;

    GUIPanel m_leftPanel;
    GUIPanel m_rightPanel;
    sf::RectangleShape m_topBar;
    sf::RectangleShape m_bottomBar;

    std::vector<Button> m_mainButtons;
    
    std::vector<Button> m_modeButtons;
    
    std::unique_ptr<ColorPicker> m_colorPicker;

    Button* m_alphaCheckbox;

    Slider* m_offsetXSlider;
    Slider* m_offsetYSlider;

    std::vector<sf::Sprite> m_maskSprites;
    std::vector<sf::Texture> m_maskTextures;
    int m_selectedMaskIndex;
    float m_maskScrollOffset;

    std::optional<sf::Text> m_statusText;
    std::optional<sf::Text> m_sizeInfoText;

    std::function<void()> m_onLoadSource;
    std::function<void()> m_onLoadMask;
    std::function<void()> m_onSaveResult;
    std::function<void()> m_onApplyMask;
    std::function<void(BlendModeType)> m_onBlendModeChange;
    std::function<void(const sf::Color&)> m_onTransparentColorChange;
    std::function<void(bool)> m_onUseAlphaChange;
    std::function<void(size_t)> m_onMaskSelect;
    std::function<void(int, int)> m_onMaskOffsetChange;

    bool m_hasSource;
    bool m_hasMask;
    bool m_hasResult;
    BlendModeType m_currentBlendMode;
    sf::Color m_transparentColor;
    bool m_useAlpha;
    sf::Vector2i m_maskOffset;
    sf::Vector2u m_sourceSize;
    sf::Vector2u m_maskSize;

    void initializePanels();
    void initializeButtons();
    void initializeModeButtons();
    void initializeColorPicker();
    void initializeSliders();
    void updateLayout();
};

}
