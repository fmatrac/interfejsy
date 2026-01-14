#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <cstdint>

namespace MaskOverlay {

/**
 * Tryby nakładania maski na obraz
 */
enum class BlendModeType {
    Replace,      // Zamiana kolorów
    Add,          // Sumowanie kolorów
    Multiply,     // Mnożenie kolorów
    Screen,       // Rozjaśnianie (rozszerzone)
    Overlay,      // Nakładka (rozszerzone)
    Difference,   // Różnica (rozszerzone)
    SoftLight,    // Miękkie światło (rozszerzone)
    HardLight     // Twarde światło (rozszerzone)
};

/**
 * Klasa obsługująca różne tryby nakładania kolorów
 */
class BlendMode {
public:
    /**
     * Nakłada kolor maski na kolor źródłowy
     * @param source Kolor piksela źródłowego
     * @param mask Kolor piksela maski
     * @param mode Tryb nakładania
     * @param transparentColor Kolor traktowany jako przezroczysty
     * @param useAlpha Czy używać kanału alfa
     * @return Wynikowy kolor
     */
    static sf::Color blend(const sf::Color& source, 
                          const sf::Color& mask,
                          BlendModeType mode,
                          const sf::Color& transparentColor,
                          bool useAlpha = true);

    /**
     * Sprawdza czy kolor jest przezroczysty
     */
    static bool isTransparent(const sf::Color& color, 
                             const sf::Color& transparentColor,
                             int tolerance = 10);

    /**
     * Zwraca nazwę trybu nakładania
     */
    static std::string getModeName(BlendModeType mode);

    /**
     * Zwraca wszystkie dostępne tryby
     */
    static std::vector<BlendModeType> getAllModes();

private:
    // Funkcje pomocnicze do obliczeń
    static std::uint8_t clamp(int value);
    static sf::Color blendReplace(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendAdd(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendMultiply(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendScreen(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendOverlay(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendDifference(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendSoftLight(const sf::Color& source, const sf::Color& mask);
    static sf::Color blendHardLight(const sf::Color& source, const sf::Color& mask);
    
    // Interpolacja z uwzględnieniem alfy
    static sf::Color applyAlpha(const sf::Color& source, 
                               const sf::Color& blended, 
                               std::uint8_t alpha);
};

} // namespace MaskOverlay
