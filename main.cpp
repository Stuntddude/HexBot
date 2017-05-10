#include <cmath>
#include <cassert>
#include <vector>
#include <SFML/Graphics.hpp>

#ifdef __APPLE__
# include "ResourcePath.hpp"
#endif

#include "Types.hpp"

bool exists(int x, int y) {
    return x + y > 1 && x + y < 7;
}

bool valid(int x, int y) {
    return x >= 0 && y >= 0 && x < 5 && y < 5 && exists(x, y);
}

sf::Vector2f tilePosition(int x, int y) {
    return sf::Vector2f(x * 100 + y * 50, y * 90 + 100);
}

int score(cell state[]) {
    //fast version (if this doesn't work, something's broken)
    int score = 0;
    for (int i = 2; i < 25 - 2; ++i)
        score += state[i].value;
    return score;
}

void drawGame(sf::RenderWindow &window, sf::Font &font, cell state[],
              i8 nextType, int total, int turns) {
    sf::Color colors[6] = {
        sf::Color::White,
        sf::Color::Red,
        sf::Color::Cyan,
        sf::Color::Green,
        sf::Color(0xAAAAAAFF),
        sf::Color(0xFF00FFFF) //magenta means we fucked up
    };

    //draw next tile
    {
        sf::CircleShape hexagon(50, 6);
        hexagon.setPosition(600, 100);
        hexagon.setFillColor(colors[nextType]);
        window.draw(hexagon);
    }

    //draw game stats
    {
        //turns remaining
        sf::Text text(std::to_string(turns), font, 40);
        text.setFillColor(sf::Color::White);
        text.setPosition(600, 350);
        window.draw(text);

        //current round score
        text.setString(std::to_string(score(state)));
        text.setPosition(600, 400);
        window.draw(text);

        //total score
        text.setString(std::to_string(total));
        text.setPosition(600, 450);
        window.draw(text);
    }



    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;
                sf::Vector2f pos = tilePosition(x, y) - sf::Vector2f(50, 50);

                //draw back hexagon
                sf::CircleShape hexagon(50, 6);
                hexagon.setFillColor(sf::Color::White);
                hexagon.setPosition(pos);
                window.draw(hexagon);

                //draw front hexagon
                hexagon.setFillColor(colors[state[i].type]);
                if (state[i].type == CITY) {
                    hexagon.setRadius(state[i].data * 5 + 20);
                    hexagon.move(50 - hexagon.getRadius(), 50 - hexagon.getRadius());
                }
                window.draw(hexagon);

                //draw text
                if (state[i].type != EMPTY) {
                    sf::Text text(std::to_string(state[i].value), font, 40);
                    text.setFillColor(sf::Color::Black);
                    text.setPosition(pos.x + 50 - text.getLocalBounds().width/2,
                                     pos.y + 50 - text.getLocalBounds().height/2 - 12);
                    window.draw(text);
                }
            }
        }
    }
}

bool findWater(cell state[], int x0, int y0) {
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y) &&
                valid(x + 2 - x0, y + 2 - y0) &&
                state[y * 5 + x].type == WATER) {
                return true;
            }
        }
    }
    return false;
}

void updateFarms(cell state[]) {
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;
                if (state[i].type == FARM && !findWater(state, x, y))
                    state[i].type = WASTE;
            }
        }
    }
}

bool hasNeighbor(cell state[], int x, int y, i8 type) {
    int i = y * 5 + x;
    if (valid(x    , y - 1) && state[i - 5].type == type) return true;
    if (valid(x + 1, y - 1) && state[i - 4].type == type) return true;
    if (valid(x - 1, y    ) && state[i - 1].type == type) return true;
    if (valid(x + 1, y    ) && state[i + 1].type == type) return true;
    if (valid(x - 1, y + 1) && state[i + 4].type == type) return true;
    if (valid(x    , y + 1) && state[i + 5].type == type) return true;
    return false;
}

bool hasMinimumNeighbors(cell state[], int x, int y, i8 type, int n) {
    int count = 0;
    int i = y * 5 + x;
    if (valid(x    , y - 1) && state[i - 5].type == type && (count += 1) >= n) return true;
    if (valid(x + 1, y - 1) && state[i - 4].type == type && (count += 1) >= n) return true;
    if (valid(x - 1, y    ) && state[i - 1].type == type && (count += 1) >= n) return true;
    if (valid(x + 1, y    ) && state[i + 1].type == type && (count += 1) >= n) return true;
    if (valid(x - 1, y + 1) && state[i + 4].type == type && (count += 1) >= n) return true;
    if (valid(x    , y + 1) && state[i + 5].type == type && (count += 1) >= n) return true;
    return false;
}

int countNeighbors(cell state[], int x, int y, i8 type) {
    int count = 0;
    int i = y * 5 + x;
    if (valid(x    , y - 1) && state[i - 5].type == type) ++count;
    if (valid(x + 1, y - 1) && state[i - 4].type == type) ++count;
    if (valid(x - 1, y    ) && state[i - 1].type == type) ++count;
    if (valid(x + 1, y    ) && state[i + 1].type == type) ++count;
    if (valid(x - 1, y + 1) && state[i + 4].type == type) ++count;
    if (valid(x    , y + 1) && state[i + 5].type == type) ++count;
    return count;
}

int sumNeighboringCities(cell state[], int x, int y) {
    int sum = 0;
    int i = y * 5 + x;
    if (valid(x    , y - 1) && state[i - 5].type == CITY) sum += state[i - 5].data;
    if (valid(x + 1, y - 1) && state[i - 4].type == CITY) sum += state[i - 4].data;
    if (valid(x - 1, y    ) && state[i - 1].type == CITY) sum += state[i - 1].data;
    if (valid(x + 1, y    ) && state[i + 1].type == CITY) sum += state[i + 1].data;
    if (valid(x - 1, y + 1) && state[i + 4].type == CITY) sum += state[i + 4].data;
    if (valid(x    , y + 1) && state[i + 5].type == CITY) sum += state[i + 5].data;
    return sum;
}

int fl(cell state[], int x, int y, i8 type) { //AKA floodImpl
    int i = y * 5 + x;
    state[i].type = MARKED;

    int count = 1;
    if (valid(x    , y - 1) && state[i - 5].type == type) count += fl(state, x    , y - 1, type);
    if (valid(x + 1, y - 1) && state[i - 4].type == type) count += fl(state, x + 1, y - 1, type);
    if (valid(x - 1, y    ) && state[i - 1].type == type) count += fl(state, x - 1, y    , type);
    if (valid(x + 1, y    ) && state[i + 1].type == type) count += fl(state, x + 1, y    , type);
    if (valid(x - 1, y + 1) && state[i + 4].type == type) count += fl(state, x - 1, y + 1, type);
    if (valid(x    , y + 1) && state[i + 5].type == type) count += fl(state, x    , y + 1, type);

    return count;
}

int flood(cell state[], int x, int y, i8 type) {
    int count = fl(state, x, y, type);

    //return all marked tiles to their prior state
    for (int i = 2; i < 25 - 2; ++i)
        if (state[i].type == MARKED)
            state[i].type = type;

    return count;
}

void attackNeighbors(cell st[], int x, int y) {
    int i = y * 5 + x;
    int dat = st[i].data;
    if (valid(x    , y - 1) && st[i - 5].type == CITY && st[i - 5].data < dat) st[i - 5].type = WASTE;
    if (valid(x + 1, y - 1) && st[i - 4].type == CITY && st[i - 4].data < dat) st[i - 4].type = WASTE;
    if (valid(x - 1, y    ) && st[i - 1].type == CITY && st[i - 1].data < dat) st[i - 1].type = WASTE;
    if (valid(x + 1, y    ) && st[i + 1].type == CITY && st[i + 1].data < dat) st[i + 1].type = WASTE;
    if (valid(x - 1, y + 1) && st[i + 4].type == CITY && st[i + 4].data < dat) st[i + 4].type = WASTE;
    if (valid(x    , y + 1) && st[i + 5].type == CITY && st[i + 5].data < dat) st[i + 5].type = WASTE;
}

void updateCities(cell state[]) {
    coord list[19];
    int cities = 0;

    //grow cities
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int i = y * 5 + x;
            if (exists(x, y) && state[i].type == CITY) {
                int maxLevel = hasNeighbor(state, x, y, WATER)? 1 : 0;
                //TODO: do this in a way that avoids filling farms multiple times
                int farms = fl(state, x, y, FARM);
                maxLevel += farms;
                if (farms > 1) {
                    //return all marked tiles to their prior state
                    for (int j = 2; j < 25 - 2; ++j)
                        if (state[j].type == MARKED)
                            state[j].type = FARM;
                }

                state[i].type = CITY;

                //level up
                state[i].data = std::min(state[i].data + 1, maxLevel);

                //build list of all cities with multiple neighbors
                if (hasMinimumNeighbors(state, x, y, CITY, 2)) {
                    list[cities].x = x;
                    list[cities].y = y;
                    list[cities].value = state[i].data;
                    ++cities;
                }
            }
        }
    }

    std::sort(list, list + cities);

    for (int i = 0; i < cities; ++i)
        attackNeighbors(state, list[i].x, list[i].y);
}

void updateWater(cell state[]) {
    for (int i = 2; i < 25 - 2; ++i) {
        if (state[i].type == WATER)
            state[i].data = 0;
    }

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;
                if (state[i].type == WATER && state[i].data == 0) {
                    int data = fl(state, x, y, WATER);
                    for (int j = i; j < 25 - 2; ++j) {
                        if (state[j].type == MARKED) {
                            state[j].type = WATER;
                            state[j].data = data;
                        }
                    }
                }
            }
        }
    }
}

int updateImpl(cell state[], int x, int y) {
    int i = y * 5 + x;
    switch (state[i].type) {
        case EMPTY: return 0;
        case WASTE: return -5;
        case WATER: return 4 * state[i].data - sumNeighboringCities(state, x, y);
        case FARM : return std::max(0, 30 - countNeighbors(state, x, y, FARM) * 10);
        case CITY : return state[i].data * 10;
    }
}

void update(cell state[]) {
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;
                state[i].value = updateImpl(state, x, y);

                //apply wasteland neighbor penalties
                if (state[i].type != WASTE && std::abs(state[i].value) > 1) {
                    int waste = countNeighbors(state, x, y, WASTE);
                    float realValue = state[i].value;
                    for (int j = 0; j < waste; ++j)
                        realValue *= 0.9f;
                    state[i].value = std::round(realValue);
                }
            }
        }
    }
}

void place(cell state[], int x, int y, i8 type) {
    //any logic that could change the types of tiles needs to be done before scoring

    int i = y * 5 + x;
    i8 prior = state[i].type;
    state[i].type = type;
    state[i].data = 0;

    if (prior == WATER || type == WATER) {
        //TODO: only update neighboring water???
        if (hasNeighbor(state, x, y, WATER))
            updateWater(state);
        else
            state[i].data = 1;
    }

    if (prior == WATER) {
        updateFarms(state);
    } else if (type == FARM && !findWater(state, x, y)) {
        //if we're placing farmland, we only need to update the tile we're placing
        state[i].type = WASTE;
    }

    updateCities(state);

    update(state);
}

move findBestMove(cell state[], i8 nextType) {
    cell next[5 * 5 - 2];

    move best = { 0, 0, -32000 };

    //test every possible move and find the one with the highest immediate payoff
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int i = y * 5 + x;
            if (exists(x, y) && state[i].type != nextType) {
                //create a copy of the current state
                memcpy(next + 2, state + 2, 21 * sizeof(cell));

                place(next, x, y, nextType);

                int value = score(next);

                if (value > best.value) {
                    best.x = x;
                    best.y = y;
                    best.value = value;

//                    printf("NEW BEST: ");
//                } else {
//                    printf("          ");
                }

//                printf("%d, %d: %d\n", x, y, value);
            }
        }
    }

    return best;
}

//forward declared so we can have mutual recursion
move minimax(cell state[], i8 nextType, int layer);

int minimax(cell state[], int layer) {
    int worst = -32000;

    if (layer < 2) {
        for (int i = WASTE; i <= CITY; ++i) {
            move current = findBestMove(state, i);
            if (current.value > worst)
                worst = current.value;
        }
    } else {
        for (int i = WASTE; i <= CITY; ++i) {
            move current = minimax(state, i, layer);
            if (current.value > worst)
                worst = current.value;
        }
    }

    return worst + score(state);
}

move minimax(cell state[], i8 nextType, int layer) {
    cell next[5 * 5 - 2];

    move best = { 0, 0, -32000 };

    //test every possible move and its futures to find the one with the highest future payoff
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int i = y * 5 + x;
            if (exists(x, y) && state[i].type != nextType) {
                //create a copy of the current state
                memcpy(next + 2, state + 2, 21 * sizeof(cell));

                place(next, x, y, nextType);

                int value = minimax(next, layer - 1);

                if (value > best.value) {
                    best.x = x;
                    best.y = y;
                    best.value = value;

//                    printf("NEW BEST: ");
//                } else {
//                    printf("          ");
                }

//                printf("%d, %d: %d\n", x, y, value);
            }
        }
    }

    return best;
}

//TODO: use floating point values for expectimax averages
move findBestAverageMove(cell state[]){
    cell next[5 * 5 - 2];

    move best = { 0, 0, -32000 };

    //test every possible move and find the one with the highest average immediate payoff
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;

                int total = 0;
                int divisor = 0;

                for (int t = WASTE; t <= CITY; ++t) {
                    if (t != state[i].type) {
                        //create a copy of the current state
                        memcpy(next + 2, state + 2, 21 * sizeof(cell));

                        place(next, x, y, t);

                        total += score(next);
                        ++divisor;
                    }
                }

                int avg = round(static_cast<float>(total)/static_cast<float>(divisor));

                if (avg > best.value) {
                    best.x = x;
                    best.y = y;
                    best.value = avg;
                }
            }
        }
    }

    return best;
}

int avgmax(cell state[], int layer) {
    if (layer < 2) {
        return findBestAverageMove(state).value + score(state);
    }

    cell next[5 * 5 - 2];

    move best = { 0, 0, -32000 };

    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            if (exists(x, y)) {
                int i = y * 5 + x;

                int total = 0;
                int divisor = 0;

                for (int t = WASTE; t <= CITY; ++t) {
                    if (t != state[i].type) {
                        //create a copy of the current state
                        memcpy(next + 2, state + 2, 21 * sizeof(cell));

                        place(next, x, y, t);

//                        total += score(next);
                        total += avgmax(next, layer - 1);
                        ++divisor;
                    }
                }

                int avg = round(static_cast<float>(total)/divisor);

                if (avg > best.value) {
                    best.x = x;
                    best.y = y;
                    best.value = avg;
                }
            }
        }
    }

    return best.value + score(state);
}

move avgmax(cell state[], i8 nextType, int layer) {
    cell next[5 * 5 - 2];

    move best = { 0, 0, -32000 };

    //test every possible move and its futures to find the highest average future payoff
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            int i = y * 5 + x;
            if (exists(x, y) && state[i].type != nextType) {
                //create a copy of the current state
                memcpy(next + 2, state + 2, 21 * sizeof(cell));

                place(next, x, y, nextType);

                int value = avgmax(next, layer - 1);

                if (value > best.value) {
                    best.x = x;
                    best.y = y;
                    best.value = value;

                    printf("NEW BEST: ");
                } else {
                    printf("          ");
                }

                printf("%d, %d: %d\n", x, y, value);
            }
        }
    }

    return best;
}

int main(int, char const**) {
    printSizes();

    //create window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    window.setFramerateLimit(60);

    //load font
    sf::Font font;
    if (!font.loadFromFile(
#ifdef __APPLE__
                           resourcePath() +
#endif
                           "sansation.ttf"))
        return EXIT_FAILURE;

    //initialize game state
//    srand(time(NULL));
    cell state[5 * 5 - 2]; //since we never touch the last two cells, no need to store them
    type_bag bag = randomBag();
    //std::find gives inscrutable template errors for no discernable reason
    //(classic C++), so we're doing it by hand
    int waterIndex, farmIndex;
    for (int i = 0; i < 4; ++i) {
        if (bag.types[i] == WATER)
            waterIndex = i;
        else if (bag.types[i] == FARM)
            farmIndex = i;
    }

    //disallow generating a farm tile before a water tile
    if (farmIndex > waterIndex)
        std::swap(bag.types[farmIndex], bag.types[waterIndex]);

    for (int i = 2; i < 25 - 2; ++i)
        state[i] = { 0, 0, 0 };
    int turns = 30;
    int total = 0;
    sf::Time fullTime;

    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            // handle placement of pieces
            if (event.type == sf::Event::MouseButtonPressed) {
                for (int y = 0; y < 5; ++y) {
                    for (int x = 0; x < 5; ++x) {
                        if (exists(x, y)) {
                            sf::Vector2f pos = tilePosition(x, y);
                            pos -= sf::Vector2f(event.mouseButton.x,
                                                event.mouseButton.y);
                            float dist = sqrtf(pos.x * pos.x + pos.y * pos.y);
                            if (dist < 50 && state[y * 5 + x].type != type(bag)) {
                                printf("placing at %d, %d\n", x, y);
                                printf("mouse at %d, %d\n", event.mouseButton.x,
                                                            event.mouseButton.y);
                                printf("tile at %f, %f\n\n", tilePosition(x, y).x,
                                                             tilePosition(x, y).y);
                                place(state, x, y, type(bag));
                                total += score(state);
                                pickNext(bag);
                                --turns;
                            }
                        }
                    }
                }
            }

            //handle auto placement
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                move best;

                //TODO: optimize selection algorithm to take tile bag behavior into account
                sf::Clock clock;
                if (turns < 2) {
                    best = findBestMove(state, type(bag));
                } else {
//                    best = findBestMove(state, nextType);
//                    best = minimax(state, type(bag), std::min(turns, 3));
                    best = avgmax(state, type(bag), std::min(turns, 3));
                }
                sf::Time elapsed = clock.getElapsedTime();
                fullTime += elapsed;

                printf("\nBEST MOVE OVERALL: %d, %d: %d\n", best.x, best.y, best.value);
                printf("      TIME ELAPSED: %f\n", elapsed.asSeconds());
                printf("TOTAL TIME ELAPSED: %f\n\n", fullTime.asSeconds());

                place(state, best.x, best.y, type(bag));
                total += score(state);
                pickNext(bag);
                --turns;
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
//                    case sf::Keyboard::Num1: nextType = WASTE; break;
//                    case sf::Keyboard::Num2: nextType = WATER; break;
//                    case sf::Keyboard::Num3: nextType = FARM ; break;
//                    case sf::Keyboard::Num4: nextType = CITY ; break;
                    case sf::Keyboard::LBracket:
                        std::rotate(bag.types, bag.types + 1, bag.types + 4);
                        break;
                    case sf::Keyboard::RBracket:
                        std::rotate(bag.types, bag.types + 3, bag.types + 4);
                        break;
                    default: break;
                }
            }
        }

        window.clear();

        drawGame(window, font, state, type(bag), total, turns);

        window.display();
    }

    return EXIT_SUCCESS;
}
