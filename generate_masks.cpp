// Generator przykładowych masek
// Kompilacja: g++ -std=c++17 generate_masks.cpp -o generate_masks -lsfml-graphics
// Uruchomienie: ./generate_masks

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

const sf::Color TRANSPARENT_COLOR(255, 0, 255); // Magenta

// Maska winieta (owalna ramka)
void createVignetteMask(const std::string& filename, unsigned int width, unsigned int height) {
    sf::Image img;
    img.create(width, height, TRANSPARENT_COLOR);
    
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float radiusX = width * 0.4f;
    float radiusY = height * 0.4f;
    
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            float dx = (x - centerX) / radiusX;
            float dy = (y - centerY) / radiusY;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > 1.0f) {
                // Gradientowe przyciemnienie
                float alpha = std::min(1.0f, (dist - 1.0f) * 2);
                sf::Uint8 darkness = static_cast<sf::Uint8>(alpha * 200);
                img.setPixel(x, y, sf::Color(darkness, darkness, darkness, 255));
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska kołowa (otwór w środku)
void createCircleMask(const std::string& filename, unsigned int size) {
    sf::Image img;
    img.create(size, size, sf::Color::Black);
    
    float center = size / 2.0f;
    float radius = size * 0.35f;
    
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float dx = x - center;
            float dy = y - center;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < radius) {
                img.setPixel(x, y, TRANSPARENT_COLOR);
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska ramki
void createFrameMask(const std::string& filename, unsigned int width, unsigned int height, unsigned int border) {
    sf::Image img;
    img.create(width, height, TRANSPARENT_COLOR);
    
    // Ramka
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            if (x < border || x >= width - border || y < border || y >= height - border) {
                // Gradient ramki
                sf::Uint8 shade = 50 + (y % 50);
                img.setPixel(x, y, sf::Color(shade, shade, shade + 20, 255));
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska siatki (grid)
void createGridMask(const std::string& filename, unsigned int size, unsigned int cellSize) {
    sf::Image img;
    img.create(size, size, TRANSPARENT_COLOR);
    
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            if (x % cellSize == 0 || y % cellSize == 0) {
                img.setPixel(x, y, sf::Color(100, 100, 100, 200));
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska skali (dla pomiarów)
void createScaleMask(const std::string& filename, unsigned int width, unsigned int height) {
    sf::Image img;
    img.create(width, height, TRANSPARENT_COLOR);
    
    // Skala pozioma na dole
    unsigned int scaleY = height - 30;
    unsigned int scaleHeight = 20;
    
    for (unsigned int x = 0; x < width; ++x) {
        unsigned int lineHeight = (x % 50 == 0) ? scaleHeight : (x % 10 == 0) ? scaleHeight / 2 : 0;
        
        if (lineHeight > 0) {
            for (unsigned int y = scaleY; y < scaleY + lineHeight; ++y) {
                img.setPixel(x, y, sf::Color::Black);
            }
        }
    }
    
    // Linia podstawy
    for (unsigned int x = 0; x < width; ++x) {
        img.setPixel(x, scaleY, sf::Color::Black);
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska gwiazdki
void createStarMask(const std::string& filename, unsigned int size) {
    sf::Image img;
    img.create(size, size, sf::Color(80, 60, 40, 255)); // Brązowe tło
    
    float centerX = size / 2.0f;
    float centerY = size / 2.0f;
    int points = 5;
    float outerRadius = size * 0.4f;
    float innerRadius = size * 0.18f;
    
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float dx = x - centerX;
            float dy = y - centerY;
            float angle = std::atan2(dy, dx);
            float dist = std::sqrt(dx * dx + dy * dy);
            
            // Oblicz promień gwiazdki dla tego kąta
            float normalizedAngle = angle + M_PI;
            float pointAngle = normalizedAngle / (2 * M_PI) * points * 2;
            float fraction = pointAngle - std::floor(pointAngle);
            
            float radius;
            if (fraction < 0.5f) {
                radius = innerRadius + (outerRadius - innerRadius) * (fraction * 2);
            } else {
                radius = outerRadius - (outerRadius - innerRadius) * ((fraction - 0.5f) * 2);
            }
            
            if (dist < radius) {
                img.setPixel(x, y, TRANSPARENT_COLOR);
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska serca
void createHeartMask(const std::string& filename, unsigned int size) {
    sf::Image img;
    img.create(size, size, sf::Color(200, 50, 80, 255)); // Czerwone tło
    
    float scale = size / 20.0f;
    float centerX = size / 2.0f;
    float centerY = size / 2.0f + size * 0.1f;
    
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            float px = (x - centerX) / scale;
            float py = -(y - centerY) / scale;
            
            // Równanie serca: (x^2 + y^2 - 1)^3 - x^2 * y^3 = 0
            float eq = std::pow(px * px + py * py - 6, 3) - px * px * py * py * py;
            
            if (eq < 0) {
                img.setPixel(x, y, TRANSPARENT_COLOR);
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

// Maska diagonalnych pasków
void createDiagonalMask(const std::string& filename, unsigned int size, unsigned int stripeWidth) {
    sf::Image img;
    img.create(size, size, TRANSPARENT_COLOR);
    
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            int diagonal = (x + y) % (stripeWidth * 2);
            if (diagonal < static_cast<int>(stripeWidth)) {
                img.setPixel(x, y, sf::Color(50, 50, 150, 180));
            }
        }
    }
    
    img.saveToFile(filename);
    std::cout << "Utworzono: " << filename << std::endl;
}

int main() {
    std::cout << "Generator przykładowych masek" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // Generuj maski
    createVignetteMask("masks/vignette.png", 800, 600);
    createCircleMask("masks/circle.png", 500);
    createFrameMask("masks/frame.png", 800, 600, 40);
    createGridMask("masks/grid.png", 500, 50);
    createScaleMask("masks/scale.png", 800, 100);
    createStarMask("masks/star.png", 400);
    createHeartMask("masks/heart.png", 400);
    createDiagonalMask("masks/diagonal.png", 500, 20);
    
    std::cout << std::endl;
    std::cout << "Gotowe! Maski zapisano w katalogu 'masks'" << std::endl;
    
    return 0;
}
