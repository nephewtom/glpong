/******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "ball_object.h"

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, 0.0f, glm::vec3(1.0f), velocity),
    Radius(radius), INITIAL_POSITION(pos)
{

}

float BallObject::VELOCITY_RANGE_X[2] = { 300.0f, 400.0f };
float BallObject::VELOCITY_RANGE_Y[2] = { 0.0f, 0.0f };
glm::vec2 BallObject::RandomVelocity()
{
    float sign = (rand() % 2 ? 1.0f : -1.0f);
    float vx = sign * (VELOCITY_RANGE_X[0] + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (VELOCITY_RANGE_X[1] - VELOCITY_RANGE_X[0]))));
    sign = (rand() % 2 ? 1.0f : -1.0f);
    float vy = sign * (VELOCITY_RANGE_Y[0] + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (VELOCITY_RANGE_Y[1] - VELOCITY_RANGE_Y[0]))));

    return glm::vec2(vx, vy);
}

BallObject::Result BallObject::Move(float dt, unsigned int window_width, unsigned int window_height, float wallWidth)
{
    // move the ball
    this->Position += this->Velocity * dt;

    if (this->Position.x <= 0.0f - Size.x) {
        this->Position = INITIAL_POSITION;
        this->Velocity = RandomVelocity();
        return GOAL_LEFT;
    }
    else if (this->Position.x >= window_width) {
        this->Position = INITIAL_POSITION;
        this->Velocity = RandomVelocity();
        return GOAL_RIGHT;
    }
    else if (this->Position.y <= 0.0f + wallWidth) {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = wallWidth;
        return WALL_HIT;
    }
    else if (this->Position.y >= window_height - wallWidth - this->Size.y) {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = window_height - this->Size.y - wallWidth;
        return WALL_HIT;
    }
    return NONE;
}