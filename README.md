# Nakładanie masek

Program do nakładania masek na obrazy z różnymi trybami mieszania kolorów.

## Kompilacja

```bash
mkdir build && cd build
cmake ..
make
./MaskOverlay
```

Wymaga SFML 2.5+ i CMake 3.16+. Na macOS: `brew install sfml`

## Jak używać

1. Wczytaj obraz źródłowy
2. Wczytaj maskę (lub wybierz z biblioteki po prawej)
3. Wybierz tryb nakładania
4. Kliknij "Zastosuj maskę"
5. Zapisz wynik

## Tryby nakładania

- Replace - zamienia kolory
- Add - sumuje kolory
- Multiply - mnoży kolory
- Screen - rozjaśnia
- Overlay - kombinacja multiply i screen
- Difference - różnica kolorów
- Soft Light - delikatne światło
- Hard Light - mocne światło

## Funkcje

**Kolor przezroczysty** - piksele maski w tym kolorze (domyślnie magenta) są pomijane

**Kanał alfa** - jeśli włączony, uwzględnia przezroczystość maski dla płynnych przejść

**Przesuwanie** - duże maski można przeciągać myszą

## Skróty klawiszowe

- 1/2/3/4 - widok źródła/maski/wyniku/podzielony
- Spacja - zastosuj maskę
- R - resetuj przesunięcie
- Ctrl+S - zapisz
- Ctrl+O - otwórz obraz
- Esc - wyjdź
