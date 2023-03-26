//
// Created by Stephan Bruny on 26.03.23.
//

#ifndef ZOIDAR_WORLDGENERATOR_HPP
#define ZOIDAR_WORLDGENERATOR_HPP

#include <vector>
#include <random>
#include <map>
#include "../lib/Perlin-Noise.hpp"

using namespace std;

enum TileType {
    Water,
    Sand,
    Grass,
    Wood,
    Mountain,
    Max,
    Unknown,
    Any
};

enum Direction {
    Left = 0, Up, Right, Down
};

struct ProtoTile {
    int id { -1 };
    bool isNonTile { false };
    bool visited { false };
    bool varied { false };
    TileType left;
    TileType top;
    TileType right;
    TileType bottom;
    TileType type { TileType::Unknown };
};

const map<TileType, vector<int>> TileVariants = {
        { Water, { 0, 17, 34 } },
        { Sand, { 1, 18, 35 } },
        { Grass, { 2, 19, 36 } },
        { Wood, { 3, 20, 37 } },
        { Mountain, { 4, 21, 38 } }
};

const map<vector<TileType>, int> TilePatterns = {
        { { Sand, Sand, Water, Water }, 78 },
        { { Any, Sand, Any, Water }, 6 },

        { { Any, Any, Sand, Grass }, 25 },
        { { Sand, Any, Grass, Sand }, 25 },
        { { Sand, Sand, Grass, Sand }, 25 },

        { { Sand, Sand, Grass, Grass }, 80 },
        { { Sand, Sand, Sand, Grass }, 9 },
};

class WorldGenerator {
public:
    random_device randomDevice;
    mt19937 randomGenerator;

    siv::PerlinNoise perlinNoise;

    vector<ProtoTile> tilemap;
    int map_width;
    int map_height;

    int offset_x = 0;
    int offset_y = 0;

    WorldGenerator() {
        this->randomGenerator = mt19937(this->randomDevice());

        initializePerlinNoise();
    }

    ProtoTile getTileAt(int x, int y) {
        int index = y * this->map_width + x;
        if (index < 0 || index > tilemap.size() - 1) return { .isNonTile = true, .type = Any };
        return this->tilemap[index];
    }

    void initializePerlinNoise() {
        uniform_int_distribution dist(0, INT_MAX);
        this->perlinNoise = siv::PerlinNoise(dist(this->randomGenerator));
    }

    int getRandomIndex(vector<int> &list) {
        std::uniform_int_distribution<> dis(0, (int)list.size() - 1);
        return dis(this->randomGenerator);
    }

    void moveOffset(int x, int y) {
        this->offset_x += x;
        this->offset_y += y;
    }

    void generate(int width, int height) {
        initializePerlinNoise();
        tilemap = vector<ProtoTile>(width * height);
        map_width = width;
        map_height = height;

        this->generatePerlinNoise(this->offset_x, this->offset_y);
        this->linkTileNeighbors();
        this->randomizeVariants();
        this->smoothEdges();
    }

    void generatePerlinNoise(int ox, int oy, double depth = 0.033) {
        for (int i = 0; i < tilemap.size(); i++) {
            int x = i % map_width;
            int y = i / map_width;

            // double noise = perlinNoise.noise2D((x + ox) * 0.01, (y + oy) * 0.01) * (int)TileType::Unknown;
            double noise = perlinNoise.noise2D_01((x + ox) * depth, (y + oy) * depth) * (int)TileType::Unknown;

            int id = noise < 0 ? 0 : (int)noise;

            tilemap[i].id = id;
            tilemap[i].type = (TileType)id;
        }
    }

    // This is super slow
    void linkTileNeighbors() {
        cout << "Linking neigbors" << endl;
        auto size = tilemap.size();
        int i = 0;
        for (auto &tile : tilemap) {
            int x = i % map_width;
            int y = i / map_width;

            tilemap[i].left = getTileAt(x - 1, y).type;
            tilemap[i].top = getTileAt(x, y - 1).type;
            tilemap[i].right = getTileAt(x + 1, y).type;
            tilemap[i].bottom = getTileAt(x, y + 1).type;
            i++;
        }
    }

    void randomizeVariants() {
        for (auto &tile : tilemap) {
            TileType type = (TileType)tile.id;
            auto variants = TileVariants.find(type);
            if (variants != TileVariants.end()) {
                auto list = variants->second;
                int index = getRandomIndex(list);
                tile.id = list[index];
                tile.varied = true;
            }
        }
    }

    int determineTileId(ProtoTile &tile) {
        vector<TileType> basePattern = { tile.left, tile.top, tile.right, tile.bottom };
        auto match = TilePatterns.find(basePattern);
        if (match != TilePatterns.end()) {
            return match->second;
        }
        return tile.id;
    }

    void smoothEdges() {
        cout << "Smoothing edges" << endl;
        for (auto &tile : tilemap) {
            tile.id = determineTileId(tile);
        }
    }

    vector<int> render() {
        vector<int> result;
        for (auto &tile : tilemap) {
            result.push_back(tile.id);
        }
        return result;
    }
};

#endif //ZOIDAR_WORLDGENERATOR_HPP
