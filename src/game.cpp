#include "filesystem.h"
#include <irrklang/irrKlang.h>
using namespace irrklang;

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "LineRenderer.h"

#include <iostream>
#include <tuple>
#include <cstdlib>
#include <ctime>

ISoundEngine* SoundEngine = createIrrKlangDevice();
SpriteRenderer* Renderer;
SpriteRenderer* RepeatRenderer;
TextRenderer* TextScore;
TextRenderer* TextMenu;
LineRenderer* AxisRenderer;
LineRenderer* ArrowRenderer;

float Game::percentage = 0.0f;
float Game::centerBoard = 0.0f;
float Game::distance = 0.0f;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height)
{}
Game::~Game()
{
    delete Renderer;
    SoundEngine->drop();
}

void Game::Init()
{
    srand(static_cast <unsigned> (time(0)));

    memset(prevGamepad1Axis, 0, sizeof(prevGamepad1Axis));
    memset(prevGamepad2Axis, 0, sizeof(prevGamepad2Axis));
    memset(prevGamepad2Axis, 0, sizeof(prevGamepad2Axis));


    Texture2D tBall, tPad, tWall;
    { // Create shaders & textures & fonts
        glm::mat4 projection = glm::ortho(0.0f, (float)Width, (float)Height, 0.0f, -1.0f, 1.0f);

        Shader spriteShader = ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", nullptr, "sprite");
        spriteShader.Use().SetInteger("image", 0);
        spriteShader.SetMatrix4("projection", projection);

        Shader axisShader = ResourceManager::LoadShader("shaders/axis.vs", "shaders/axis.fs", nullptr, "axis");
        axisShader.Use().SetMatrix4("projection", projection);

        Shader arrowShader = ResourceManager::LoadShader("shaders/arrow.vs", "shaders/arrow.fs", nullptr, "arrow");
        arrowShader.Use().SetMatrix4("projection", projection);

        // set render-specific controls
        Renderer = new SpriteRenderer(spriteShader);
        RepeatRenderer = new SpriteRenderer(spriteShader, true); // used for net
        AxisRenderer = new LineRenderer(axisShader);
        AxisRenderer->initAxisData();
        ArrowRenderer = new LineRenderer(arrowShader);
        ArrowRenderer->initArrowData();

        tBall = ResourceManager::LoadTexture("textures/tenis.png", true, "face");
        tPad = ResourceManager::LoadTexture("textures/paddle.png", true, "pad");
        tWall = ResourceManager::LoadTexture("textures/wall2.png", false, "wall");
        ResourceManager::LoadTexture("textures/net.png", true, "net");
        ResourceManager::LoadTexture("textures/grass.jpg", false, "grass");

        TextScore = new TextRenderer(Width, Height);
        TextScore->Load(FileSystem::getPath("fonts/OCRAEXT.TTF").c_str(), 72);

        TextMenu = new TextRenderer(Width, Height);
        TextMenu->Load(FileSystem::getPath("fonts/outrideracadital.ttf").c_str(), 32);
    }

    { // Create game objects
        topWall = new GameObject(glm::vec2(0.0f, 0.0f), glm::vec2((float)Width, wallWidth), tWall);
        bottomWall = new GameObject(glm::vec2(0.0f, (float)Height - wallWidth), glm::vec2((float)Width, wallWidth), tWall);

        float padsInitialY = Height / 2.0f - scaleFactor * PADSIZE.y / 2.0f;
        glm::vec2 leftPadPos = glm::vec2(0.0f, padsInitialY);
        leftPad = new GameObject(leftPadPos, scaleFactor * PADSIZE, tPad, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        glm::vec2 rightPadPos = glm::vec2(Width - scaleFactor * PADSIZE.x, padsInitialY);
        rightPad = new GameObject(rightPadPos, scaleFactor * PADSIZE, tPad, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec2 ballPos = glm::vec2(Width / 2.0f, Height / 2.0f);
        ball = new BallObject(ballPos, BALL_RADIUS, BallObject::RandomVelocity(), tBall);
    }
    { // Creat helpers objects

    }

    // TESTING
    SoundEngine->play2D(FileSystem::getPath("audio/menu.mp3").c_str(), true);
}

void Game::ProcessInput(float dt)
{
    glm::vec2 velocity = PAD_VELOCITY * dt;
    const float* axes;
    int axesCount;
    const unsigned char* buttons;
    int buttonCount;


    // TODO: Velocidad dependiente del Axis.
    // Más cerca del axis 1.0/-1.0, la velocidad aumenta más rápidamente.
    // Sería una especie de aceleración

    // Joystick1 - leftPad
    {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
        gamepad1Axis = (float*)axes;
        if (this->Keys[GLFW_KEY_W] || (GamePadPresent[0] && gamepad1Axis[1] < -0.1f)) {
            if (leftPad->Position.y >= 0.0f + wallWidth) {
                leftPad->Position.y -= velocity.y;
            }
        }
        if (this->Keys[GLFW_KEY_S] || (GamePadPresent[0] && gamepad1Axis[1] > 0.1f)) {
            if (leftPad->Position.y <= Height - leftPad->Size.y - wallWidth) {
                leftPad->Position.y += velocity.y;
            }
        }
        if (this->Keys[GLFW_KEY_D] || (GamePadPresent[0] && gamepad1Axis[0] > 0.1f)) {
            if (leftPad->Position.x >= 0.0f && leftPad->Position.x < leftPadLimitX) {
                leftPad->Position.x += velocity.x;
                if (leftPad->Position.x >= leftPadLimitX) {
                    leftPad->Position.x = leftPadLimitX;
                }
            }
        }
        if (this->Keys[GLFW_KEY_A] || (GamePadPresent[0] && gamepad1Axis[0] < -0.1f)) {
            if (leftPad->Position.x > 0.0f && leftPad->Position.x <= leftPadLimitX) {
                leftPad->Position.x -= velocity.x;
                if (leftPad->Position.x < 0.0f) {
                    leftPad->Position.x = 0.0f;
                }
            }
        }

        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        if (this->Keys[GLFW_KEY_E] || (GamePadPresent[0] && GLFW_PRESS == buttons[0])) {
            leftReady = true;
        }
    }

    // Joystick 2 - rightPad
    {
        axes = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &axesCount);
        gamepad2Axis = (float*)axes;
        if (this->Keys[GLFW_KEY_UP] || (GamePadPresent[1] && gamepad2Axis[1] < -0.1f))
        {
            if (rightPad->Position.y >= 0.0f + wallWidth) {
                rightPad->Position.y -= velocity.y;
            }
        }
        if (this->Keys[GLFW_KEY_DOWN] || (GamePadPresent[1] && gamepad2Axis[1] > 0.1f))
        {
            if (rightPad->Position.y <= Height - rightPad->Size.y - wallWidth) {
                rightPad->Position.y += velocity.y;
            }
        }
        if (this->Keys[GLFW_KEY_LEFT] || (GamePadPresent[1] && gamepad2Axis[0] < -0.1f))
        {
            if (rightPad->Position.x <= Width - rightPad->Size.x && rightPad->Position.x > rightPadLimitX) {
                rightPad->Position.x -= velocity.x;
                if (rightPad->Position.x < rightPadLimitX) {
                    rightPad->Position.x = rightPadLimitX;
                }
            }
        }
        if (this->Keys[GLFW_KEY_RIGHT] || (GamePadPresent[1] && gamepad2Axis[0] > 0.1f))
        {
            if (rightPad->Position.x <= Width - rightPad->Size.x && rightPad->Position.x >= rightPadLimitX) {
                rightPad->Position.x += velocity.x;
                if (rightPad->Position.x >= Width - rightPad->Size.x) {
                    rightPad->Position.x = Width - rightPad->Size.x;
                }
            }
        }

        buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &buttonCount);
        if (this->Keys[GLFW_KEY_ENTER] || (GamePadPresent[1] && GLFW_PRESS == buttons[0])) {
            rightReady = true;
        }
    }
}

Direction Game::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision Game::CheckCollision(BallObject& ball, GameObject& pad)
{
    glm::vec2 center(ball.Position + ball.Radius);
    glm::vec2 aabb_half_extents(pad.Size.x / 2.0f, pad.Size.y / 2.0f);
    glm::vec2 aabb_center(
        pad.Position.x + aabb_half_extents.x,
        pad.Position.y + aabb_half_extents.y
    );
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;

    //return glm::length(difference) < ball.Radius;
    if (glm::length(difference) <= ball.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void Game::AdjustBallVelocity(BallObject& ball, GameObject& pad)
{
    Game::centerBoard = pad.Position.y + pad.Size.y / 2.0f;
    float ballPosY;
    if (ball.Position.y < Game::centerBoard) {
        ballPosY = ball.Position.y + ball.Radius;
    }
    else {
        ballPosY = ball.Position.y;
    }

    // TODO: Comprobar que el porcentaje no es mayor que 1.0 ... no tiene sentido
    Game::distance = abs(ballPosY - Game::centerBoard);
    Game::percentage = Game::distance / (pad.Size.y / 2.0f);

    float strength = 3.0f;
    float velocity = abs(ball.Velocity.x) * Game::percentage * strength;

    if (velocity < 500.0f)
        velocity = 500.0f;
    if (velocity > 2000.0f)
        velocity = 2000.0f;

    if (ball.Velocity.x > 0)
        ball.Velocity.x = -velocity;
    else
        ball.Velocity.x = velocity;

}

void Game::Update(float dt)
{
    if (State == GAME_MENU || State == GAME_RESULT) {
        if (leftReady && rightReady) {
            if (markedTime == 0) {
                markedTime = (int)abs(Time);
                SoundEngine->stopAllSounds();

                // TESTING
                SoundEngine->play2D(FileSystem::getPath("audio/game.mp3").c_str(), true);
                scoreRightInt = scoreLeftInt = 0;
                titleFadingOut = true;
            }
            if (abs(Time) > markedTime + fadingTime) {
                markedTime = 0;
                titleFadingOut = false;
                State = GAME_ACTIVE;
                std::cout << "Game starts!" << std::endl;
            }
            //std::cout << "bothReadyTime:" << markedTime << " | Time:" << abs(Time) << std::endl;
        }

        TASK_DO_FOR(blink, 0.5f)
            TASK_NOT_FOR(blink, 0.5f)

            return;
    }

    if (goalScored) {
        if (markedTime == 0) {
            markedTime = (int)abs(Time);

            // TESTING
            SoundEngine->play2D("audio/whistle.mp3", false);
        }
        if (abs(Time) > markedTime + kickTime) {
            markedTime = 0;
            goalScored = false;
        }
        return;
    }

    BallObject::Result result = ball->Move(dt, Width, Height, wallWidth);
    if (result == BallObject::WALL_HIT) {
        SoundEngine->play2D("audio/bleep.mp3", false);
    }
    else if (result == BallObject::GOAL_LEFT || result == BallObject::GOAL_RIGHT) {
        if (result == BallObject::GOAL_LEFT) {
            scoreRightInt++;
        }
        else {
            scoreLeftInt++;
        }
        // TESTING
        SoundEngine->play2D("audio/goal.mp3", false);

        if (scoreRightInt == 5 || scoreLeftInt == 5) {
            State = GAME_RESULT;
            isLeftWinner = scoreLeftInt > scoreRightInt;
            SoundEngine->stopAllSounds();

            // TESTING
            SoundEngine->play2D(FileSystem::getPath("audio/menu.mp3").c_str(), true);
            leftReady = rightReady = false;
            
            return;
        }
        goalScored = true;
    }

    if (ball->leftPadCollision || ball->rightPadCollision) {
        if (ball->Position.x > 1.0f * Width / 4.0f && ball->Position.x < 3.0f * Width / 4.0f) {
            ball->leftPadCollision = ball->rightPadCollision = false;
            leftPadLimitX = defaultLeftPadLimitX;
            rightPadLimitX = defaultRightPadLimitX;
        }
    }

    Collision collisionLeftPad = CheckCollision(*ball, *leftPad);
    if (std::get<0>(collisionLeftPad)) {
        ball->leftPadCollision = true;
        if (pauseOnPadCollision) {
            paused = true;
        }

        //diffGamepad1Axis = gamepad1Axis[0] - prevGamepad1Axis[0];
        diffLeftPadX = leftPad->Position.x - prevLeftPadX;
        AdjustBallVelocity(*ball, *leftPad);
        ball->Position.x = leftPad->Position.x + leftPad->Size.x;

        //leftPadLimitX = leftPad->Position.x;
        SoundEngine->play2D(FileSystem::getPath("audio/bleep.mp3").c_str(), false);
    }

    Collision collisionRightPad = CheckCollision(*ball, *rightPad);
    if (std::get<0>(collisionRightPad)) {
        ball->rightPadCollision = true;
        if (pauseOnPadCollision) {
            paused = true;
        }
        AdjustBallVelocity(*ball, *rightPad);
        ball->Position.x = rightPad->Position.x - ball->Size.x;

        SoundEngine->play2D(FileSystem::getPath("audio/bleep.mp3").c_str(), false);
    }
}

void Game::Render()
{
    //Texture2D tGrass = ResourceManager::GetTexture("grass");
    // Renderer->DrawSprite(tGrass, glm::vec2(0.0f, 0.0f), glm::vec2((float)Width, (float)Height));
    topWall->Draw(*Renderer);
    bottomWall->Draw(*Renderer);
    
    Texture2D tNet = ResourceManager::GetTexture("net");
    float midScreen = Width / 2.0f;
    RepeatRenderer->DrawSprite(tNet, glm::vec2(midScreen - midScreen / 100.0f, wallWidth), glm::vec2(2.0f * midScreen / 100.0f, (float)Height - wallWidth));

    leftPad->Draw(*Renderer);
    rightPad->Draw(*Renderer);
    //AxisRenderer->DrawAxis(leftPad->Position, glm::vec2(20.0f, 20.0f));
    //AxisRenderer->DrawAxis(rightPad->Position, glm::vec2(20.0f, 20.0f));
    //ArrowRenderer->DrawArrow(rightPad->Position+rightPad->Size/2.0f, glm::vec2(100.0f, 20.0f), 135.0f);

    if (State == GAME_ACTIVE) {
        if (goalScored) {
            float rgb[3];
            for (int i = 0; i < 3; i++) {
                rgb[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f + 1.0f)));
            }
            glm::vec3 go = glm::vec3(rgb[0], rgb[1], rgb[2]);
            TextMenu->RenderText("Go!", 3.5f * Width / 8.f, Height / 2.0f, 2.0f, go);
        }
        else {
            ball->Draw(*Renderer);
            //AxisRenderer->DrawAxis(ball->Position, glm::vec2(20.0f, 20.0f));
            //ArrowRenderer->DrawArrow(ball->Position+ball->Size/2.0f, glm::vec2(50.0f, 20.0f), -45.0f);
        }
    }

    sprintf(scoreLeft, "%d", scoreLeftInt);
    TextScore->RenderText(scoreLeft, Width / 16.f, wallWidth + 10, 1.0f);
    sprintf(scoreRight, "%d", scoreRightInt);
    TextScore->RenderText(scoreRight, 14.0f * Width / 16.f, wallWidth + 10, 1.0f);

    if (State == GAME_MENU || State == GAME_RESULT) {
        glm::vec3 titleColor = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 pressButtonColor = glm::vec3(1.0f, 0.0f, 1.0f);
        glm::vec3 leftTextColor = leftPad->Color;
        glm::vec3 rightTextColor = rightPad->Color;

        if (titleFadingOut) {
            float rgb[3];
            for (int i = 0; i < 3; i++) {
                rgb[i] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f + 1.0f)));
            }
            titleColor = glm::vec3(rgb[0], rgb[1], rgb[2]);
            TextMenu->RenderText("Match starts!", 2.0f * Width / 8.f, Height / 2.0f, 2.0f, titleColor);
        }
        else {
            TextMenu->RenderText("Mega Pong", 2.3f * Width / 8.f, Height / 3.0f, 2.0f, titleColor);

            IF_TASK(blink)
                TextMenu->RenderText("Press A button to start", 2.f * Width / 8.f, Height / 2.0f, 1.0f, pressButtonColor);
            FI_TASK

                if (leftReady) {
                    TextMenu->RenderText("Left Player Ready!", Width / 16.f, 150.f + Height / 2.0f, 1.0f, leftTextColor);
                }
            if (rightReady) {
                TextMenu->RenderText("Right Player Ready!", 9 * Width / 16.f, 150.f + Height / 2.0f, 1.0f, rightTextColor);
            }

            if (State == GAME_RESULT) {
                char winner[128];
                sprintf(winner, "Winner player %s!", isLeftWinner ? "Left" : "Right");
                glm::vec3 winnerColor = isLeftWinner ? leftPad->Color : rightPad->Color;
                TextMenu->RenderText(winner, 2.f * Width / 8.f, 400.0f + Height / 2.0f, 1.0f, winnerColor);
            }
        }
    }
}

void Game::PaintLetterBox(int fsWidth, int fsHeight, int fsStartX)
{
    // This does not work...
    Texture2D tAny = ResourceManager::GetTexture("wall");
    Renderer->DrawSprite(tAny, glm::vec2(-(float)fsStartX, 0.0f), glm::vec2((float)fsStartX + 100.f, fsHeight), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}