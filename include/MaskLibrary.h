#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>

namespace MaskOverlay {

struct MaskEntry {
    std::string name;
    std::string path;
    sf::Texture thumbnail;
    bool loaded = false;
};

class MaskLibrary {
public:
    MaskLibrary();

    void scanDirectory(const std::string& directory);

    void addMask(const std::string& path);

    size_t getCount() const;

    const MaskEntry& getMask(size_t index) const;

    std::string getMaskPath(size_t index) const;

    void clear();

    bool isEmpty() const;

    const std::vector<MaskEntry>& getAllMasks() const;

    void setThumbnailSize(unsigned int size);

    unsigned int getThumbnailSize() const;

private:
    std::vector<MaskEntry> m_masks;
    unsigned int m_thumbnailSize;

    void loadThumbnail(MaskEntry& entry);
    bool isImageFile(const std::string& path) const;
};

}
