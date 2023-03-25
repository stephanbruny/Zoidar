//
// Created by Stephan Bruny on 25.03.23.
//

#ifndef ZOIDAR_WFCMAP_HPP
#define ZOIDAR_WFCMAP_HPP

#include <vector>
#include <random>

struct TileRelation {
    int id;
    std::vector<int> possibleNeighborsTop;
    std::vector<int> possibleNeighborsRight;
    std::vector<int> possibleNeighborsBottom;
    std::vector<int> possibleNeighborsLeft;
};

const TileRelation water {
    0,
    { 0, 5, 6, 7, 17, 34, 77, 78 },
    { 0, 17, 34, 7,24, 41,60, 77 },
    { 0, 17, 34, 40, 60, 61 },
    { 0, 6, 17, 5, 22, 39, 61, 78 }
};

const TileRelation sand {
    1,
    { 1, 18, 35, 6, 11, 12, 13,39, 40, 41,  61, 64, 101 },
    { 1, 18, 35, 5, 22, 39, 8,  25, 42, 28, 79, 82, 76 },
    { 1, 18, 35, 5, 6, 7, 8, 9, 10, 46, 77, 78, 79, 80  },
    { 1, 18, 35, 7, 24, 41, 10, 27, 44,  28, 60, 77, 62, 79, 65, 82, 99 }
};

const TileRelation grass {
        2,
        { 2, 19,  36, 46, 79, 80 },
        { 2, 19,  36, 27, 28, 63, 80 },
        { 2, 19,  36, 11, 12, 13, 62, 63 },
        { 2, 19,  36, 25, 13, 30, 47, 62, 79 }
};

enum Biom {
    WATER,
    SAND,
    GRASS
};

enum class NeighborDirection {
    Top,
    Right,
    Bottom,
    Left
};

class WFCMap {
public:
    std::vector<int> tiles;
    int map_width;

    WFCMap(int size, int width): tiles(size, -1) {
        this->map_width = width;
    }

    TileRelation getByBiom(Biom biom) {
        if (biom == WATER) return water;
        if (biom == SAND) return sand;
        return grass;
    }

    std::vector<int> getTileRelationsByDirection(TileRelation tiles, NeighborDirection dir) {
        switch (dir) {
            case NeighborDirection::Top:
                return tiles.possibleNeighborsTop;
                break;
            case NeighborDirection::Right:
                return tiles.possibleNeighborsRight;
                break;
            case NeighborDirection::Bottom:
                return tiles.possibleNeighborsBottom;
                break;
            default:
                return tiles.possibleNeighborsLeft;
        }
    }

    NeighborDirection getOppositeDirection(NeighborDirection dir) {
        switch (dir) {
            case NeighborDirection::Top:
                return NeighborDirection::Bottom;
                break;
            case NeighborDirection::Right:
                return NeighborDirection::Left;
                break;
            case NeighborDirection::Bottom:
                return NeighborDirection::Top;
                break;
            default:
                return NeighborDirection::Right;
        }
    }

    std::vector<int> findCandidates(int id, NeighborDirection dir) {
        for (int b = 0; b <= GRASS; b++) {
            auto tiles = getByBiom((Biom)b);
            auto odir = getOppositeDirection(dir);
            auto refTiles = getTileRelationsByDirection(tiles, odir);
            for (int &c : tiles.possibleNeighborsRight) {
                if (c == id) return getTileRelationsByDirection(tiles, dir);
            }
        }
        return { -1 };
    }

    int getIndex(int x, int y) {
        return y * this->map_width + x;
    }

    int getTileAt(int x, int y) {
        int index = y * this->map_width + x;
        return this->tiles[index];
    }

    void generate() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 2);
        Biom randomBiom = (Biom)dis(gen);
        TileRelation tiles = this->getByBiom(randomBiom);
        this->tiles[0] = tiles.id;

        for(int i = 1; i < this->tiles.size(); i++) {
            int x = i % this->map_width;
            int y = i / this->map_width;
            std::vector<int> entropy;
            if (x > 0) {
                auto leftCandiates = findCandidates(getTileAt(x - 1, y), NeighborDirection::Right);
                entropy.insert(entropy.end(), leftCandiates.begin(), leftCandiates.end());
            }
            if (y > 0) {
                auto topCandiates = findCandidates(getTileAt(x, y - 1), NeighborDirection::Bottom);
                entropy.insert(entropy.end(), topCandiates.begin(), topCandiates.end());
            }
            std::uniform_int_distribution<> entropy_dist(0, entropy.size());
            this->tiles[i] = entropy_dist(gen);
        }
    }

    std::vector<int> getMap() {
        return this->tiles;
    }
};

#endif //ZOIDAR_WFCMAP_HPP
