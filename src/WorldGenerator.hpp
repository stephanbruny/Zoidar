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
    TileType left { Any };
    TileType top { Any };
    TileType right { Any };
    TileType bottom { Any };
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
        { { Sand, Sand, Sand, Water, Water }, 78 },
        { { Sand, Sand, Sand, Water, Sand }, 22 },
        { { Any, Sand, Sand, Any, Water }, 6 },
        { { Sand, Sand, Sand, Sand, Water }, 6 },
        { { Water, Sand, Sand, Sand, Sand }, 24 },
        { { Sand, Water, Sand, Sand, Sand }, 40 },
        { { Water, Water, Sand, Sand, Sand }, 60 },
        { { Sand, Water, Sand, Water, Sand }, 61 },
        { { Water, Sand, Sand, Sand, Water }, 77 },

        // { { Any, Any, Sand, Grass }, 25 },
        { { Sand, Any, Sand, Grass, Sand }, 25 },
        { { Sand, Sand, Sand, Grass, Sand }, 25 },
        { { Sand, Sand, Sand, Grass, Grass }, 80 },
        { { Sand, Sand, Sand, Sand, Grass }, 9 },
        { { Sand, Grass, Sand, Grass, Sand }, 63 },
        { { Sand, Grass, Sand, Sand, Sand }, 43 },
        { { Grass, Grass, Sand, Sand, Sand }, 62 },
        { { Grass, Sand, Sand, Sand, Sand }, 27 },
        { { Grass, Sand, Sand, Sand, Grass }, 79 },

        // Grass<>Forest
        { { Grass, Grass, Wood, Grass, Wood }, 95 },
        { { Grass, Grass, Wood, Wood, Wood }, 94 },
        { { Wood, Wood, Wood, Grass, Wood }, 68 },
        { { Grass, Wood, Wood, Wood, Wood }, 70 },
        { { Wood, Wood, Wood, Grass, Grass }, 112 },
        { { Wood, Grass, Wood, Grass, Wood }, 95 },
        { { Wood, Grass, Wood, Wood, Wood }, 86 },
        { { Wood, Wood, Wood, Wood, Grass }, 52 },
        { { Grass, Wood, Wood, Wood, Grass }, 111 },

        // Wood<>Mountain
        { { Wood, Wood, Mountain, Mountain, Mountain }, 98 },
        { { Mountain, Wood, Mountain, Wood, Mountain }, 99 },
        { { Mountain, Wood, Mountain, Wood, Mountain }, 99 },
        { { Mountain, Mountain, Mountain, Wood, Wood }, 116 },
        { { Wood, Mountain, Mountain, Mountain, Wood }, 115 },
        { { Mountain, Wood, Mountain, Mountain, Mountain }, 92 },
        { { Mountain, Mountain, Mountain, Wood, Mountain }, 74 },
        { { Mountain, Mountain, Mountain, Mountain, Wood }, 58 },
        { { Wood, Mountain, Mountain, Mountain, Mountain }, 76 },
};

const map<TileType, vector<int>> LayerTiles = {
        { Sand, { 119, 120, 121 } },
        { Grass, { 122, 123, 124 } },
        { Wood, { 125, 126, 127 } },
        { Mountain, { 136, 137, 138 } },
};

const map<vector<int>, vector<int>> LayerCombinations = {
        { { 126, 126, 126, 126 }, { 132, 133, 149, 150 } },
        { { 136, 136, 136, 136 }, { 134, 135, 151, 152 } },
        { { 137, 137, 137, 137 }, { 168, 169, 185, 186 } },
};

class WorldGenerator {
public:
    random_device randomDevice;
    mt19937 randomGenerator;

    siv::PerlinNoise perlinNoise;

    vector<ProtoTile> tilemap;
    vector<ProtoTile> layer;
    int map_width {};
    int map_height {};

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

    ProtoTile getUpperTileAt(int x, int y) {
        int index = y * this->map_width + x;
        if (index < 0 || index > tilemap.size() - 1) return { .isNonTile = true, .type = Any };
        return this->layer[index];
    }

    void setUpperLayerTileId(int x, int y, int id) {
        int index = y * this->map_width + x;
        this->layer[index].id = id;
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

        this->generateBase();
        this->linkTileNeighbors();
        this->randomizeVariants();
        this->smoothEdges();
        this->generateLayer(); // Generate second layer
        this->buildUpperLayerCombinations();
    }

    void generateBase() {
        auto values = generatePerlinNoise((int)TileType::Unknown, 0, 0);
        int i = 0;
        for (auto &id : values) {
            tilemap[i].id = id;
            tilemap[i].type = (TileType)id;
            i++;
        }
    }

    void generateLayer() {
        layer.clear();
        auto values = generatePerlinNoise(10, 0, 0, 2.66);

        for (int i = 0; i < values.size(); i++) {
            layer.push_back({ .id = -1, .type = TileType::Unknown });
            TileType type = tilemap[i].type;
            auto candidates = LayerTiles.find(type);
            if (candidates != LayerTiles.end()) {
                auto candidateList = candidates->second;
                int randIndex = getRandomIndex(candidateList);
                if (values[i] < (int)type * 2) {
                    layer[i].id = candidateList[randIndex];
                    layer[i].type = type;
                }
            }
        }
    }

    [[nodiscard]] vector<int> generatePerlinNoise(int max, int ox, int oy, double depth = 0.033) const {
        vector<int> result;
        cout << "generate perlin noise" << endl;
        for (int i = 0; i < tilemap.size(); i++) {
            int x = i % map_width;
            int y = i / map_width;

            // double noise = perlinNoise.noise2D((x + ox) * 0.01, (y + oy) * 0.01) * (int)TileType::Unknown;
            double noise = perlinNoise.noise2D_01((x + ox) * depth, (y + oy) * depth) * max;

            int id = noise < 0 ? 0 : (int)noise;
            result.push_back(id);
        }
        return result;
    }

    // This is super slow
    void linkTileNeighbors() {
        cout << "Linking neighbors" << endl;
        auto size = tilemap.size();
        for (int i = 0; i < tilemap.size(); i++) {
            int x = i % map_width;
            int y = i / map_width;

            tilemap[i].left = getTileAt(x - 1, y).type;
            tilemap[i].top = getTileAt(x, y - 1).type;
            tilemap[i].right = getTileAt(x + 1, y).type;
            tilemap[i].bottom = getTileAt(x, y + 1).type;
        }
    }

    void randomizeVariants() {
        cout << "randomize variants" << endl;
        for (auto &tile : tilemap) {
            auto type = (TileType)tile.id;
            auto variants = TileVariants.find(type);
            if (variants != TileVariants.end()) {
                auto list = variants->second;
                int index = getRandomIndex(list);
                tile.id = list[index];
                tile.varied = true;
            }
        }
    }

    static int determineTileId(ProtoTile &tile) {
        vector<TileType> basePattern = { tile.left, tile.top, tile.type, tile.right, tile.bottom };
        auto match = TilePatterns.find(basePattern);
        if (match != TilePatterns.end()) {
            return match->second;
        }
        return tile.id;
    }

    void smoothEdges() {
        cout << "Smoothing edges" << endl;
        for (int i = 0; i < tilemap.size(); i++) {
            tilemap[i].id = determineTileId(tilemap[i]);
        }
    }

    void buildUpperLayerCombinations() {
        for(int i = 0; i < layer.size(); i += 2) {
            int x = i % map_width;
            int y = i / map_width;

            vector<int> group = {
                    getUpperTileAt(x, y).id,
                    getUpperTileAt(x + 1, y).id,
                    getUpperTileAt(x, y + 1).id,
                    getUpperTileAt(x + 1, y + 1).id,
            };

            auto match = LayerCombinations.find(group);
            if (match != LayerCombinations.end()) {
                auto replacement = match->second;
                setUpperLayerTileId(x, y, replacement[0]);
                setUpperLayerTileId(x + 1, y, replacement[1]);
                setUpperLayerTileId(x, y + 1, replacement[2]);
                setUpperLayerTileId(x + 1, y + 1, replacement[3]);
            }
        }
    }

    vector<int> render() {
        vector<int> result;
        for (auto &tile : tilemap) {
            result.push_back(tile.id);
        }
        return result;
    }

    vector<int> renderUpperLayer() {
        vector<int> result;
        for (auto &tile : layer) {
            result.push_back(tile.id);
        }
        return result;
    }
};

#endif //ZOIDAR_WORLDGENERATOR_HPP
