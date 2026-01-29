# Dokumentacja Techniczna

## Architektura

Aplikacja wykorzystuje wzorzec MVC z podziałem na:
- **Model** - ImageProcessor, MaskLibrary, BlendMode
- **View** - GUI (panele, przyciski, suwaki)
- **Controller** - Application (koordynacja, zdarzenia)

## Zależności

- **SFML 3.0** - grafika, okno, eventy, obrazy
- **C++17** - std::optional, std::filesystem, std::function
- **CMake 3.16+** - system budowania

## Główne klasy

### Application
Główna klasa zarządzająca aplikacją.

**Odpowiedzialności:**
- Pętla główna (eventy, update, render)
- Zarządzanie stanem (viewMode, dragging)
- Dialogi systemowe (otwórz/zapisz plik)
- Koordynacja między komponentami

### ImageProcessor
Przetwarzanie obrazów i nakładanie masek.

**Odpowiedzialności:**
- Wczytywanie obrazów źródłowych i masek
- Piksel-po-pikselu nakładanie maski z wybranym trybem
- Zarządzanie offsetem maski (dla dużych masek)
- Generowanie tekstur do wyświetlania

**Kluczowe metody:**
- `loadSourceImage()` / `loadMask()` - wczytywanie
- `applyMask(mode, transparentColor, useAlpha)` - główna logika
- `saveResult()` - zapis wyniku

**Algorytm applyMask:**
```
dla każdego piksela (x,y) w obrazie źródłowym:
    oblicz pozycję w masce: (x+offsetX, y+offsetY)
    jeśli pozycja w granicach maski:
        pobierz kolor z maski
        zastosuj blend(source, mask, mode, transparentColor, useAlpha)
    inaczej:
        użyj koloru źródłowego
```

### BlendMode
Statyczna klasa z algorytmami mieszania kolorów.

**Tryby podstawowe:**
- **Replace** - `result = mask`
- **Add** - `result = clamp(source + mask)`
- **Multiply** - `result = (source * mask) / 255`

**Tryby rozszerzone:**
- **Screen** - `255 - ((255-s) * (255-m)) / 255`
- **Overlay** - kombinacja multiply/screen w zależności od jasności
- **Difference** - `|source - mask|`
- **Soft/Hard Light** - złożone formuły oświetlenia

**Kluczowe metody:**
- `blend()` - główna funkcja wybierająca tryb
- `isTransparent()` - sprawdza kolor przezroczysty
- `applyAlpha()` - interpolacja z uwzględnieniem kanału alfa

### GUI
Interfejs użytkownika z panelami, przyciskami, suwakami.

**Układ:**
- Lewy panel (250px) - przyciski główne, tryby, color picker
- Prawy panel (200px) - biblioteka masek
- Środek - podgląd obrazu
- Dolny pasek - status, informacje o rozmiarach

**Eventy:**
Zamiast przekazywać sf::Event, GUI używa dedykowanych metod:
- `handleMouseClick(pos)`
- `handleMouseWheel(pos, delta)`
- `handleResize()`

### MaskLibrary
Zarządzanie biblioteką masek.

**Odpowiedzialności:**
- Skanowanie katalogów w poszukiwaniu obrazów
- Przechowywanie ścieżek i tekstur

**Format przechowywania:**
```cpp
struct MaskEntry {
    string name;
    string path;
    Texture thumbnail;
    bool loaded;
};
```

## Struktury danych
**Workflow:**
```
Image (load) → Texture (upload) → Sprite (display)
```

**Callbacki GUI:**
```cpp
std::function<void()> m_onLoadSource;
std::function<void(BlendModeType)> m_onBlendModeChange;
std::function<void(const sf::Color&)> m_onTransparentColorChange;
```

## Budowanie

```cmake
find_package(SFML 3 COMPONENTS Graphics Window System)
if(NOT SFML_FOUND)
    find_package(SFML 2.5 REQUIRED ...)
endif()
```
