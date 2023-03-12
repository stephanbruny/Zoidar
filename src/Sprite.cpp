//
// Created by Stephan Bruny on 04.03.23.
//

#include "Sprite.h"

Sprite::Sprite(std::shared_ptr<Texture2D> texture) {
    this->texture = texture;
    this->position = Vector2 { 0, 0 };
    this->tint = Color { 255, 255, 255, 255 };
}

void Sprite::move(int x, int y) {
    this->position.x = x;
    this->position.y = y;
}

void Sprite::moveRel(int x, int y) {
    this->position.x += x;
    this->position.y += y;
}

void Sprite::draw() {
    DrawTexture(*this->texture, this->position.x, this->position.y, this->tint);
};