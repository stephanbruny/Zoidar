//
// Created by Stephan Bruny on 04.03.23.
//

#ifndef ZOIDAR_SPRITE_H
#define ZOIDAR_SPRITE_H

#include <memory>
#include "raylib.h"

class Sprite {
private:
    std::shared_ptr<Texture2D> texture;
public:
    Vector2 position;
    Color tint;

    explicit Sprite(std::shared_ptr<Texture2D> texture);
    void move(int x, int y);
    void moveRel(int x, int y);
    void draw();
};


#endif //ZOIDAR_SPRITE_H
