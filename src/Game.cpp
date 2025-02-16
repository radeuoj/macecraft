#include "Game.h"

#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "World.h"

#include <typeinfo>

namespace Macecraft
{

Game::Game()
{
    InitGLFW();
    InitImGui();
    InitOpenGL();

    std::string vertSource = get_file_contents("res/shaders/default.vert");
    std::string fragSource = get_file_contents("res/shaders/default.frag");
    m_DefaultShader = std::make_unique<Shader>(vertSource.data(), fragSource.data());
    m_DefaultShader->Activate();

    m_DefaultAtlas = std::make_unique<TextureAtlas>("res/textures/dirt.png", 16, 16);
    m_DefaultAtlas->Bind();

    m_World = std::make_unique<World>(m_DefaultAtlas.get());
}

void Game::InitGLFW()
{
    // Setup glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize a glfw window
    m_Window = glfwCreateWindow(width, height, m_WindowTitle, nullptr, nullptr);
    if (m_Window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        Game *game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        game->width = width;
        game->height = height;
        glViewport(0, 0, width, height);
    });

    // VSYNC
    glfwSwapInterval(1);
}

void Game::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Game::InitOpenGL()
{
    gladLoadGL();
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

Game::~Game()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Game::Run()
{
    
    // TODO: Timer class

    double prevTime = glfwGetTime();
    double lastTime = glfwGetTime();
    m_FPS = 0;
    int frameCount = 0;
    float frameTimeTotal = 0;

    // "Game loop"
    while (!glfwWindowShouldClose(m_Window))
    {
        double currentTime = glfwGetTime();
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;
        frameCount++;

        if (currentTime - lastTime > 1.0)
        {
            lastTime += 1.0;
            m_FPS = frameCount;
            m_FrameTime = frameTimeTotal / float(frameCount) * 1000.0f;
            frameCount = 0;
            frameTimeTotal = 0;

            UpdateOncePerSecond();
        }

        glClearColor(0.67f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Camera.Inputs(m_Window, deltaTime);
        m_Camera.Matrix(m_DefaultShader.get(), "camMatrix");

        // m_Frustum.UpdateFromCamera(m_Camera);
        m_Frustum.UpdateFromViewProjMatrix(m_Camera.viewproj);
        
        Update(deltaTime);

        UpdateImGui(deltaTime);

        frameTimeTotal += glfwGetTime() - prevTime;

        glfwSwapBuffers(m_Window);

        glfwPollEvents();
    }
}

void Game::Update(float deltaTime)
{
    m_World->GenerateChunksIfNeeded(m_Camera.position);
    m_World->RenderChunks(m_DefaultShader.get(), m_Camera.position, m_Frustum);
}

// TODO: fix this by adding a limit to  how many chunks you can delete per frame
void Game::UpdateOncePerSecond()
{
    m_World->CleanupChunks(m_Camera.position);
}

void Game::UpdateImGui(float deltaTime)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // ImGui::ShowDemoWindow(); // Show demo window! :)

    ImGui::Begin("Hello world");
    ImGui::Text("FPS: %d time: %.2fms", m_FPS, m_FrameTime);
    ImGui::Text("position %.2f %.2f %.2f", m_Camera.position.x, m_Camera.position.y, m_Camera.position.z);
    ImGui::Text("orientation %.2f %.2f %.2f", m_Camera.orientation.x, m_Camera.orientation.y, m_Camera.orientation.z);
    ImGui::Text("%d chunks loaded (hopefully)", m_World->chunks.size());
    ImGui::Text("%d chunks flushed this frame", m_World->chunksFlushedThisFrame);
    ImGui::Text("Normal: %.2f %.2f Distance %.2f", m_Frustum.nearPlane.normal.x, m_Frustum.nearPlane.normal.y, m_Frustum.nearPlane.distance);
    ImGui::Checkbox("Enable frustum culling", &m_World->ENABLE_FRUSTUM_CULLING);
    ImGui::NewLine();

    ImGui::SliderFloat("Sprint speed", &Camera::SPRINT_SPEED, 0.0f, 1000.0f);
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
} // Macecraft