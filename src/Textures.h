//
// Created by Stephan Bruny on 04.03.23.
//

#ifndef ZOIDAR_TEXTURES_H
#define ZOIDAR_TEXTURES_H

#include <string>
#include <map>
#include "raylib.h"

class Textures {
private:
    std::map<const std::string, std::shared_ptr<Texture2D>> texture_cache;

public:
    Textures();
    void load(const std::string filepath);
    std::shared_ptr<Texture2D> get(std::string filepath);
    void clear();
};


#endif //ZOIDAR_TEXTURES_H
