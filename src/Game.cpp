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

#include "battery/embed.hpp"

namespace Macecraft
{

    // constexpr int S = 4;
    // Chunk chunks[S][S];

    Game::Game()
    {
        initGLFW();
        initImGui();
        initOpenGL();

        m_GlobalRenderer.init();
        m_DefaultShader.init(b::embed<"embed/shaders/default.vert">().data(), b::embed<"embed/shaders/default.frag">().data());
        dirtTex.init(RESOURCES_PATH "textures/dirt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);
        m_World = std::make_unique<World>();
        m_World->init();
    }

    void Game::initGLFW()
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

    void Game::initImGui()
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

    void Game::initOpenGL()
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

    void Game::run()
    {
        // Texture
        // Texture dirtTex(RESOURCES_PATH "textures/dirt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);
        // Texture mcTex(RESOURCES_PATH "textures/mc.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);

        dirtTex.bind();

        for (Chunk& chunk : m_World->chunks)
        {
            chunk.render();
        }

        double prevTime = glfwGetTime();
        double lastTime = glfwGetTime();
        m_FPS = 0;
        int frameCount = 0;

        // "Game loop"
        while (!glfwWindowShouldClose(m_Window))
        {
            double currentTime = glfwGetTime();
            float deltaTime = currentTime - prevTime;
            prevTime = currentTime;
            frameCount++;

            if (currentTime - lastTime > 1.0)
            {
                lastTime = currentTime;
                m_FPS = frameCount;
                frameCount = 0;
            }

            glClearColor(0.67f, 0.85f, 0.90f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_DefaultShader.activate();

            m_Camera.inputs(m_Window, deltaTime);
            m_Camera.matrix(45.0f, 0.1f, 1000.0f, m_DefaultShader, "camMatrix");

            update(deltaTime);

            m_GlobalRenderer.flush();

            // printf("position %.2f %.2f %.2f\n", m_Camera.position.x, m_Camera.position.y, m_Camera.position.z);

            updateImGui(deltaTime);

            glfwSwapBuffers(m_Window);

            glfwPollEvents();
        }

        dirtTex.Delete();
    }

    void Game::update(float deltaTime)
    {
        for (Chunk& chunk : m_World->chunks)
        {
            chunk.m_Renderer.flush();
        }
    }

    void Game::updateImGui(float deltaTime)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow(); // Show demo window! :)

        ImGui::Begin("Hello world");
        ImGui::Text("FPS: %d", m_FPS);
        ImGui::Text("position %.2f %.2f %.2f", m_Camera.position.x, m_Camera.position.y, m_Camera.position.z);
        ImGui::Text("orientation %.2f %.2f %.2f", m_Camera.orientation.x, m_Camera.orientation.y, m_Camera.orientation.z);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
} // Macecraft