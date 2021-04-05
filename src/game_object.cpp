/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game_object.h"

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, float rotation, glm::vec3 color, glm::vec2 velocity) :
    Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(rotation), Sprite(sprite)
{
    
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}