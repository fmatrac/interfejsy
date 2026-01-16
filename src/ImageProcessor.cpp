#include "ImageProcessor.h"
#include <iostream>

namespace MaskOverlay {

ImageProcessor::ImageProcessor()
    : m_maskOffset(0, 0)
    , m_hasSource(false)
    , m_hasMask(false)
    , m_hasResult(false)
{
}

bool ImageProcessor::loadSourceImage(const std::string& path) {
    if (!m_sourceImage.loadFromFile(path)) {
        std::cerr << "Nie można wczytać obrazu źródłowego: " << path << std::endl;
        return false;
    }
    
    m_hasSource = true;
    m_hasResult = false;
    updateSourceTexture();
    
    std::cout << "Wczytano obraz źródłowy: " << path 
              << " (" << m_sourceImage.getSize().x << "x" << m_sourceImage.getSize().y << ")" << std::endl;
    
    return true;
}

bool ImageProcessor::loadMask(const std::string& path) {
    if (!m_maskImage.loadFromFile(path)) {
        std::cerr << "Nie można wczytać maski: " << path << std::endl;
        return false;
    }
    
    m_hasMask = true;
    m_hasResult = false;
    updateMaskTexture();
    

    resetMaskOffset();
    
    std::cout << "Wczytano maskę: " << path 
              << " (" << m_maskImage.getSize().x << "x" << m_maskImage.getSize().y << ")" << std::endl;
    
    return true;
}

void ImageProcessor::applyMask(BlendModeType mode, 
                               const sf::Color& transparentColor,
                               bool useAlpha) {
    if (!m_hasSource || !m_hasMask) {
        std::cerr << "Brak obrazu źródłowego lub maski!" << std::endl;
        return;
    }
    
    sf::Vector2u sourceSize = m_sourceImage.getSize();
    sf::Vector2u maskSize = m_maskImage.getSize();
    

    m_resultImage.resize(sf::Vector2u(sourceSize.x, sourceSize.y), sf::Color::Transparent);
    

    for (unsigned int y = 0; y < sourceSize.y; ++y) {
        for (unsigned int x = 0; x < sourceSize.x; ++x) {
            sf::Color sourcePixel = m_sourceImage.getPixel(sf::Vector2u(x, y));
            

            int maskX = static_cast<int>(x) + m_maskOffset.x;
            int maskY = static_cast<int>(y) + m_maskOffset.y;
            

            if (maskX >= 0 && maskX < static_cast<int>(maskSize.x) &&
                maskY >= 0 && maskY < static_cast<int>(maskSize.y)) {
                
                sf::Color maskPixel = m_maskImage.getPixel(sf::Vector2u(
                    static_cast<unsigned int>(maskX), 
                    static_cast<unsigned int>(maskY)
                ));
                

                sf::Color resultPixel = BlendMode::blend(
                    sourcePixel, maskPixel, mode, transparentColor, useAlpha
                );
                
                m_resultImage.setPixel(sf::Vector2u(x, y), resultPixel);
            } else {

                m_resultImage.setPixel(sf::Vector2u(x, y), sourcePixel);
            }
        }
    }
    
    m_hasResult = true;
    updateResultTexture();
    
    std::cout << "Zastosowano maskę w trybie: " << BlendMode::getModeName(mode) << std::endl;
}

bool ImageProcessor::saveResult(const std::string& path) {
    if (!m_hasResult) {
        std::cerr << "Brak wyniku do zapisania!" << std::endl;
        return false;
    }
    
    if (!m_resultImage.saveToFile(path)) {
        std::cerr << "Nie można zapisać wyniku do: " << path << std::endl;
        return false;
    }
    
    std::cout << "Zapisano wynik do: " << path << std::endl;
    return true;
}

void ImageProcessor::setMaskOffset(int x, int y) {
    m_maskOffset.x = x;
    m_maskOffset.y = y;
}

sf::Vector2i ImageProcessor::getMaskOffset() const {
    return m_maskOffset;
}

void ImageProcessor::resetMaskOffset() {
    m_maskOffset = sf::Vector2i(0, 0);
}

sf::Vector2u ImageProcessor::getSourceSize() const {
    return m_hasSource ? m_sourceImage.getSize() : sf::Vector2u(0, 0);
}

sf::Vector2u ImageProcessor::getMaskSize() const {
    return m_hasMask ? m_maskImage.getSize() : sf::Vector2u(0, 0);
}

bool ImageProcessor::hasSourceImage() const {
    return m_hasSource;
}

bool ImageProcessor::hasMask() const {
    return m_hasMask;
}

const sf::Texture& ImageProcessor::getSourceTexture() const {
    return m_sourceTexture;
}

const sf::Texture& ImageProcessor::getMaskTexture() const {
    return m_maskTexture;
}

const sf::Texture& ImageProcessor::getResultTexture() const {
    return m_resultTexture;
}

bool ImageProcessor::hasResult() const {
    return m_hasResult;
}

const sf::Image& ImageProcessor::getResultImage() const {
    return m_resultImage;
}

void ImageProcessor::updateSourceTexture() {
    if (m_hasSource) {
        (void)m_sourceTexture.loadFromImage(m_sourceImage);
        m_sourceTexture.setSmooth(true);
    }
}

void ImageProcessor::updateMaskTexture() {
    if (m_hasMask) {
        (void)m_maskTexture.loadFromImage(m_maskImage);
        m_maskTexture.setSmooth(true);
    }
}

void ImageProcessor::updateResultTexture() {
    if (m_hasResult) {
        (void)m_resultTexture.loadFromImage(m_resultImage);
        m_resultTexture.setSmooth(true);
    }
}

}
