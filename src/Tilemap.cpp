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
    this->tiles.reserve(map_size);
    for(int i = 0; i <  map_size; i++) {
        this->tiles.emplace_back(Tile { 0, 0 });
    }
}

void Tilemap::setTexture(std::shared_ptr<Texture2D> texture) {
    this->texture = std::move(texture);
}

void Tilemap::setTile(int x, int y, uint8_t id, uint8_t index) {
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
    int ox = tx * this->tile_size.width;
    int oy = ty * this->tile_size.height;
    DrawTextureRec(
        *this->texture,
        Rectangle { (float)ox, (float)oy, (float)this->tile_size.width, (float)this->tile_size.height },
        Vector2 { x, y },
        WHITE
    );
}

void Tilemap::draw(float offsetX = 0, float offsetY = 0, int max_width = 0, int max_height = 0) {
    int ox = -offsetX / tile_size.width;
    int oy = -offsetY / tile_size.height;
    int max_tx = ox + max_width / tile_size.width;
    int max_ty = oy + max_height / tile_size.width;

    int start_index = oy * size.width + ox;
    int end_index = max_ty * size.width + max_tx + 1;

    if (end_index > tiles.size()) end_index = tiles.size();

    for (int i = start_index; i < end_index; i++) {
        auto tile = tiles[i];
        int tx = i % this->size.width;
        int ty = i / this->size.width;
        if (tx < ox || ty < oy || tx > max_tx || ty > max_ty) { continue; }
        if (tx > max_tx && ty > max_ty) break;
        int posX = tx * this->tile_size.width + (int)offsetX;
        int posY = ty * this->tile_size.height + (int)offsetY;
        if (tile.index >= 0) this->drawTile(posX, posY, tile.index);
    }
}

void Tilemap::setMinIndex(int i) {
    this->min_index = i;
}