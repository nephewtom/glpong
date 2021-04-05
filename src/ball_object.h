#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"


class BallObject : public GameObject
{
public:
    float   Radius;
    glm::vec2 INITIAL_POSITION;

    bool leftPadCollision = false;
    bool rightPadCollision = false;

    // constructor(s)
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    enum Result { NONE, WALL_HIT, GOAL_LEFT, GOAL_RIGHT };
    Result Move(float dt, unsigned int window_width, unsigned int window_height, float wallWidth);

    static glm::vec2 RandomVelocity();
    static float VELOCITY_RANGE_X[2];
    static float VELOCITY_RANGE_Y[2];
};

#endif