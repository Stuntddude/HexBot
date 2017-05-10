#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <SFML/Graphics.hpp>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef struct {
    i8 type;
    i8 data;
    i16 value;
} cell;

typedef struct {
    i8 x;
    i8 y;
    i16 value;
} move, coord;

bool operator<(coord const & lhs, coord const & rhs) {
    return lhs.value < rhs.value;
}

const i8 EMPTY  = 0;
const i8 WASTE  = 1;
const i8 WATER  = 2;
const i8 FARM   = 3;
const i8 CITY   = 4;
const i8 MARKED = 5;

void printSizes() {
    printf("cell: %lu\n", sizeof(cell));
    printf("sf::CircleShape: %lu\n", sizeof(sf::CircleShape));
    printf("sf::Event: %lu\n", sizeof(sf::Event));
    printf("sf::FloatRect: %lu\n", sizeof(sf::FloatRect));
    printf("sf::Font: %lu\n", sizeof(sf::Font));
    printf("sf::Image: %lu\n", sizeof(sf::Image));
    printf("sf::RenderWindow: %lu\n", sizeof(sf::RenderWindow));
    printf("sf::Text: %lu\n", sizeof(sf::Text));
    printf("sf::VideoMode: %lu\n", sizeof(sf::VideoMode));
    printf("std::string: %lu\n", sizeof(std::string));
    printf("std::vector<cell>: %lu\n", sizeof(std::vector<cell>));
}

namespace std {
    std::string to_string(coord c) {
        return to_string(c.x) + " " + to_string(c.y) + " " + to_string(c.value);
    }
}

template <typename TYPE>
void printVector(std::vector<TYPE> list) {
    if (list.empty()) {
        printf("[]\n");
        return;
    }

    printf("[%s", std::to_string(list.at(0)).c_str());
    for (int i = 1; i < list.size(); ++i)
        printf(", %s", std::to_string(list.at(i)).c_str());
    printf("]\n");
}

#endif
