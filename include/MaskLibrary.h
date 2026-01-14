#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>

namespace MaskOverlay {

/**
 * Struktura reprezentująca maskę w bibliotece
 */
struct MaskEntry {
    std::string name;
    std::string path;
    sf::Texture thumbnail;
    bool loaded = false;
};

/**
 * Klasa zarządzająca biblioteką masek
 */
class MaskLibrary {
public:
    MaskLibrary();

    /**
     * Skanuje katalog w poszukiwaniu masek (pliki BMP/PNG)
     * @param directory Ścieżka do katalogu z maskami
     */
    void scanDirectory(const std::string& directory);

    /**
     * Dodaje maskę do biblioteki
     * @param path Ścieżka do pliku maski
     */
    void addMask(const std::string& path);

    /**
     * Pobiera liczbę masek w bibliotece
     */
    size_t getCount() const;

    /**
     * Pobiera wpis maski o podanym indeksie
     */
    const MaskEntry& getMask(size_t index) const;

    /**
     * Pobiera ścieżkę do maski o podanym indeksie
     */
    std::string getMaskPath(size_t index) const;

    /**
     * Czyści bibliotekę
     */
    void clear();

    /**
     * Sprawdza czy biblioteka jest pusta
     */
    bool isEmpty() const;

    /**
     * Pobiera wszystkie maski
     */
    const std::vector<MaskEntry>& getAllMasks() const;

    /**
     * Ustawia rozmiar miniaturek
     */
    void setThumbnailSize(unsigned int size);

    /**
     * Pobiera rozmiar miniaturek
     */
    unsigned int getThumbnailSize() const;

private:
    std::vector<MaskEntry> m_masks;
    unsigned int m_thumbnailSize;

    void loadThumbnail(MaskEntry& entry);
    bool isImageFile(const std::string& path) const;
};

} // namespace MaskOverlay
