//
// Created by Stephan Bruny on 04.03.23.
//

#include "Textures.h"

Textures::Textures() {}

void Textures::load(const std::string filepath) {
    Image img = LoadImage(filepath.c_str());
    auto texture = std::make_shared<Texture2D>(LoadTextureFromImage(img));
    this->texture_cache.insert(std::pair<std::string, std::shared_ptr<Texture2D>>(filepath, texture));
    UnloadImage(img);
}

std::shared_ptr<Texture2D> Textures::get(const std::string filepath) {
    auto pos = this->texture_cache.find(filepath);
    if (pos == this->texture_cache.end()) {
        throw std::invalid_argument(filepath);
    }
    return pos->second;
}

void Textures::clear() {
    for(auto &texture : this->texture_cache) {
        UnloadTexture(*texture.second);
    }
    this->texture_cache.clear();
}

Textures::~Textures() {
    this->clear();
}