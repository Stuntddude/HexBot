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

typedef struct {
    i32 index;
    i8 types[4];
} type_bag;

bool operator<(coord const & lhs, coord const & rhs) {
    return lhs.value < rhs.value;
}

const i8 EMPTY  = 0;
const i8 WASTE  = 1;
const i8 WATER  = 2;
const i8 FARM   = 3;
const i8 CITY   = 4;
const i8 MARKED = 5;

//because std::random_shuffle fucking sucks, apparently?
void random_shuffle(i8 types[]) {
    for (int i = 4; i > 1; --i) {
        std::swap(types[rand() % i], types[i - 1]);
    }
}

type_bag randomBag() {
    type_bag bag = { 0, { WASTE, WATER, FARM, CITY } };
    random_shuffle(bag.types);
    return bag;
}

i8 type(type_bag &bag) {
    return bag.types[bag.index];
}

void pickNext(type_bag &bag) {
    ++bag.index;

    if (bag.index == 4) {
        bag.index = 0;
        random_shuffle(bag.types);
    }
}

void pickNextSimple(type_bag &bag) {
    ++bag.index;

    if (bag.index == 4)
        bag.index = 0;
}

type_bag pick(type_bag bag, int i) {
    std::swap(bag.types[bag.index], bag.types[i]);
    return bag;
}

void setNext(type_bag &bag, i8 type) {
    int index = bag.index;
    for (int i = bag.index + 1; i < 4; ++i)
        if (bag.types[i] == type)
            index = i;
    std::swap(bag.types[index], bag.types[bag.index]);
}

void printSizes() {
    type_bag bags[4];
    printf("cell: %lu\n", sizeof(cell));
    printf("type_bag: %lu\n", sizeof(type_bag));
    printf("type_bag[4]: %lu\n", sizeof(bags));
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

#endif
