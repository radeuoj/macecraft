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
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "texture.h"
#include "camera.h"
#include "renderer.h"

#include "battery/embed.hpp"

unsigned int WIDTH = 1600;
unsigned int HEIGHT = 900;

GLfloat vertices[] =
{ //     COORDINATES  /  TexCoord   /        COLORS      /   //
    0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
    0.0f, 1.0f, 0.0f,  1.0f,  1.0f,

    0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    1.0f, 0.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 0.0f, 1.0f,  0.0f,  1.0f,

    0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    1.0f, 0.0f, 1.0f,  1.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 1.0f,  0.0f,  1.0f,

    0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
    1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 1.0f,  0.0f,  1.0f,

    0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    0.0f, 0.0f, 1.0f,  1.0f,  0.0f,
    0.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    0.0f, 1.0f, 1.0f,  1.0f,  1.0f,
    0.0f, 1.0f, 0.0f,  0.0f,  1.0f,

    1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  0.0f,  1.0f,
    1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    1.0f, 1.0f, 1.0f,  0.0f,  1.0f,
    1.0f, 1.0f, 0.0f,  1.0f,  1.0f,
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
}

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

    // VAO VAO1;
    // VAO1.Bind();

    // VBO VBO1(vertices, sizeof(vertices));
    // EBO EBO1(indices, sizeof(indices));

    // VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    // VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 9 * sizeof(float), (void*)(3 * sizeof(float)));

    // VAO1.Unbind();
    // VBO1.Unbind();
    // EBO1.Unbind();


    Renderer renderer;


    renderer.RenderVertex({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f});
    renderer.RenderVertex({1.0f, 0.0f, 0.0f}, {1.0f, 0.0f});
    renderer.RenderVertex({1.0f, 1.0f, 0.0f}, {1.0f, 1.0f});

    renderer.RenderVertex({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f});
    renderer.RenderVertex({1.0f, 1.0f, 0.0f}, {1.0f, 1.0f});
    renderer.RenderVertex({0.0f, 1.0f, 0.0f}, {0.0f, 1.0f});

    // Texture
    Texture mctex(RESOURCES_PATH "textures/mc.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);


    glEnable(GL_DEPTH_TEST);

    Camera camera(&WIDTH, &HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    double prevTime = glfwGetTime();

    // "Game loop"
    while (!glfwWindowShouldClose(window))
    {
        double curTime = glfwGetTime();
        float deltaTime = curTime - prevTime;
        prevTime = curTime;
        float FPS = 1.0f / deltaTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow(); // Show demo window! :)

        ImGui::Begin("Hello world");
        ImGui::Text("FPS: %.2f", FPS);
        ImGui::Text("position %.2f %.2f %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("orientation %.2f %.2f %.2f", camera.Orientation.x, camera.Orientation.y, camera.Orientation.z);
        ImGui::End();

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();


        camera.Inputs(window, deltaTime);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");


        mctex.Bind();


        // VAO1.Bind();


        renderer.Flush();

        // glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, std::size(vertices));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // VAO1.Delete();
    // VBO1.Delete();
    // EBO1.Delete();
    // mctex.Delete();
    shaderProgram.Delete();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}