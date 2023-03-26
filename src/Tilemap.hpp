//
// Created by Stephan Bruny on 16.03.23.
//

#ifndef ZOIDAR_TILEMAP_HPP
#define ZOIDAR_TILEMAP_HPP

#include <memory>
#include <vector>
#include "raylib.h"

struct Size {
    int width;
    int height;
};

struct Tile {
    int id;
    int index;
};

class Tilemap {
public:
    Tilemap(int width, int height, int tile_width, int tile_height);

    void setTexture(std::shared_ptr<Texture2D> texture);
    void setTile(int x, int y, int id, int index);
    Tile getTile(int x, int y);

    void drawTile(float x, float y, int index);
    void draw(float offsetX, float offsetY);

    void setMinIndex(int i);

    Size size;
    Size tile_size;

private:
    Vector2 offset;
    std::shared_ptr<Texture> texture;
    std::vector<Tile> tiles;
    int min_index;
};


#endif //ZOIDAR_TILEMAP_HPP
