#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include "BlendMode.h"

namespace MaskOverlay {

/**
 * Klasa do przetwarzania obrazów i nakładania masek
 */
class ImageProcessor {
public:
    ImageProcessor();

    /**
     * Wczytuje obraz źródłowy
     * @param path Ścieżka do pliku BMP
     * @return true jeśli wczytano pomyślnie
     */
    bool loadSourceImage(const std::string& path);

    /**
     * Wczytuje maskę
     * @param path Ścieżka do pliku BMP
     * @return true jeśli wczytano pomyślnie
     */
    bool loadMask(const std::string& path);

    /**
     * Nakłada maskę na obraz źródłowy
     * @param mode Tryb nakładania
     * @param transparentColor Kolor przezroczysty
     * @param useAlpha Czy używać kanału alfa
     */
    void applyMask(BlendModeType mode, 
                   const sf::Color& transparentColor,
                   bool useAlpha = true);

    /**
     * Zapisuje wynik do pliku BMP
     * @param path Ścieżka do pliku wyjściowego
     * @return true jeśli zapisano pomyślnie
     */
    bool saveResult(const std::string& path);

    /**
     * Ustawia przesunięcie maski (dla masek większych od obrazu)
     */
    void setMaskOffset(int x, int y);

    /**
     * Pobiera przesunięcie maski
     */
    sf::Vector2i getMaskOffset() const;

    /**
     * Resetuje przesunięcie maski
     */
    void resetMaskOffset();

    /**
     * Pobiera rozmiar obrazu źródłowego
     */
    sf::Vector2u getSourceSize() const;

    /**
     * Pobiera rozmiar maski
     */
    sf::Vector2u getMaskSize() const;

    /**
     * Sprawdza czy obraz źródłowy jest wczytany
     */
    bool hasSourceImage() const;

    /**
     * Sprawdza czy maska jest wczytana
     */
    bool hasMask() const;

    /**
     * Pobiera teksturę obrazu źródłowego (do wyświetlania)
     */
    const sf::Texture& getSourceTexture() const;

    /**
     * Pobiera teksturę maski (do wyświetlania)
     */
    const sf::Texture& getMaskTexture() const;

    /**
     * Pobiera teksturę wyniku (do wyświetlania)
     */
    const sf::Texture& getResultTexture() const;

    /**
     * Sprawdza czy wynik jest dostępny
     */
    bool hasResult() const;

    /**
     * Pobiera obraz wynikowy
     */
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

} // namespace MaskOverlay
