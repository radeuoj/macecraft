#undef min
#undef max
#include "PlayerLayer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "imgui.h"
#include "WorldLayer.h"

namespace Macecraft {

static WorldLayer* s_World;

void PlayerLayer::OnLoad()
{
    s_World = game.GetLayer<WorldLayer>();

    std::string vertSource = get_file_contents("res/shaders/ui.vert");
    std::string fragSource = get_file_contents("res/shaders/ui.frag");
    m_UIShader = std::make_unique<Shader>(vertSource.data(), fragSource.data());
    m_UIShader->Activate();

    m_PointedBlockOutlineRenderer.LinkAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), offsetof(VertexData, x));
    m_PointedBlockOutlineRenderer.LinkAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), offsetof(VertexData, texCoords));

    m_StaticUIRenderer.LinkAttribPointer(0, 2, GL_FLOAT, false, sizeof(UIVertexData), offsetof(UIVertexData, position));
    m_StaticUIRenderer.LinkAttribPointer(1, 3, GL_FLOAT, false, sizeof(UIVertexData), offsetof(UIVertexData, color));

    // My best attempt for a crosshair
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.015f, -0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.005f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.015f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.015f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.005f, -0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.015f, -0.01f), glm::vec3(0.0f) });
    //
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.015f, -0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.005f, -0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.015f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.015f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(-0.005f, 0.01f), glm::vec3(0.0f) });
    // m_StaticUIRenderer.vertices.push_back(UIVertexData { glm::vec2(0.015f, -0.01f), glm::vec3(0.0f) });

    m_StaticUIRenderer.PushIndices(
        m_StaticUIRenderer.PushVertex(glm::vec2(-0.01, -0.01), glm::vec3(0.0f)),
        m_StaticUIRenderer.PushVertex(glm::vec2(0.01, 0.01), glm::vec3(0.0f)),
        m_StaticUIRenderer.PushVertex(glm::vec2(0.01, -0.01), glm::vec3(0.0f)),
        m_StaticUIRenderer.PushVertex(glm::vec2(-0.01, 0.01), glm::vec3(0.0f))
    );
    m_StaticUIRenderer.BindVertices();
}

void PlayerLayer::OnUpdate(float deltaTime)
{
    m_Velocity = glm::vec3(0.0f);
    
    HandleInputs(deltaTime);
    HandleBlockLookingAt();
    
    m_Position += m_Velocity * deltaTime;
    game.GetCamera()->position = m_Position;
    game.GetCamera()->orientation = m_Orientation;
    
    game.GetShader()->Activate();
}

static bool s_IsPlayerLookingAtSomething = false;
static glm::ivec2 s_HitChunkPos = glm::ivec2(0);
static glm::ivec3 s_HitBlockPos = glm::ivec3(0);

void PlayerLayer::OnRender(float deltaTime)
{
    if (s_IsPlayerLookingAtSomething)
        DrawBlockOutline(s_HitChunkPos, s_HitBlockPos);
}

void PlayerLayer::DrawBlockOutline(glm::ivec2 chunkPos, glm::ivec3 blockPos)
{
    m_PointedBlockOutlineRenderer.vertices.clear();

    auto bottomLeftBack = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z, 0);
    auto bottomRightBack = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z, 0);
    auto bottomLeftFront = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z + 1, 0);
    auto bottomRightFront = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z + 1, 0);
    auto topLeftBack = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z, 0);
    auto topRightBack = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z, 0);
    auto topLeftFront = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z + 1, 0);
    auto topRightFront = m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1, 0);

    m_PointedBlockOutlineRenderer.PushIndices(
        // Bottom
        bottomLeftBack, bottomRightBack,
        bottomLeftFront, bottomRightFront,
        bottomLeftBack, bottomLeftFront,
        bottomRightBack,  bottomRightFront,

        // Top
        topLeftBack, topRightBack,
        topLeftFront, topRightFront,
        topLeftBack, topLeftFront,
        topRightBack,  topRightFront,

        // Sides
        bottomLeftBack, topLeftBack,
        bottomRightBack, topRightBack,
        bottomLeftFront, topLeftFront,
        bottomRightFront, topRightFront
    );

    // // Bottom
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z + 1, 0);
    //
    // // Top
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1, 0);
    //
    // // Sides
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x, blockPos.y + 1, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y, blockPos.z + 1, 0);
    // m_PointedBlockOutlineRenderer.PushVertex(blockPos.x + 1, blockPos.y + 1, blockPos.z + 1, 0);
    
    m_PointedBlockOutlineRenderer.BindVertices();
    glUniform2i(glGetUniformLocation(game.GetShader()->GetID(), "chunkPos"), chunkPos.x, chunkPos.y);
    glLineWidth(4.5f);
    m_PointedBlockOutlineRenderer.Flush(GL_LINES);
    glLineWidth(1.0f);
}

void PlayerLayer::OnUIRender(float deltaTime)
{
    m_UIShader->Activate();
    glLineWidth(2.5f);
    m_StaticUIRenderer.Flush(GL_LINES);
    glLineWidth(1.0f);
}

static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
static bool s_FirstClick = true;

void PlayerLayer::HandleInputs(float deltaTime)
{
    glm::vec3 horizontalOrientation = glm::normalize(glm::vec3(m_Orientation.x, 0.0f, m_Orientation.z));

    float speed = 0.0f;
    
    if (glfwGetKey(game.GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed = SPRINT_SPEED;
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        speed = NORMAL_SPEED;
    }
    
    if (glfwGetKey(game.GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
    {
        m_Velocity += speed * horizontalOrientation;
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
    {
        m_Velocity += speed * -horizontalOrientation;
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
    {
        m_Velocity += speed * -glm::normalize(glm::cross(horizontalOrientation, UP));
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
    {
        m_Velocity += speed * glm::normalize(glm::cross(horizontalOrientation, UP));
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        m_Velocity += speed * UP;
    }

    if (glfwGetKey(game.GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        m_Velocity += speed * -UP;
    }

    if (glfwGetMouseButton(game.GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (s_FirstClick)
        {
            glfwSetCursorPos(game.GetWindow(), game.width / 2, game.height / 2);
            s_FirstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(game.GetWindow(), &mouseX, &mouseY);

        float rotX = SENSITIVITY * (float)(mouseY - (float)game.height / 2) / (float)game.height;
        float rotY = SENSITIVITY * (float)(mouseX - (float)game.width / 2) / (float)game.width;

        glm::vec3 newm_Orientation = glm::rotate(m_Orientation, glm::radians(-rotX), glm::normalize(glm::cross(m_Orientation, UP)));

        if (!(glm::angle(newm_Orientation, UP) <= glm::radians(5.0f) or glm::angle(newm_Orientation, -UP) <= glm::radians(5.0f)))
        {
            m_Orientation = newm_Orientation;
        }

        m_Orientation = glm::rotate(m_Orientation, glm::radians(-rotY), UP);

        glfwSetCursorPos(game.GetWindow(), (float)game.width / 2, (float)game.height / 2);
    }
    else if (glfwGetMouseButton(game.GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        s_FirstClick = true;
    }
}

static glm::ivec3 blockPlayerIsLookingAt = glm::ivec3(0);

void PlayerLayer::HandleBlockLookingAt()
{
    BlockType hitBlock;
    // glm::ivec2 hitChunkPos;
    // glm::ivec3 hitPosInChunk;
    if (s_World->Raycast(m_Position, m_Orientation, 10.0f, &hitBlock, &s_HitChunkPos, &s_HitBlockPos))
    {
        s_IsPlayerLookingAtSomething = true;
        blockPlayerIsLookingAt = glm::ivec3(s_HitChunkPos.x * Chunk::SIZE, 0, s_HitChunkPos.y * Chunk::SIZE) + s_HitBlockPos;
    }
    else
    {
        s_IsPlayerLookingAtSomething = false;
        blockPlayerIsLookingAt = glm::ivec3(0);
    }
}

void PlayerLayer::OnImGuiRender(float deltaTime)
{
    ImGui::Text("Player velocity: %.2f", glm::length(m_Velocity));
    ImGui::Text("Player looking at: %d %d %d", blockPlayerIsLookingAt.x, blockPlayerIsLookingAt.y, blockPlayerIsLookingAt.z);
    ImGui::SliderFloat("Sprint speed", &SPRINT_SPEED, 0.0f, 1000.0f);
}



}
