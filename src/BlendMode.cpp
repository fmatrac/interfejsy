#include "BlendMode.h"
#include <algorithm>
#include <cmath>

namespace MaskOverlay {

std::uint8_t BlendMode::clamp(int value) {
    return static_cast<std::uint8_t>(std::max(0, std::min(255, value)));
}

bool BlendMode::isTransparent(const sf::Color& color, 
                              const sf::Color& transparentColor,
                              int tolerance) {
    if (color.a == 0) {
        return true;
    }
    
    int dr = std::abs(static_cast<int>(color.r) - static_cast<int>(transparentColor.r));
    int dg = std::abs(static_cast<int>(color.g) - static_cast<int>(transparentColor.g));
    int db = std::abs(static_cast<int>(color.b) - static_cast<int>(transparentColor.b));
    
    return (dr <= tolerance && dg <= tolerance && db <= tolerance);
}

sf::Color BlendMode::blend(const sf::Color& source, 
                          const sf::Color& mask,
                          BlendModeType mode,
                          const sf::Color& transparentColor,
                          bool useAlpha) {
    if (isTransparent(mask, transparentColor)) {
        return source;
    }
    
    sf::Color blended;
    
    switch (mode) {
        case BlendModeType::Replace:
            blended = blendReplace(source, mask);
            break;
        case BlendModeType::Add:
            blended = blendAdd(source, mask);
            break;
        case BlendModeType::Multiply:
            blended = blendMultiply(source, mask);
            break;
        case BlendModeType::Screen:
            blended = blendScreen(source, mask);
            break;
        case BlendModeType::Overlay:
            blended = blendOverlay(source, mask);
            break;
        case BlendModeType::Difference:
            blended = blendDifference(source, mask);
            break;
        case BlendModeType::SoftLight:
            blended = blendSoftLight(source, mask);
            break;
        case BlendModeType::HardLight:
            blended = blendHardLight(source, mask);
            break;
        default:
            blended = blendReplace(source, mask);
    }
    
    if (useAlpha && mask.a < 255) {
        return applyAlpha(source, blended, mask.a);
    }
    
    return blended;
}

sf::Color BlendMode::applyAlpha(const sf::Color& source, 
                               const sf::Color& blended, 
                               std::uint8_t alpha) {
    float a = alpha / 255.0f;
    return sf::Color(
        clamp(static_cast<int>(source.r * (1 - a) + blended.r * a)),
        clamp(static_cast<int>(source.g * (1 - a) + blended.g * a)),
        clamp(static_cast<int>(source.b * (1 - a) + blended.b * a)),
        255
    );
}

sf::Color BlendMode::blendReplace(const sf::Color& source, const sf::Color& mask) {
    return sf::Color(mask.r, mask.g, mask.b, 255);
}

sf::Color BlendMode::blendAdd(const sf::Color& source, const sf::Color& mask) {
    return sf::Color(
        clamp(source.r + mask.r),
        clamp(source.g + mask.g),
        clamp(source.b + mask.b),
        255
    );
}

sf::Color BlendMode::blendMultiply(const sf::Color& source, const sf::Color& mask) {
    return sf::Color(
        clamp((source.r * mask.r) / 255),
        clamp((source.g * mask.g) / 255),
        clamp((source.b * mask.b) / 255),
        255
    );
}

sf::Color BlendMode::blendScreen(const sf::Color& source, const sf::Color& mask) {
    return sf::Color(
        clamp(255 - ((255 - source.r) * (255 - mask.r)) / 255),
        clamp(255 - ((255 - source.g) * (255 - mask.g)) / 255),
        clamp(255 - ((255 - source.b) * (255 - mask.b)) / 255),
        255
    );
}

sf::Color BlendMode::blendOverlay(const sf::Color& source, const sf::Color& mask) {
    auto overlayChannel = [](int base, int blend) -> int {
        if (base < 128) {
            return (2 * base * blend) / 255;
        } else {
            return 255 - (2 * (255 - base) * (255 - blend)) / 255;
        }
    };
    
    return sf::Color(
        clamp(overlayChannel(source.r, mask.r)),
        clamp(overlayChannel(source.g, mask.g)),
        clamp(overlayChannel(source.b, mask.b)),
        255
    );
}

sf::Color BlendMode::blendDifference(const sf::Color& source, const sf::Color& mask) {
    return sf::Color(
        clamp(std::abs(source.r - mask.r)),
        clamp(std::abs(source.g - mask.g)),
        clamp(std::abs(source.b - mask.b)),
        255
    );
}

sf::Color BlendMode::blendSoftLight(const sf::Color& source, const sf::Color& mask) {
    auto softLightChannel = [](int base, int blend) -> int {
        float b = base / 255.0f;
        float l = blend / 255.0f;
        float result;
        
        if (l < 0.5f) {
            result = b - (1 - 2 * l) * b * (1 - b);
        } else {
            float d = (b <= 0.25f) ? ((16 * b - 12) * b + 4) * b : std::sqrt(b);
            result = b + (2 * l - 1) * (d - b);
        }
        
        return static_cast<int>(result * 255);
    };
    
    return sf::Color(
        clamp(softLightChannel(source.r, mask.r)),
        clamp(softLightChannel(source.g, mask.g)),
        clamp(softLightChannel(source.b, mask.b)),
        255
    );
}

sf::Color BlendMode::blendHardLight(const sf::Color& source, const sf::Color& mask) {

    auto hardLightChannel = [](int base, int blend) -> int {
        if (blend < 128) {
            return (2 * base * blend) / 255;
        } else {
            return 255 - (2 * (255 - base) * (255 - blend)) / 255;
        }
    };
    
    return sf::Color(
        clamp(hardLightChannel(source.r, mask.r)),
        clamp(hardLightChannel(source.g, mask.g)),
        clamp(hardLightChannel(source.b, mask.b)),
        255
    );
}

std::string BlendMode::getModeName(BlendModeType mode) {
    switch (mode) {
        case BlendModeType::Replace:    return "Zamiana";
        case BlendModeType::Add:        return "Sumowanie";
        case BlendModeType::Multiply:   return "Mnozenie";
        case BlendModeType::Screen:     return "Screen";
        case BlendModeType::Overlay:    return "Nakladka";
        case BlendModeType::Difference: return "Roznica";
        case BlendModeType::SoftLight:  return "Miekkie swiatlo";
        case BlendModeType::HardLight:  return "Twarde swiatlo";
        default:                        return "Nieznany";
    }
}

std::vector<BlendModeType> BlendMode::getAllModes() {
    return {
        BlendModeType::Replace,
        BlendModeType::Add,
        BlendModeType::Multiply,
        BlendModeType::Screen,
        BlendModeType::Overlay,
        BlendModeType::Difference,
        BlendModeType::SoftLight,
        BlendModeType::HardLight
    };
}

}
