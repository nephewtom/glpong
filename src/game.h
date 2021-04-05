#ifndef GAME_H
#define GAME_H
#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_object.h"
#include "ball_object.h"

// TASK(blink)
#define TASK(name) \
    bool name = true; \
    float name##Time = 0.0f;

// TASK_DO_FOR(blink, 0.5f)
#define TASK_DO_FOR(name, duration, not) \
if (not##name) { \
    if (name##Time == 0.0f) { \
        name##Time = Time; \
    } \
    if (Time > name##Time + duration) {\
        name = !name;\
        name##Time = 0.0f; \
    } \
}
// TASK_NOT_FOR(blink, 1.5f)
#define TASK_NOT_FOR(name, duration) TASK_DO_FOR(name, duration, !)

#define IF_TASK(name) \
if (name) {
#define FI_TASK }

// TASK_DO_WHEN(goalScored)
// SoundEngine->play2D("audio/whistle.mp3", false);
// TASK_AT_END(goalScored, 1.0f)
// TASK_DONE

// TASK_DO_WHEN(bothReady)
// SoundEngine->stopAllSounds();
// SoundEngine->play2D(FileSystem::getPath("audio/game.mp3").c_str(), true);
// scoreRightInt = scoreLeftInt = 0;
// TASK_AT_END(bothReady, 3.0f)
// State = GAME_ACTIVE;
// std::cout << "Game starts!" << std::endl;
// TASK_DONE

// TASK_TRUE()
#define TASK_TRUE(name) \
    name = true; \

enum GameState {
    GAME_MENU,
    GAME_ACTIVE,
    GAME_RESULT
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game
{
public:

    GameState     State;

    bool          paused = false;
    float         pausedTime = 0.0f;
    float         elapsedTime = 0.0f;
    long          pausedFrames = 0;
    long          elapsedFrames = 0;
    float         Time;
    long          Frames;

    bool          Keys[1024];
    bool          KeysProcessed[1024];

    bool          GamePadPresent[4];
    //bool          gamepad1ButtonA;
    float*        gamepad1Axis;
    float         prevGamepad1Axis[2];
    float         diffGamepad1Axis = 0;

    //bool          gamepad2ButtonA;
    float*        gamepad2Axis;
    float         prevGamepad2Axis[2];
    
    unsigned int  Width;
    unsigned int  Height;
    float wallWidth = Height / 25.0f;

    glm::vec2 PADSIZE = glm::vec2(10.0f, 50.0f);
    glm::vec2 PAD_VELOCITY = glm::vec2(800.0f, 1000.0f);
    float scaleFactor = 3.0f;

    GameObject* topWall;
    GameObject* bottomWall;
    GameObject* leftPad;
    GameObject* rightPad;

    // TESTING 
    bool leftReady = true;
    bool rightReady = true;

    bool pauseOnPadCollision = false;
    const float defaultLeftPadLimitX = Width / 4.0f;
    float leftPadLimitX = defaultLeftPadLimitX;
    const float defaultRightPadLimitX = 3.0f * Width / 4.0f - PADSIZE.x;
    float rightPadLimitX = defaultRightPadLimitX;

    // TODO: impulse to ball...
    float prevLeftPadX;
    float diffLeftPadX;

    unsigned int markedTime = 0;
    bool titleFadingOut = false;
    float fadingTime = 1.0f; // secs to fade titles

    bool goalScored = false;
    float kickTime = 1.5f; // secs to kick
    int scoreLeftInt = 0;
    char scoreLeft[3];
    int scoreRightInt = 0;
    char scoreRight[3];
    bool isLeftWinner;

    TASK(blink)

    static float percentage;
    static float distance;
    static float centerBoard;

    float BALL_RADIUS = 10.0f;
    BallObject* ball;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void PaintLetterBox(int fsWidth, int fsHeight, int fsStartX);

    void MoveBall(float dt);
    Direction VectorDirection(glm::vec2 target);
    Collision CheckCollision(BallObject& ball, GameObject& pad);
    void AdjustBallVelocity(BallObject& ball, GameObject& pad);
};

#endif