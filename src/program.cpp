#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "game.h"
#include "resource_manager.h"

#include <iostream>

// GLFW function declerations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Width and Height of the screen
const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;
GLfloat aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

int windowPos[2] = { 0, 0 };
int windowSize[2] = { SCREEN_WIDTH, SCREEN_HEIGHT };
GLFWmonitor* monitor;
GLFWwindow* window;
bool isFullScreen = false;
int fsWidth, fsHeight, fsStartX;

// Gamepad
bool check_gamepad_present(int number);

Game Pong(SCREEN_WIDTH, SCREEN_HEIGHT);
float currentFrameTime = 0.0f;
long realFrames = 0;
int fps = 0;
int pfps = 0;
int efps = 0;

bool imguiVisible = false;

int main(int argc, char* argv[])
{
    { // GLFW set up
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, false);

        window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);
        monitor = glfwGetPrimaryMonitor();

        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
        else {
            std::cout << "Success to initialize GLAD" << std::endl;
        }
    }
    { // Imgui set up
        std::cout << IMGUI_CHECKVERSION() << std::endl;
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui::GetStyle().Alpha = 0.75f;
    }
    bool show_imgui_demo = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    { // OpenGL configuration
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    { // Check gamepads and Init game
        Pong.GamePadPresent[0] = check_gamepad_present(GLFW_JOYSTICK_1);
        Pong.GamePadPresent[1] = check_gamepad_present(GLFW_JOYSTICK_2);
        Pong.GamePadPresent[2] = check_gamepad_present(GLFW_JOYSTICK_3);
        Pong.GamePadPresent[3] = check_gamepad_present(GLFW_JOYSTICK_4);
        Pong.Init();
        Pong.State = GAME_MENU;
    }

    // deltaTime variables
    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
    float accumDelta = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        currentFrameTime = (float)glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        if (deltaTime < 1 / 60.0f) {
            accumDelta += deltaTime;
            if (accumDelta >= 1 / 60.0f) {
                realFrames++;
                accumDelta = 0.0f;
                fps++;
                if ((fps - 30) == 30 || (fps - 30) == 0) {
                    //std::cout << "30/" << currentFrameTime << ":" << fps << std::endl;
                    if (Pong.GamePadPresent[0]) {
                        Pong.prevGamepad1Axis[0] = Pong.gamepad1Axis[0];
                        Pong.prevGamepad1Axis[1] = Pong.gamepad1Axis[1];

                        // TODO: Mejor tomar muestras de leftPad->Position.x para saber cuanto se ha movido
                        Pong.prevLeftPadX = Pong.leftPad->Position.x;
                    }
                }
                if (fps == 60) {
                    //std::cout << "60/" << currentFrameTime << ":" << fps << std::endl;
                    fps = 0;
                }
            }
        }

        glfwPollEvents();

        { // ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (show_imgui_demo)
                ImGui::ShowDemoWindow(&show_imgui_demo);
            if (imguiVisible) {
                ImGui::Begin("Debugging Pong");

                if (ImGui::CollapsingHeader("Time")) {
                    ImGui::Text("Real Time: %.3f ", currentFrameTime); ImGui::SameLine();
                    ImGui::Text("Real Frames: %d", realFrames); ImGui::SameLine();
                    ImGui::Text("60 Frames count: %d", fps);
                    ImGui::Text("Pong Time: %.3f ", Pong.Time); ImGui::SameLine();
                    ImGui::Text("Pong Frames: %d", Pong.Frames);
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Gamepad 1")) {

                    ImGui::SliderFloat2("Axis [Horizontal, vertical] : ", Pong.gamepad1Axis, -1.0f, 1.0f);
                    ImGui::SliderFloat2("Prev [Horizontal, vertical] : ", Pong.prevGamepad1Axis, -1.0f, 1.0f);
                    ImGui::Text("Collision DiffAxis: %.3f ", Pong.diffGamepad1Axis);
                    ImGui::Text("Collision DiffLeftPadX: %.3f ", Pong.diffLeftPadX);
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Ball")) {
                    if (ImGui::TreeNode("Position & Velocity")) {
                        float* ballPosition = glm::value_ptr(Pong.ball->Position);
                        ImGui::SliderFloat2("(px, py) : position", ballPosition, 0.0f, (float)Pong.Width);
                        float* ballVelocity = glm::value_ptr(Pong.ball->Velocity);
                        ImGui::SliderFloat2("(vx, vy) : velocity", ballVelocity, -2000.0f, 2000.0f);
                        ImGui::Checkbox("leftPadCollision", &Pong.ball->leftPadCollision);
                        ImGui::SameLine();
                        ImGui::Checkbox("rightPadCollision", &Pong.ball->rightPadCollision);
                        ImGui::TreePop();
                        ImGui::Separator();
                    }
                    if (ImGui::TreeNode("Adust velocity calculus")) {
                        ImGui::DragFloat("centerBoard##%5.2f", &Pong.centerBoard);
                        ImGui::DragFloat("d##%5.2f", &Pong.distance);
                        ImGui::DragFloat("%##%.3f", &Pong.percentage);
                        ImGui::Checkbox("pauseOnPadCollision", &Pong.pauseOnPadCollision);
                        ImGui::TreePop();
                        ImGui::Separator();
                    }
                }
                if (ImGui::CollapsingHeader("Pads")) {
                    if (ImGui::TreeNode("Velocity")) {
                        float* padsVelocity = glm::value_ptr(Pong.PAD_VELOCITY);
                        ImGui::SliderFloat2("(vx, vy) : velocity", padsVelocity, 800.0f, 2000.0f);
                        ImGui::TreePop();
                        ImGui::Separator();
                    }
                    if (ImGui::TreeNode("LeftPad")) {
                        float* leftPadPos = glm::value_ptr(Pong.leftPad->Position);
                        ImGui::SliderFloat2("(px, py) : position", leftPadPos, 0.0f, (float)Pong.Width);
                        ImGui::TreePop();
                        ImGui::Separator();
                    }
                    if (ImGui::TreeNode("RightPad")) {
                        float* rightPadPos = glm::value_ptr(Pong.rightPad->Position);
                        ImGui::SliderFloat2("(px, py) : position", rightPadPos, 0.0f, (float)Pong.Width);
                        ImGui::TreePop();
                        ImGui::Separator();
                    }
                }
                if (ImGui::CollapsingHeader("Misc ")) {
                    ImGui::SliderFloat("rightPad length", &Pong.rightPad->Size.y, 1.0f, 300.0f);
                    ImGui::SliderFloat("wall length", &Pong.wallWidth, 5.0f, 200.0f);
                    ImGui::ColorEdit3("Background color", (float*)&clear_color);
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Imgui")) {
                    ImGui::Text("Framerate: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    ImGui::PushItemWidth(100);
                    ImGui::SliderFloat("Imgui transparency", &ImGui::GetStyle().Alpha, 0.4f, 1.0f);
                    ImGui::SameLine();
                    ImGui::Checkbox("Demo Window", &show_imgui_demo);
                }
                ImGui::End();
            }
        }

        // Pong game has its own Time & Frames, since we have the ability to stop time
        Pong.Time = Pong.paused ? Pong.pausedTime - Pong.elapsedTime : currentFrameTime - Pong.elapsedTime;
        Pong.Frames = Pong.paused ? Pong.pausedFrames - Pong.elapsedFrames : realFrames - Pong.elapsedFrames;

        if (!Pong.paused) {
            Pong.Update(deltaTime);
        }
        Pong.ProcessInput(deltaTime);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        if (isFullScreen) {
            // TODO: Solve how to do this
            // One way it would be to have a sprite in the background
            Pong.PaintLetterBox(fsWidth, fsHeight, fsStartX);
        }

        Pong.Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    { // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ResourceManager::Clear();
        glfwTerminate();
    }
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        Pong.paused = !Pong.paused;
        if (Pong.paused) {
            Pong.pausedTime = currentFrameTime;
            Pong.pausedFrames = realFrames;
            pfps = fps;
        }
        else {
            Pong.elapsedTime += (currentFrameTime - Pong.pausedTime);
            Pong.elapsedFrames += (realFrames - Pong.pausedFrames);
            efps += (fps - pfps);
        }
    }

    // F11 controls full screen toggle
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {

        if (isFullScreen) {
            // restore last window size and position
            glfwSetWindowMonitor(window, nullptr, windowPos[0], windowPos[1], windowSize[0], windowSize[1], GLFW_DONT_CARE);
        }
        else {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            // backup window position and window size
            glfwGetWindowPos(window, &windowPos[0], &windowPos[1]);
            glfwGetWindowSize(window, &windowSize[0], &windowSize[1]);

            // Set full-screen
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        isFullScreen = !isFullScreen;
    }

    if (glfwGetKey(window, GLFW_KEY_F12) == GLFW_PRESS) {
        imguiVisible = !imguiVisible;
    }


    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Pong.Keys[key] = true;
        else if (action == GLFW_RELEASE) {
            Pong.Keys[key] = false;
            Pong.KeysProcessed[key] = false;
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.

    std::cout << "glViewport was called: " << width << "," << height << std::endl;
    if (!isFullScreen) {
        float aspect = (float)height / (float)SCREEN_HEIGHT;
        fsWidth = (int)(SCREEN_WIDTH * aspect);
        fsHeight = height;
        fsStartX = (int)((width - fsWidth) / 2.0f);
        glViewport(fsStartX, 0, fsWidth, fsHeight);
        std::cout << "Calculated: " << fsWidth << "," << fsHeight << "," << fsStartX << std::endl;
    }
    else
        glViewport(0, 0, width, height);
}

bool check_gamepad_present(int number)
{
    int joyPresent = glfwJoystickPresent(number);
    if (joyPresent == GLFW_TRUE) {
        std::cout << "Gamepad " << number << " present" << std::endl;
        const char* name = glfwGetGamepadName(number);
        std::cout << "Gamepad:" << name << std::endl;
    }
    else {
        std::cout << "Joystick " << number << " NOT present" << std::endl;
    }

    return joyPresent;
}