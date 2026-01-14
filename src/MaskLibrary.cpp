#include "MaskLibrary.h"
#include <iostream>
#include <algorithm>

namespace MaskOverlay {

MaskLibrary::MaskLibrary()
    : m_thumbnailSize(64)
{
}

void MaskLibrary::scanDirectory(const std::string& directory) {
    try {
        if (!std::filesystem::exists(directory)) {
            std::cerr << "Katalog nie istnieje: " << directory << std::endl;
            return;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && isImageFile(entry.path().string())) {
                addMask(entry.path().string());
            }
        }
        
        // Sortuj maski alfabetycznie
        std::sort(m_masks.begin(), m_masks.end(), 
            [](const MaskEntry& a, const MaskEntry& b) {
                return a.name < b.name;
            });
        
        std::cout << "Znaleziono " << m_masks.size() << " masek w katalogu: " << directory << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd podczas skanowania katalogu: " << e.what() << std::endl;
    }
}

void MaskLibrary::addMask(const std::string& path) {
    if (!isImageFile(path)) {
        return;
    }
    
    MaskEntry entry;
    entry.path = path;
    entry.name = std::filesystem::path(path).stem().string();
    entry.loaded = false;
    
    // Załaduj miniaturkę
    loadThumbnail(entry);
    
    m_masks.push_back(std::move(entry));
}

void MaskLibrary::loadThumbnail(MaskEntry& entry) {
    sf::Image image;
    if (!image.loadFromFile(entry.path)) {
        std::cerr << "Nie można wczytać miniaturki: " << entry.path << std::endl;
        return;
    }
    
    // Skaluj do rozmiaru miniaturki
    sf::Vector2u originalSize = image.getSize();
    
    // Oblicz współczynnik skalowania (zachowaj proporcje)
    float scaleX = static_cast<float>(m_thumbnailSize) / originalSize.x;
    float scaleY = static_cast<float>(m_thumbnailSize) / originalSize.y;
    float scale = std::min(scaleX, scaleY);
    
    unsigned int newWidth = static_cast<unsigned int>(originalSize.x * scale);
    unsigned int newHeight = static_cast<unsigned int>(originalSize.y * scale);
    
    // Utwórz skalowany obraz (proste skalowanie - można użyć lepszego algorytmu)
    sf::Image thumbnail;
    thumbnail.resize(sf::Vector2u(newWidth, newHeight));
    
    for (unsigned int y = 0; y < newHeight; ++y) {
        for (unsigned int x = 0; x < newWidth; ++x) {
            unsigned int srcX = static_cast<unsigned int>(x / scale);
            unsigned int srcY = static_cast<unsigned int>(y / scale);
            srcX = std::min(srcX, originalSize.x - 1);
            srcY = std::min(srcY, originalSize.y - 1);
            thumbnail.setPixel(sf::Vector2u(x, y), image.getPixel(sf::Vector2u(srcX, srcY)));
        }
    }
    
    (void)entry.thumbnail.loadFromImage(thumbnail);
    entry.loaded = true;
}

size_t MaskLibrary::getCount() const {
    return m_masks.size();
}

const MaskEntry& MaskLibrary::getMask(size_t index) const {
    return m_masks.at(index);
}

std::string MaskLibrary::getMaskPath(size_t index) const {
    if (index < m_masks.size()) {
        return m_masks[index].path;
    }
    return "";
}

void MaskLibrary::clear() {
    m_masks.clear();
}

bool MaskLibrary::isEmpty() const {
    return m_masks.empty();
}

const std::vector<MaskEntry>& MaskLibrary::getAllMasks() const {
    return m_masks;
}

void MaskLibrary::setThumbnailSize(unsigned int size) {
    m_thumbnailSize = size;
}

unsigned int MaskLibrary::getThumbnailSize() const {
    return m_thumbnailSize;
}

bool MaskLibrary::isImageFile(const std::string& path) const {
    std::filesystem::path filePath(path);
    std::string ext = filePath.extension().string();
    
    // Konwertuj na małe litery
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".bmp" || ext == ".png" || ext == ".jpg" || ext == ".jpeg");
}

} // namespace MaskOverlay
