#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "chunk.h"
#include "renderer.h"

#include "battery/embed.hpp"

unsigned int WIDTH = 1600;
unsigned int HEIGHT = 900;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

constexpr int S = 4;
Chunk chunks[S][S];

int main()
{
    // Setup glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Initialize a glfw window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Macecraft", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // VSYNC
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    gladLoadGL();

    // Viewport
    glViewport(0, 0, WIDTH, HEIGHT);


    Shader shaderProgram(b::embed<"embed/shaders/default.vert">().data(), b::embed<"embed/shaders/default.frag">().data());


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    Renderer renderer;

    // Texture
    Texture dirtTex(RESOURCES_PATH "textures/dirt.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);
    Texture mcTex(RESOURCES_PATH "textures/mc.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);

    dirtTex.Bind();

    Camera camera(&WIDTH, &HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    // for (int i = 0; i < 128; i++)
    // {
    //     for (int j = 0; j < 128; j++)
    //     {
    //         for (int k = 0; k < 3; k++)
    //         {
    //             renderer.RenderBlock({i, k, j}, {1.0f, 1.0f, 1.0f}, dirtTex);
    //         }
    //     }
    // }

    for (int i = 0; i < S; i++)
    {
        for (int j = 0; j < S; j++)
        {
            chunks[i][j] = Chunk({i, j});
        }
    }


    double prevTime = glfwGetTime();
    double lastTime = glfwGetTime();
    int FPS = 0;
    int frameCount = 0;

    // "Game loop"
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        float deltaTime = currentTime - prevTime;
        prevTime = currentTime;
        frameCount++;

        if (currentTime - lastTime > 1.0)
        {
            lastTime = currentTime;
            FPS = frameCount;
            frameCount = 0;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow(); // Show demo window! :)

        glClearColor(0.67f, 0.85f, 0.90f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();


        camera.Inputs(window, deltaTime);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");


        // renderer.RenderSprite({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, mcTex);


        // for (int i = 0; i < 128; i++)
        // {
        //     for (int j = 0; j < 128; j++)
        //     {
        //         for (int k = 0; k < 3; k++)
        //         {
        //             renderer.RenderBlock({i, k, j}, {1.0f, 1.0f, 1.0f}, dirtTex);
        //         }
        //     }
        // }

        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                chunks[i][j].Render(renderer);
            }
        }

        VertexData vertices[] =
        {
            { 1, 0, 0, 0, 0, 0 },
            { 1, 0, 1, 0, 0, 1 },
            { 1, 0, 0, 1, 0, 2 },
            { 1, 0, 1, 0, 0, 1 },
            { 1, 0, 0, 1, 0, 2 },
            { 1, 0, 1, 1, 0, 3 },
        };

        renderer.RenderVertices(vertices, std::size(vertices));

        renderer.Flush();



        // mcTex.Bind();



        ImGui::Begin("Hello world");
        ImGui::Text("FPS: %d", FPS);
        ImGui::Text("position %.2f %.2f %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("orientation %.2f %.2f %.2f", camera.Orientation.x, camera.Orientation.y, camera.Orientation.z);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    renderer.Delete();
    dirtTex.Delete();
    shaderProgram.Delete();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}