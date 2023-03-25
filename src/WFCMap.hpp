//
// Created by Stephan Bruny on 25.03.23.
//

#ifndef ZOIDAR_WFCMAP_HPP
#define ZOIDAR_WFCMAP_HPP

#include <vector>
#include <random>

using namespace std;

struct TileRelation {
    int id;
    std::vector<int> possibleNeighborsTop;
    std::vector<int> possibleNeighborsRight;
    std::vector<int> possibleNeighborsBottom;
    std::vector<int> possibleNeighborsLeft;
};

struct TileFixedRelation {
    int id;
    std::vector<int> neighbors;
};

const vector<int> possible_tile_ids = {
    0, 17, 34,
    1, 18, 35,
    5, 6, 7,
    22, 24,
    39, 40, 41,

    60, 77,
    61, 78
};

const std::vector<TileRelation> combinations = {
        { 0, { 0, 17, 34, 6 }, { 0, 17, 34, 24, 60, 77 }, { 40, 0, 17, 34 }, { 22, 17, 34, 61, 78, 0 } },
        { 17, { 0, 17, 34, 6 }, { 0, 17, 34, 24, 60, 77 }, { 40, 0, 17, 34 }, { 22, 17, 34, 61, 78, 0 } },
        { 34, { 0, 17, 34, 6 }, { 0, 17, 34, 24, 60, 77 }, { 40, 0, 17, 34 }, { 22, 17, 34, 61, 78, 0 } },

        { 1,  { 1, 40, 18, 35 }, { 22, 18, 35, 1 }, { 6, 18, 35, 1 }, { 24, 18, 35, 1 } },
        { 18, { 1, 40, 18, 35 }, { 22, 18, 35, 1 }, { 6, 18, 35, 1 }, { 24, 18, 35, 1 } },
        { 35, { 1, 40, 18, 35 }, { 22, 18, 35, 1 }, { 6, 18, 35, 1 }, { 24, 18, 35, 1 } },

        { 5, { 1, 18, 35 }, { 6 }, { 22 }, { 22 } },
        { 6, { 1, 18, 35 }, { 6, 7, 78 }, { 0, 17, 34 }, { 1, 18, 35 } },
        { 7, { 1, 18, 35 }, { 6 }, { 24 }, { 6 } },

        { 22, { 22, 5 }, { 0, 17, 34 }, { 1 }, { 22, 39 } },
        { 24, { 24, 7 }, { 1, 18, 35 }, { 24, 41 }, { 0, 17, 34 } },

        { 39, { 22 }, { 40 }, { 1, 18, 35 }, { 1, 18, 35 } },
        { 40, { 0 }, { 40, 41, 61 }, { 1, 18, 35 }, { 40, 39, 60 } },
        { 41, { 24 }, { 1, 18, 35 }, { 1 , 18, 35}, { 40 } },

        { 60, { 0, 17, 34 }, { 61, 40 }, { 77, 24 }, { 0, 17, 34 } },
        { 61, { 0, 17, 34 }, { 0, 17, 34 }, { 78, 22 }, { 0, 60, 40 } },
        { 77, { 60, 24 }, { 78, 6 }, { 0, 17, 34 }, { 0, 17, 34 } },
        { 78, { 61, 22 }, { 0, 17, 34 }, { 0, 17, 34 }, { 0, 77, 6 } },

};


struct WaveTile {
    int id { -1 };
    bool isCollapsed { false };
    bool isValid { false };
    vector<int> entropy;
};

class WFCMap {
public:
    std::vector<int> tileMap;
    std::vector<WaveTile> waveMap;
    int map_width;
    int map_height;

    std::random_device randomDevice;
    std::mt19937 randomGenerator;

    const int tile_max = 119; // most right and bottom tile

    WFCMap(int size, int width): tileMap(size, -1) {
        this->map_width = width;
        this->map_height = size / width;
        this->randomGenerator = std::mt19937(this->randomDevice());
        this->init();
    }

    void init() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, (int)possible_tile_ids.size() - 1);
        this->waveMap = vector<WaveTile>(this->tileMap.size());
        /*
        this->waveMap[0].id = possible_tile_ids[getRandomIndex((int)possible_tile_ids.size())];
        this->waveMap[0].isCollapsed = true;
        this->waveMap[0].isValid = true;*/
        for (auto & i : this->waveMap) {
            i.id = possible_tile_ids[getRandomIndex((int)possible_tile_ids.size())];
        }

    }

    int getRandomTileListIndex() {
        return getRandomIndex((int)this->waveMap.size());
    }

    int getRandomTileIndex() {
        return possible_tile_ids[getRandomIndex((int)possible_tile_ids.size())];
    }

    int getRandomIndex(vector<int> &list) {
        std::uniform_int_distribution<> dis(0, (int)list.size() - 1);
        return dis(this->randomGenerator);
    }

    int getRandomIndex(int length) {
        std::uniform_int_distribution<> dis(0, length - 1);
        return dis(this->randomGenerator);
    }

    int getTileIndex(int x, int y) {
        if (x < 0 || y < 0 || x > this->map_width || y > map_height) return 0;
        return y * this->map_width + x;
    }

    int getTileAt(int x, int y) {
        if (x < 0 || y < 0 || x > this->map_width || y > map_height) return 0;
        int index = y * this->map_width + x;
        return this->tileMap[index];
    }

    WaveTile getWaveTileAt(int x, int y) {
        if (x < 0 || y < 0 || x > this->map_width || y > map_height) {
            throw "Oh no!";
        };
        int index = y * (this->map_width - 1) + x;
        return this->waveMap[index];
    }

    TileRelation getRelation(int id, std::vector<TileRelation> relationList) {
        for (auto &rel : relationList) {
            if (rel.id == id) return rel;
        }
        throw exception();
    }

    bool isInIdList(int id, vector<int> list) {
        return find(list.begin(), list.end(), id) != list.end();
    }

    bool isValidLeftNeighbor(int id, int id_right) {
        if (id_right == -1) return true;
        TileRelation right = getRelation(id_right, combinations);
        return isInIdList(id, right.possibleNeighborsLeft);
    }

    bool isValidRightNeighbor(int id, int id_left) {
        if (id_left == -1) return true;
        TileRelation right = getRelation(id_left, combinations);
        return isInIdList(id, right.possibleNeighborsRight);
    }

    bool isValidTopNeighbor(int id, int id_bottom) {
        if (id_bottom == -1) return true;
        TileRelation right = getRelation(id_bottom, combinations);
        return isInIdList(id, right.possibleNeighborsBottom);
    }

    bool isValidBottomNeighbor(int id, int id_top) {
        if (id_top == -1) return true;
        TileRelation right = getRelation(id_top, combinations);
        return isInIdList(id, right.possibleNeighborsTop);
    }

    bool isValidNeighbor(int id, int x, int y) {
        if (id < 0) return false;
        if (x > 0) {
            if (!isValidRightNeighbor(id, getWaveTileAt(x - 1, y).id)) return false;
        }
        if (y > 0) {
            if (!isValidTopNeighbor(id, getWaveTileAt(x, y - 1).id)) return false;
        }
        if (x < this->map_width)
            if (!isValidLeftNeighbor(id, getWaveTileAt(x + 1, y).id)) return false;
        if (y < this->map_height - 1)
            if (!isValidBottomNeighbor(id, getWaveTileAt(x, y + 1).id)) return false;
        return true;
    }

    void markInvalid() {
        for (int i = 0; i < this->waveMap.size(); i++) {
            int x = i % this->map_width;
            int y = i / this->map_width;
            auto &current_tile = this->waveMap[i];
            if (current_tile.isCollapsed && current_tile.isValid) continue;
            bool valid = isValidNeighbor(current_tile.id, x, y);
            current_tile.isValid = valid;
            current_tile.isCollapsed = valid;
        }
    }

    vector<int> excludeNeighbors(vector<int> entropy, vector<int> potentialNeighbors) {
        entropy.erase(
            remove_if(
                entropy.begin(),
                entropy.end(),
                [&](int const & id) {
                    return find(potentialNeighbors.begin(), potentialNeighbors.end(), id) == potentialNeighbors.end();
                }
            ), entropy.end()
        );
        return entropy;
    }

    void invalidateNeighbors(int x, int y) {
        if (x > 0) {
            int left = getTileIndex(x - 1, y);
            this->waveMap[left].isValid = false;
        }
        if (y > 0) {
            int top = getTileIndex(x, y - 1);
            this->waveMap[top].isValid = false;
        }
        if (x < map_width - 1) {
            int right = getTileIndex(x + 1, y);
            this->waveMap[right].isValid = false;
        }
        if (y < map_height - 1) {
            int bottom = getTileIndex(x, y + 1);
            this->waveMap[bottom].isValid = false;
        }
    }

    void invalidateNeighbors(int index) {
        int x = index % this->map_width;
        int y = index / this->map_width;
        invalidateNeighbors(x, y);
    }

    void invalidateMap() {
        for (int i = 0; i < this->waveMap.size(); i++) {
            int x = i % this->map_width;
            int y = i / this->map_width;
            auto &current_tile = this->waveMap[i];
            if (!current_tile.isValid) {
                invalidateNeighbors(x, y);
            }
        }
    }

    void collapseMap(int retry = 0, bool onlyInvalids = false) {
        bool tryAgain = false;
        int retries = 3;
        for (int i = 0; i < this->waveMap.size(); i++) {
            int x = i % this->map_width;
            int y = i / this->map_width;
            auto &current_tile = this->waveMap[i];

            if (onlyInvalids) {
                if (current_tile.isValid) continue;
            }

            if (current_tile.isCollapsed) continue;
            current_tile.isCollapsed = isValidNeighbor(current_tile.id, x, y);
            if (current_tile.isCollapsed) continue;

            vector<int> options = possible_tile_ids;

            if (x > 0) {
                auto left = getWaveTileAt(x - 1, y);
                if (left.isCollapsed) {
                    options = excludeNeighbors(options, { getRelation(left.id, combinations).possibleNeighborsRight });
                }
            }

            if (y > 0) {
                auto top = getWaveTileAt(x, y - 1);
                if (top.isCollapsed) {
                    options = excludeNeighbors(options, { getRelation(top.id, combinations).possibleNeighborsBottom });
                }
            }

            if (x < map_width - 1) {
                auto right = getWaveTileAt(x + 1, y);
                if (right.isCollapsed) {
                    options = excludeNeighbors(options, { getRelation(right.id, combinations).possibleNeighborsLeft });
                }
            }

            if (y < map_height - 1) {
                auto bottom = getWaveTileAt(x, y + 1);
                if (bottom.isCollapsed) {
                    options = excludeNeighbors(options, { getRelation(bottom.id, combinations).possibleNeighborsTop });
                }
            }

            if (!options.empty()) {
                current_tile.id = options[getRandomIndex((int)options.size())];
                current_tile.isCollapsed = isValidNeighbor(current_tile.id, x, y);
            } else {
                current_tile.id = getRandomTileIndex();
                current_tile.isCollapsed = false;
                current_tile.isValid = false;
            }
        }
        markInvalid();
    }

    void randomizeOne() {
        int index = getRandomTileListIndex();
        this->waveMap[index].id = getRandomTileIndex();
        this->waveMap[index].isCollapsed = false;
        this->waveMap[index].isValid = false;
        invalidateNeighbors(index);
    }

    void generate() {
        this->init();
        this->collapseMap();
        this->render();
    }

    void render() {
        for (int i = 0; i < this->waveMap.size(); i++) {
            /*if (!this->waveMap[i].isValid) {
                this->tileMap[i] = 102;
                continue;
            }*/
            this->tileMap[i] = this->waveMap[i].id;
        }
    }

    void regenerate() {
        this->collapseMap(0, true);
        this->render();
    }

    std::vector<int> getMap() {
        return this->tileMap;
    }
};

#endif //ZOIDAR_WFCMAP_HPP
