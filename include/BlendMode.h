#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <cstdint>

namespace MaskOverlay {

enum class BlendModeType {
    Replace,
    Add,
    Multiply,
    Screen,
    Overlay,
    Difference,
    SoftLight,
    HardLight
};

class BlendMode {
public:
    static sf::Color blend(const sf::Color& source, 
                          const sf::Color& mask,
                          BlendModeType mode,
                          const sf::Color& transparentColor,
                          bool useAlpha = true);

    static bool isTransparent(const sf::Color& color, 
                             const sf::Color& transparentColor,
                             int tolerance = 10);

    static std::string getModeName(BlendModeType mode);

    static std::vector<BlendModeType> getAllModes();

private:
    static std::uint8_t clamp(int value);
    static sf::Color blendReplace(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendAdd(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendMultiply(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendScreen(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendOverlay(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendDifference(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendSoftLight(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendHardLight(const sf::Color& source, const sf::Color& mask);
    
    static sf::Color applyAlpha(const sf::Color& source, 
                               const sf::Color& blended, 
                               std::uint8_t alpha);
};

}
