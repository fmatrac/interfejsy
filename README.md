# Nakładanie masek - Projekt

Program do nakładania masek na obrazy z wykorzystaniem biblioteki SFML.

## Opis

Program umożliwia wczytanie obrazu źródłowego i maski, a następnie nałożenie maski na obraz z wykorzystaniem różnych trybów nakładania kolorów. Wynik można zapisać do pliku BMP/PNG.

## Wymagania

- CMake 3.16+
- SFML 2.5+
- Kompilator C++17 (GCC, Clang lub MSVC)

## Instalacja SFML na macOS

```bash
brew install sfml
```

## Kompilacja

```bash
mkdir build
cd build
cmake ..
make
```

## Uruchomienie

```bash
./MaskOverlay
```

## Funkcje programu

### Wymagania podstawowe (zrealizowane):
- ✅ Wczytywanie obrazów BMP/PNG/JPG
- ✅ Wczytywanie masek
- ✅ Nakładanie z zamianą kolorów (Replace)
- ✅ Nakładanie z sumowaniem kolorów (Add)
- ✅ Nakładanie z mnożeniem kolorów (Multiply)
- ✅ Wybór koloru przezroczystego (RGB picker)
- ✅ Zapis wyniku do pliku BMP/PNG

### Wymagania rozszerzone (zrealizowane):
- ✅ Dodatkowe tryby nakładania:
  - Screen (rozjaśnianie)
  - Overlay (nakładka)
  - Difference (różnica)
  - Soft Light (miękkie światło)
  - Hard Light (twarde światło)
- ✅ Obsługa kanału alfa (częściowa przezroczystość)
- ✅ Obsługa masek większych od obrazu (przesuwanie myszą)
- ✅ Biblioteka masek z miniaturkami

## Sterowanie

| Klawisz | Funkcja |
|---------|---------|
| 1 | Widok obrazu źródłowego |
| 2 | Widok maski |
| 3 | Widok wyniku |
| 4 | Widok podzielony (źródło + wynik) |
| Spacja | Zastosuj maskę |
| R | Resetuj przesunięcie maski |
| Ctrl+S | Zapisz wynik |
| Ctrl+O | Otwórz obraz źródłowy |
| Esc | Zamknij program |

## Tryby nakładania

### Podstawowe:
1. **Zamiana** - piksel maski zastępuje piksel obrazu
2. **Sumowanie** - kolory są dodawane (z ograniczeniem do 255)
3. **Mnożenie** - kolory są mnożone (normalizowane)

### Rozszerzone:
4. **Screen** - rozjaśnianie: `1 - (1-a) * (1-b)`
5. **Nakładka** - kombinacja mnożenia i screen
6. **Różnica** - wartość bezwzględna różnicy kolorów
7. **Miękkie światło** - delikatne oświetlenie
8. **Twarde światło** - mocniejsze oświetlenie

## Kolor przezroczysty

Program pozwala wybrać kolor, który będzie traktowany jako przezroczysty w masce. Domyślnie jest to magenta (255, 0, 255). Piksele maski o tym kolorze nie będą nakładane na obraz źródłowy.

Dodatkowo program obsługuje kanał alfa - jeśli maska zawiera przezroczystość (np. PNG z kanałem alfa), ta przezroczystość będzie uwzględniona przy nakładaniu.

## Przesuwanie maski

Jeśli maska jest większa od obrazu źródłowego, można ją przesuwać przeciągając myszą w obszarze podglądu. Pozwala to wybrać odpowiedni fragment maski do nałożenia na obraz.

## Biblioteka masek

Program automatycznie skanuje katalog `masks` i wyświetla znalezione maski jako miniaturki w prawym panelu. Kliknięcie na miniaturkę automatycznie wczytuje daną maskę.

## Tworzenie własnych masek

Maski można tworzyć w dowolnym programie graficznym (np. GIMP, Photoshop, Paint). Wystarczy:
1. Utworzyć obraz w formacie BMP lub PNG
2. Obszary, które mają być przezroczyste, wypełnić wybranym kolorem (domyślnie magenta) lub ustawić jako przezroczyste (kanał alfa)
3. Zapisać plik w katalogu `masks`

## Przykłady zastosowań

- Tworzenie winietek i obramowań dla zdjęć
- Nakładanie wzorców na zdjęcia medyczne/techniczne
- Dodawanie skali pomiarowej
- Efekty artystyczne (portrety w kwiatach, itp.)

## Struktura projektu

```
projekt/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Application.h
│   ├── BlendMode.h
│   ├── GUI.h
│   ├── ImageProcessor.h
│   └── MaskLibrary.h
├── src/
│   ├── main.cpp
│   ├── Application.cpp
│   ├── BlendMode.cpp
│   ├── GUI.cpp
│   ├── ImageProcessor.cpp
│   └── MaskLibrary.cpp
└── masks/
    └── (pliki masek)
```

## Autor

Projekt nr 16 - Interfejsy użytkownika i biblioteki graficzne
