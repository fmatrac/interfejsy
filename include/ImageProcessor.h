#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include "BlendMode.h"

namespace MaskOverlay {

class ImageProcessor {
public:
    ImageProcessor();

    bool loadSourceImage(const std::string& path);

    bool loadMask(const std::string& path);

    void applyMask(BlendModeType mode, 
                   const sf::Color& transparentColor,
                   bool useAlpha = true);

    bool saveResult(const std::string& path);

    void setMaskOffset(int x, int y);

    sf::Vector2i getMaskOffset() const;

    void resetMaskOffset();

    sf::Vector2u getSourceSize() const;

    sf::Vector2u getMaskSize() const;

    bool hasSourceImage() const;

    bool hasMask() const;

    const sf::Texture& getSourceTexture() const;

    const sf::Texture& getMaskTexture() const;

    const sf::Texture& getResultTexture() const;

    bool hasResult() const;

    const sf::Image& getResultImage() const;

private:
    sf::Image m_sourceImage;
    sf::Image m_maskImage;
    sf::Image m_resultImage;
    
    sf::Texture m_sourceTexture;
    sf::Texture m_maskTexture;
    sf::Texture m_resultTexture;

    sf::Vector2i m_maskOffset;

    bool m_hasSource;
    bool m_hasMask;
    bool m_hasResult;

    void updateSourceTexture();
    void updateMaskTexture();
    void updateResultTexture();
};

}
