#include "renderer2d.h"

#include "core/logging.h"
#include "math/math.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "platform/platform.h"
#include "camera.h"
#include "shader_paths.h"
#include "batch.h"

#include <glad/glad.h>

namespace R2D
{

static constexpr s32 maxSpriteCount = 20000;
static constexpr s32 maxTexCount = 10;
static s32 activeSlots[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

struct Vertex
{
    Vector3 position;
    Vector2 texCoord;
    f32 texIndex;
};

using R2DBatchData = BatchData<Vertex, maxTexCount>;

static struct
{
    const Camera* currentCamera = nullptr;

    R2DBatchData spriteBatch;
    u32 vao, vbo, ibo;
} r2dData;

void Init()
{
    {   // Init Sprite Batch

        // Compile Shaders
        AssertWithMessage(
            r2dData.spriteBatch.shader.CompileFromFile(r2dVertShaderPath, Shader::Type::VERTEX_SHADER),
            "Failed to compile Sprite Vertex Shader"
        );

        AssertWithMessage(
            r2dData.spriteBatch.shader.CompileFromFile(r2dFragShaderPath, Shader::Type::FRAGMENT_SHADER),
            "Failed to compile Sprite Fragment Shader"
        );

        AssertWithMessage(
            r2dData.spriteBatch.shader.Link(),
            "Failed to link Sprite Shader"
        );

        constexpr size_t batchSize = 4 * maxSpriteCount * sizeof(Vertex);

        r2dData.spriteBatch.elemVerticesBuffer = (Vertex*) PlatformAllocate(batchSize);

    }

    // Setup OpenGL Buffers and Arrays
    glGenVertexArrays(1, &r2dData.vao);
    glBindVertexArray(r2dData.vao);

    glGenBuffers(1, &r2dData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r2dData.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * maxSpriteCount * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (const void*) offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void*) offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, false, sizeof(Vertex), (const void*) offsetof(Vertex, texIndex));
    
    u32 indices[maxSpriteCount * 6];
    u32 offset = 0;
    for (int i = 0; i < maxSpriteCount * 6; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glGenBuffers(1, &r2dData.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r2dData.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Shutdown()
{
    PlatformFree(r2dData.spriteBatch.elemVerticesBuffer);
}

void Begin(const Camera& camera)
{
    r2dData.currentCamera = &camera;

    {   // Begin Sprite Batch
        r2dData.spriteBatch.elemVerticesPtr = r2dData.spriteBatch.elemVerticesBuffer;
        r2dData.spriteBatch.nextActiveTexSlot = 0;
        r2dData.spriteBatch.elemCount = 0;
    }
}

void End()
{
    R2DBatchData& batch = r2dData.spriteBatch;
    Shader& shader = batch.shader;

    shader.Bind();

    // Set View Projection for the batch
    Vector3 center = Vector3(r2dData.currentCamera->position.x, r2dData.currentCamera->position.y, 0.0f);
    Matrix4 viewProjection = r2dData.currentCamera->projection * r2dData.currentCamera->LookAtMatrix(center);
    shader.SetUniformMatrix4("u_viewProjection", viewProjection);    

    // Set all textures for the batch
    for (int i = 0; i < batch.nextActiveTexSlot; i++)
        batch.textures[i].Bind(i);

    shader.SetUniform1iv("u_textures", batch.nextActiveTexSlot, activeSlots);    

    glBindVertexArray(r2dData.vao);

    GLsizeiptr size = (u8*) batch.elemVerticesPtr - (u8*) batch.elemVerticesBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, r2dData.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch.elemVerticesBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r2dData.ibo);
    glDrawElements(GL_TRIANGLES, 6 * batch.elemCount, GL_UNSIGNED_INT, nullptr);
}

static void PushSprite(R2DBatchData& batch, const Sprite& sprite, const Matrix4& transform)
{
    if (batch.elemCount >= maxSpriteCount)
    {
        End();
        Begin(*r2dData.currentCamera);
    }

    // Find if texture has already been set to active
    int textureSlot = batch.nextActiveTexSlot;
    for (int i = 0; i < batch.nextActiveTexSlot; i++)
    {
        if (batch.textures[i].texID == sprite.atlas.texID)
        {
            textureSlot = i;
            break;
        }
    }

    if (textureSlot == batch.nextActiveTexSlot)
    {
        // End the batch if all the texture slots are occupied
        if (textureSlot >= maxTexCount)
        {
            End();
            Begin(*r2dData.currentCamera);

            textureSlot = 0;
        }

        batch.textures[textureSlot] = sprite.atlas;
        batch.nextActiveTexSlot++;
    }

    f32 top  = 1.0f - sprite.pivot.y;
    f32 left = -sprite.pivot.x;
    f32 bottom = -sprite.pivot.y;
    f32 right = 1.0f - sprite.pivot.x;

    f32 z = 0.0f;

    batch.elemVerticesPtr->position = transform * Vector3(left, bottom, z);
    batch.elemVerticesPtr->texCoord = Vector2(sprite.texCoords.s, sprite.texCoords.v);
    batch.elemVerticesPtr->texIndex = (f32) textureSlot;
    batch.elemVerticesPtr++;

    batch.elemVerticesPtr->position = transform * Vector3(right, bottom, z);
    batch.elemVerticesPtr->texCoord = Vector2(sprite.texCoords.u, sprite.texCoords.v);
    batch.elemVerticesPtr->texIndex = (f32) textureSlot;
    batch.elemVerticesPtr++;
    
    batch.elemVerticesPtr->position = transform * Vector3(right, top, z);
    batch.elemVerticesPtr->texCoord = Vector2(sprite.texCoords.u, sprite.texCoords.t);
    batch.elemVerticesPtr->texIndex = (f32) textureSlot;
    batch.elemVerticesPtr++;

    batch.elemVerticesPtr->position = transform * Vector3(left, top, z);
    batch.elemVerticesPtr->texCoord = Vector2(sprite.texCoords.s, sprite.texCoords.t);
    batch.elemVerticesPtr->texIndex = (f32) textureSlot;
    batch.elemVerticesPtr++;

    batch.elemCount++;
}

void RenderSprite(const Sprite& sprite, const Matrix4& transform)
{
    PushSprite(r2dData.spriteBatch, sprite, transform);
}

} // namespace R2D
