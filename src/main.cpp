#include "Application.h"
#include <iostream>

int main() {
    std::cout << "=== Nakładanie masek ===" << std::endl;
    std::cout << "Projekt - Interfejsy użytkownika i biblioteki graficzne" << std::endl;
    std::cout << std::endl;
    
    MaskOverlay::Application app;
    return app.run();
}
