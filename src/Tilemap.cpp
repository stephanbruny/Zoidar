//
// Created by Stephan Bruny on 16.03.23.
//

#include "Tilemap.hpp"

#include <utility>

Tilemap::Tilemap(int width, int height, int tile_width, int tile_height) {
    this->size = { width, height };
    this->tile_size = { tile_height, tile_height };
    this->min_index = 0;
    const int map_size = width * height;
    for(int i = 0; i <  map_size; i++) {
        this->tiles.push_back(Tile { 0, 0 });
    }
}

void Tilemap::setTexture(std::shared_ptr<Texture2D> texture) {
    this->texture = texture;
}

void Tilemap::setTile(int x, int y, int id, int index) {
    int position = y * this->size.width + x;
    this->tiles[position] = { id, index };
}

Tile Tilemap::getTile(int x, int y) {
    int position = y * this->size.width + x;
    return this->tiles[position];
}

void Tilemap::drawTile(float x, float y, int index) {
    if (this->min_index > 0 && index < this->min_index || index < 0) return;
    int w = this->texture->width / this->tile_size.width;
    int tx = index % w;
    int ty = index / w;
    float ox = tx * this->tile_size.width;
    float oy = ty * this->tile_size.height;
    DrawTextureRec(
        *this->texture,
        Rectangle { ox, oy, (float)this->tile_size.width, (float)this->tile_size.height },
        Vector2 { x, y },
        WHITE
    );
}

void Tilemap::draw() {
    int current_tile_index = 0;
    for (auto &tile : this->tiles) {
        int tx = current_tile_index % this->size.width;
        int ty = current_tile_index / this->size.width;
        this->drawTile(tx * this->tile_size.width, ty * this->tile_size.height, tile.index);
        current_tile_index++;
    }
}

void Tilemap::setMinIndex(int i) {
    this->min_index = i;
}